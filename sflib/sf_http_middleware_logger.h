#pragma once

#include "sf_http_middleware.h"
#include "sf_utils.h"

namespace skyfire
{
/**
     * @brief http 日志中间件
     * 
     */
class http_middleware_logger : public make_instance_t<http_middleware_logger, http_middleware>
{
    bool before(const http_server_request& http_server_request, http_server_response& res) override;
    bool after(const http_server_request& http_server_request, http_server_response& res) override;
};
}