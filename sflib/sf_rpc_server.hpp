
/**
* @version 1.0.0
* @author skyfire
* @file sf_rpc_server.hpp
*/

/*
 * sf_rpc_server RPC服务器
 */

#pragma once

#include "sf_rpc_server.h"
#include "sf_rpc_utils.h"

namespace skyfire {
template <typename _Type>
void sf_rpc_server::send_back__(const SOCKET sock, const int id_code,
    _Type data)
{
    sf_rpc_res_context_t res;
    res.call_id = id_code;
    res.ret = skyfire::to_json(data);
    sf_debug("call ret", skyfire::to_json(res));
    tcp_server__->send(sock, rpc_res_type,
        to_byte_array(skyfire::to_json(res).to_string()));
}

inline void sf_rpc_server::on_data_coming__(const SOCKET sock,
    const sf_pkg_header_t& header,
    const byte_array& data)
{
    if (header.type != rpc_req_type) {
        return;
    }
    std::string id;
    byte_array param;
    sf_rpc_req_context_t req;
    sf_debug("call req", to_string(data));
    from_json(sf_json::from_string(to_string(data)), req);
    for (auto& p : func_vec__) {
        p(sock, req);
    }
}

template <typename _Func>
void sf_rpc_server::reg_rpc_func(const std::string& id, _Func func)
{
    static_assert(
        !sf_check_param_reference<decltype(std::function(func))>::value,
        "Param can't be reference");
    static_assert(!sf_check_param_pointer<decltype(std::function(func))>::value,
        "Param can't be pointer");
    static_assert(
        !sf_check_return_reference<decltype(std::function(func))>::value,
        "Return can't be reference");
    static_assert(
        !sf_check_return_pointer<decltype(std::function(func))>::value,
        "Return can't be pointer");

    using _Ret = typename sf_function_type_helper<decltype(func)>::return_type;
    using _Param = typename sf_function_type_helper<decltype(func)>::param_type;
    // auto 让 constexpr-if 生效
    auto f = [this, id, func](SOCKET s, const auto& req) {
        if (req.func_id == id) {
            _Param param;
            from_json(req.params, param);
            if constexpr (std::is_same<_Ret, void>::value) {
                sf_invoke(func, param);
                send_back__(s, req.call_id, '\0');
            } else {
                _Ret ret = sf_invoke(func, param);
                send_back__(s, req.call_id, ret);
            }
        }
    };
    func_vec__.push_back(f);
}

inline sf_rpc_server::sf_rpc_server()
{
    sf_bind(
        sf_rpc_server::tcp_server__, data_coming,
        [this](SOCKET sock, const sf_pkg_header_t& header,
            const byte_array& data) {
            on_data_coming__(sock, header, data);
        },
        true);
    sf_bind(
        sf_rpc_server::tcp_server__, new_connection,
        [this](SOCKET sock) {
            {
                std::unique_lock<std::shared_mutex> lck(mu_client_list__);
                client_list__.insert(sock);
            }
            client_connected(sock);
        },
        true);
    sf_bind(
        sf_rpc_server::tcp_server__, closed,
        [this](SOCKET sock) {
            {
                std::unique_lock<std::shared_mutex> lck(mu_client_list__);
                client_list__.erase(sock);
            }
            client_disconnected(sock);
        },
        true);
}

inline bool sf_rpc_server::listen(const std::string& ip,
    unsigned short port) const
{
    return tcp_server__->listen(ip, port);
}

inline void sf_rpc_server::close() const { tcp_server__->close(); }
} // namespace skyfire
