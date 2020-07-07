
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_nat_traversal_utils.hpp
*/

#pragma once

#include "sf_tcp_nat_traversal_utils.h"

namespace skyfire {

inline bool sf_tcp_nat_traversal_connection::send(
    const byte_array &data) const {
    if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
        return client__->send(data);
    }
    return server__->send(sock__, data);
}

inline bool sf_tcp_nat_traversal_connection::send(
    int type, const byte_array &data) const {
    if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
        return client__->send(type, data);
    }
    return server__->send(sock__, type, data);
}

inline sf_tcp_nat_traversal_connection::sf_tcp_nat_traversal_connection(
    std::shared_ptr<sf_tcp_client> client,
    std::shared_ptr<sf_tcp_server> server, int sock, std::string connect_id,
    sf_tcp_nat_traversal_connection_type type)
    : client__(std::move(client)),
      server__(std::move(server)),
      sock__(sock),
      connect_id__(connect_id),
      type__(type) {
    if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
        sf_bind(
            client__, data_coming,
            [this](const sf_pkg_header_t &header, const byte_array &data) {
                data_coming(header, data);
            },
            true);
        sf_bind(
            client__, raw_data_coming,
            [this](const byte_array &data) { raw_data_coming(data); }, true);
        sf_bind(
            client__, closed, [this]() { closed(); }, true);
    } else {
        sf_bind(
            server__, data_coming,
            [this](SOCKET, const sf_pkg_header_t &header,
                   const byte_array &data) { data_coming(header, data); },
            true);
        sf_bind(
            server__, raw_data_coming,
            [this](SOCKET, const byte_array &data) { raw_data_coming(data); },
            true);
        sf_bind(
            server__, closed, [this](SOCKET) { closed(); }, true);
    }
}

}    // namespace skyfire
