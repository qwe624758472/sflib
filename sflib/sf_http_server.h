
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_server.h
*/

#pragma once

#include "sf_stdc++.h"
#include "sf_http_base_server.hpp"
#include "sf_http_part_router.h"
#include "sf_http_router.hpp"
#include "sf_http_static_router.hpp"
#include "sf_router_shared_ptr_compare.hpp"
#include "sf_websocket_router.hpp"
#include "sf_utils.h"

namespace skyfire {
/**
 * @brief  HTTP服务器框架，丰富基础框架的接口
 */

class http_server final
    : public make_instance_t<http_server, http_base_server>,
      public std::enable_shared_from_this<http_server> {
   private:
    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        http_routers__;

    std::multiset<std::shared_ptr<websocket_router>,
                  router_shared_ptr_compare__<websocket_router>>
        websocket_routers__;

    void default_request_callback__(const http_request &req,
                                    http_response &res);

    void default_websocket_request_callback__(const http_request &req,
                                              http_response &res) const;

    void default_websocket_binary_data_callback__(SOCKET sock,
                                                  const std::string &url,
                                                  const byte_array &data);

    void default_websocket_text_data_callback__(SOCKET sock,
                                                const std::string &url,
                                                const std::string &data);

    void default_websocket_open_callback__(SOCKET sock, const std::string &url);

    void default_websocket_close_callback__(SOCKET sock,
                                            const std::string &url);

   public:
    explicit http_server(
        const http_server_config &config = http_server_config{});

    /**
     * 添加http路由
     * @param router http路由
     */
    void add_router(const std::shared_ptr<router> &router);

    /**
     * 添加websocket路由
     * @param router websocket路由
     */
    void add_router(const std::shared_ptr<websocket_router> &router);

    friend websocket_router;
};
}    // namespace skyfire