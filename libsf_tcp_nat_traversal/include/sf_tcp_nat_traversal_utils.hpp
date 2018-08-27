#pragma once

#include "sf_tcp_nat_traversal_utils.h"

namespace skyfire {

    inline bool sf_tcp_nat_traversal_connection::send(const byte_array &data) {
        if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
            return client__->send(data);
        }
        return server__->send(sock__, data);
    }

    inline bool sf_tcp_nat_traversal_connection::send(int type, const byte_array &data) {
        if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
            return client__->send(type, data);
        }
        return server__->send(sock__, type, data);
    }

    inline sf_tcp_nat_traversal_connection::sf_tcp_nat_traversal_connection(std::shared_ptr <sf_tcpclient> client,
                                                                     std::shared_ptr <sf_tcpserver> server, int sock,
                                                                     int connect_id,
                                                                     sf_tcp_nat_traversal_connection_type type) :
            client__(std::move(client)),
            server__(std::move(server)),
            sock__(sock),
            connect_id__(connect_id),
            type__(type) {
        if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
            sf_bind_signal(client__,
                           data_coming,
                           [=](const pkg_header_t &header, const byte_array &data) {
                               data_coming(header, data);
                           }, true);
            sf_bind_signal(client__,
                           raw_data_coming,
                           [=](const byte_array &data) {
                               raw_data_coming(data);
                           }, true);
            sf_bind_signal(client__,
                           closed,
                           [=]() {
                               closed();
                           },
                           true);
        } else {
            sf_bind_signal(server__,
                           data_coming,
                           [=](SOCKET, const pkg_header_t &header, const byte_array &data) {
                               data_coming(header, data);
                           }, true);
            sf_bind_signal(server__,
                           raw_data_coming,
                           [=](SOCKET, const byte_array &data) {
                               raw_data_coming(data);
                           }, true);
            sf_bind_signal(server__,
                           closed,
                           [=](SOCKET) {
                               closed();
                           },
                           true);
        }
    }

}
