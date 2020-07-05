
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_server_linux.hpp
*/

#pragma once

#include "sf_define.h"
#include "sf_net_utils.hpp"
#include "sf_tcp_server_interface.hpp"
#include "sf_tcp_server_linux.h"

namespace skyfire {
inline sf_tcp_server::sf_tcp_server(bool raw, int thread_count):raw__(raw), thread_count__(thread_count) { 
    if(thread_count__ < 1){
        thread_count__ = 1;
    }
 }

inline SOCKET sf_tcp_server::raw_socket() { return listen_fd__; }

inline epoll_context_t* sf_tcp_server::find_context__(SOCKET sock) const
{
    auto& epoll_data = epoll_data__();
    {
        std::shared_lock<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
        if (epoll_data.sock_context__.count(sock) != 0) {
            return &epoll_data;
        }
    }
    {
        std::shared_lock<std::shared_mutex> lck(mu_context_pool__);
        for (auto& p : context_pool__) {
            {
                std::shared_lock<std::shared_mutex> lck(p->mu_epoll_context__);
                if (p->sock_context__.count(sock) != 0) {
                    return p;
                }
            }
        }
    }
    sf_debug(sock, "socket fd not found");
    return nullptr;
}

inline bool sf_tcp_server::send(int sock, const byte_array& data)
{
    sf_debug("send", data.size());
    auto psock_context_data__ = find_context__(sock);
    if (psock_context_data__ == nullptr) {
        sf_debug("psock_context_data__ is nullptr");
        return false;
    }

    std::shared_lock<std::shared_mutex> lck(psock_context_data__->mu_epoll_context__);
    auto& sock_context__ = psock_context_data__->sock_context__;

    auto send_data = data;
    lck.unlock();
    before_raw_send_filter__(sock, send_data);
    lck.lock();
    sock_context__[sock].data_buffer_out.push_back(send_data);
    sock_context__[sock].ev.events |= EPOLLOUT;

    return epoll_ctl(psock_context_data__->epoll_fd, EPOLL_CTL_MOD, sock,
               &sock_context__[sock].ev)
        != -1;
}

inline bool sf_tcp_server::send(int sock, int type, const byte_array& data)
{
    auto psock_context_data__ = find_context__(sock);
    if (psock_context_data__ == nullptr) {
        sf_debug("psock_context_data__ is nullptr");
        return false;
    }
    std::shared_lock<std::shared_mutex> lck(psock_context_data__->mu_epoll_context__);
    auto& sock_context__ = psock_context_data__->sock_context__;
    sf_pkg_header_t header {};
    header.type = htonl(type);
    header.length = htonl(data.size());
    make_header_checksum(header);
    auto tmp_data = data;

    lck.unlock();
    before_send_filter__(sock, header, tmp_data);
    auto send_data = make_pkg(header) + tmp_data;
    before_raw_send_filter__(sock, send_data);
    lck.lock();

    sock_context__[sock].data_buffer_out.push_back(send_data);

    sock_context__[sock].ev.events |= EPOLLOUT;

    return epoll_ctl(psock_context_data__->epoll_fd, EPOLL_CTL_MOD, sock,
               &sock_context__[sock].ev)
        != -1;
}

inline void sf_tcp_server::close(SOCKET sock)
{
    ::shutdown(sock, SHUT_RDWR);
    ::close(sock);
}

inline void sf_tcp_server::close()
{
    shutdown(listen_fd__, SHUT_RDWR);
    ::close(listen_fd__);
    listen_fd__ = -1;

    for (auto& p : thread_vec__) {
        p.join();
    }
}

inline bool sf_tcp_server::listen(const std::string& ip, unsigned short port)
{
    listen_fd__ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd__ == -1) {
        return false;
    }

    listen_sock_filter__(listen_fd__);

    if (fcntl(listen_fd__, F_SETFL,
            fcntl(listen_fd__, F_GETFD, 0) | O_NONBLOCK)
        == -1) {
        return false;
    }

    int opt = 1;
    if (-1 == setsockopt(listen_fd__, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void*>(&opt), sizeof(opt))) {
        return false;
    }

    if (-1 == setsockopt(listen_fd__, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const void*>(&opt), sizeof(opt))) {
        return false;
    }

    sockaddr_in internet_addr {};
    internet_addr.sin_family = AF_INET;
    internet_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    internet_addr.sin_port = htons(port);

    if (::bind(listen_fd__, reinterpret_cast<sockaddr*>(&internet_addr),
            sizeof(sockaddr_in))
        == -1) {
        return false;
    }

    if (::listen(listen_fd__, max_tcp_connection) == -1) {
        return false;
    }

    thread_vec__.emplace_back(
        std::thread(&sf_tcp_server::work_thread__, this, true, listen_fd__));

    if (manage_clients__) {
        for (int i = 1; i < thread_count__; ++i) {
            thread_vec__.emplace_back(std::thread(&sf_tcp_server::work_thread__,
                this, true, listen_fd__));
        }
    }
    return true;
}

inline void sf_tcp_server::work_thread__(bool listen_thread, SOCKET listen_fd)
{
    sf_debug("start thread");
    auto& epoll_data = epoll_data__();
    epoll_data.epoll_fd = epoll_create(max_tcp_connection);
    {
        std::unique_lock<std::shared_mutex>
            lck(mu_context_pool__);
        context_pool__.push_back(&epoll_data);
        sf_debug("create epoll_data, sock_context__", &epoll_data.sock_context__, context_pool__.size());
    }

    {
        std::lock_guard<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
        auto& sock_context__ = epoll_data.sock_context__;

        if (listen_thread) {
            sock_context__[listen_fd] = sock_data_context_t {};
            sock_context__[listen_fd].ev.events = EPOLLIN | EPOLLET;
            sock_context__[listen_fd].ev.data.fd = listen_fd;

            if (epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_ADD, listen_fd,
                    &sock_context__[listen_fd].ev)
                < 0) {
                sf_debug("add to epoll error");
                close(listen_fd);
                return;
            }
        }
    }
    std::vector<epoll_event> evs(max_tcp_connection);

    while (true) {
        int wait_fds = 0;
        if ((wait_fds = epoll_wait(epoll_data.epoll_fd, evs.data(),
                 max_tcp_connection, -1))
            == -1) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        if (wait_fds == 0) {
            continue;
        }

        sf_debug("new epoll event", wait_fds);

        auto listen_err = false;

        for (auto i = 0; i < wait_fds; ++i) {
            if (evs[i].data.fd == listen_fd__ && (evs[i].events & EPOLLIN)) {
                if (!handle_accept__()) {
                    listen_err = true;
                }
            } else if (evs[i].events & EPOLLIN) {
                handle_read__(evs[i]);
            } else if (evs[i].events & EPOLLOUT) {
                handle_write__(evs[i]);
            }
        }
        if (listen_err) {
            sf_debug("listen error");
            break;
        }
    }
}

inline sf_tcp_server::~sf_tcp_server() { close(); }

inline bool sf_tcp_server::in_dispatch__(SOCKET fd)
{
    sf_debug("add new fd", fd);
    auto& epoll_data = epoll_data__();
    std::lock_guard<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
    auto& sock_context__ = epoll_data.sock_context__;
    sock_context__[fd] = sock_data_context_t {};
    sock_context__[fd].ev.events = EPOLLIN | EPOLLET;
    sock_context__[fd].ev.data.fd = fd;

    sf_debug("add sock to sock_context__", fd, &sock_context__);

    if (epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_ADD, fd,
            &sock_context__[fd].ev)
        < 0) {
        sf_debug("add to epoll error");
        close(fd);
        return false;
    }
    return true;
}

inline bool sf_tcp_server::handle_accept__()
{
    int conn_fd = 0;
    sockaddr_in client_addr {};
    socklen_t len = sizeof(client_addr);
    while ((conn_fd = accept(listen_fd__, (struct sockaddr*)&client_addr,
                &len))
        > 0) {
        new_connection_filter__(conn_fd);
        if (manage_clients__) {
            if (fcntl(conn_fd, F_SETFL,
                    fcntl(conn_fd, F_GETFD, 0) | O_NONBLOCK)
                == -1) {
                sf_debug("set no block error");
                close(conn_fd);
                return true;
            }
            if (!in_dispatch__(conn_fd)) {
                return true;
            }
        }
        sf_debug("new connection", conn_fd);
        new_connection(conn_fd);
    }

    if (errno == EAGAIN || errno == EINTR) {
        return true;
    } else {
        sf_debug("accept error");
        {
            auto& epoll_data = epoll_data__();
            std::lock_guard<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
            epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_DEL, listen_fd__,
                &epoll_data.sock_context__[listen_fd__].ev);
            epoll_data.sock_context__.erase(listen_fd__);
        }
        return false;
    }
}

inline void sf_tcp_server::handle_read__(const epoll_event& ev)
{
    byte_array recv_buf(sf_default_buffer_size);
    sf_pkg_header_t header {};
    auto& epoll_data = epoll_data__();
    auto& sock_context__ = epoll_data.sock_context__;
    while (true) {
        sf_debug("start read", ev.data.fd);
        auto count_read = static_cast<int>(
            recv(ev.data.fd, recv_buf.data(), sf_default_buffer_size, 0));
        sf_debug("read", count_read);
        if (count_read <= 0) {
            sf_debug("errno", errno);
            // EWOULDBLOCK == EAGAIN
            if ((errno == EAGAIN || errno == EINTR /* || errno == EWOULDBLOCK */) && count_read < 0) {
                sf_debug("read finished", errno);
                break;
            } else {
                disconnect_sock_filter__(ev.data.fd);
                {
                    std::lock_guard<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
                    epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_DEL,
                        ev.data.fd, &sock_context__[ev.data.fd].ev);
                    sock_context__.erase(ev.data.fd);
                }
                closed(ev.data.fd);
                close(ev.data.fd);
                sf_debug("read error / connection closed");
                break;
            }
        }
        recv_buf.resize(static_cast<unsigned long>(count_read));
        after_raw_recv_filter__(ev.data.fd, recv_buf);
        if (raw__) {
            sf_debug("raw data", recv_buf.size());
            raw_data_coming(ev.data.fd, recv_buf);
            sf_debug("after resolve");
        } else {
            std::shared_lock<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
            sock_context__[ev.data.fd].data_buffer_in.insert(
                sock_context__[ev.data.fd].data_buffer_in.end(),
                recv_buf.begin(), recv_buf.end());
            size_t read_pos = 0;
            while (sock_context__[ev.data.fd].data_buffer_in.size() - read_pos >= sizeof(sf_pkg_header_t)) {
                memmove(
                    &header,
                    sock_context__[ev.data.fd].data_buffer_in.data() + read_pos,
                    sizeof(header));
                if (!check_header_checksum(header)) {
                    lck.unlock();
                    disconnect_sock_filter__(ev.data.fd);
                    lck.lock();
                    close(ev.data.fd);
                    closed(ev.data.fd);
                    break;
                }
                header.length = ntohl(header.length);
                header.type = ntohl(header.type);
                if (sock_context__[ev.data.fd].data_buffer_in.size() - read_pos - sizeof(header) >= header.length) {
                    byte_array data = { byte_array(
                        sock_context__[ev.data.fd].data_buffer_in.begin() + read_pos + sizeof(header),
                        sock_context__[ev.data.fd].data_buffer_in.begin() + read_pos + sizeof(header) + header.length) };
                    read_pos += sizeof(header) + header.length;
                    lck.unlock();
                    after_recv_filter__(ev.data.fd, header, data);
                    data_coming(ev.data.fd, header, data);
                    lck.lock();
                } else {
                    break;
                }
            }
            if (read_pos != 0) {
                sock_context__[ev.data.fd].data_buffer_in.erase(
                    sock_context__[ev.data.fd].data_buffer_in.begin(),
                    sock_context__[ev.data.fd].data_buffer_in.begin() + read_pos);
            }
        }
    }
}

inline void sf_tcp_server::handle_write__(const epoll_event& ev)
{
    auto& epoll_data = epoll_data__();
    auto& sock_context__ = epoll_data.sock_context__;
    SOCKET fd = ev.data.fd;
    {
        std::shared_lock<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
        if (sock_context__[fd].data_buffer_out.empty()) {
            sf_debug("sock", fd, "empty", &sock_context__[fd].data_buffer_out);
            return;
        }
    }
    while (true) {
        byte_array p;
        {
            std::shared_lock<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
            if (sock_context__[fd].data_buffer_out.empty()) {
                lck.unlock();
                write_finished(fd);
                lck.lock();
                sock_context__[fd].ev.events &= ~EPOLLOUT;
                epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_MOD, fd,
                    &sock_context__[fd].ev);
                sf_debug(fd, "write_finished",
                    sock_context__[fd].data_buffer_out.size());
                break;
            }
            p = sock_context__[fd].data_buffer_out.front();
        }

        sf_debug("pkg size:", p.size());
        auto data_size = p.size();
        auto n = data_size;
        int tmp_write;
        bool error_flag = false;
        while (n > 0) {
            tmp_write = static_cast<unsigned long>(
                ::send(fd, p.data() + data_size - n, n, MSG_NOSIGNAL));
            if (tmp_write == -1 && errno != EAGAIN) {
                write_error(fd);
                error_flag = true;
                break;
            }
            sf_debug("write", tmp_write);
            n -= tmp_write;
        }
        if (error_flag) {
            sf_debug("write error");
            disconnect_sock_filter__(fd);
            std::lock_guard<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
            epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_DEL, fd,
                &sock_context__[fd].ev);
            close(fd);
            closed(fd);
            sock_context__.erase(fd);
            break;
        } else {
            std::shared_lock<std::shared_mutex> lck(epoll_data.mu_epoll_context__);
            if (n == 0) {
                sf_debug("pop front");
                sf_debug(fd, "before",
                    sock_context__[fd].data_buffer_out.size());
                sock_context__[fd].data_buffer_out.pop_front();
                sf_debug(fd, "after",
                    sock_context__[fd].data_buffer_out.size());
            } else {
                sock_context__[fd].data_buffer_out.front() = {
                    sock_context__[fd].data_buffer_out.front().end() - n,
                    sock_context__[fd].data_buffer_out.front().end()
                };
                sf_debug(fd, "write pendding",
                    sock_context__[fd].data_buffer_out.size());
                break;
            }
        }
    }
}

inline epoll_context_t& sf_tcp_server::epoll_data__() const
{
    thread_local static epoll_context_t d;
    return d;
}

} // namespace skyfire