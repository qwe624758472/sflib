
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_utils.h
*/
#pragma once

#include "sf_http_request_line.h"
#include "sf_json.hpp"
#include "sf_stdc++.h"
#include "sf_tcp_utils.h"
#include "sf_type.h"
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <zconf.h>
#include <zlib.h>

namespace skyfire
{
constexpr char session_id_key[] = "_SESSION_ID";

class http_server_req_multipart;

/**
 * @brief  http头原始类型
 */
using http_header_t = std::unordered_map<std::string, std::string>;

using http_param_t = std::unordered_map<std::string, std::string>;

/**
 * @brief  websocket追加的uuid
 */
constexpr char websocket_sha1_append_str[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/**
 * @brief  请求上下文
 */
struct request_context_t
{
    bool       new_req = false; // 是否是新情求
    byte_array buffer;          // buffer
};

SF_JSONIFY(request_context_t, new_req, buffer)

/**
 *  @brief websocket上下文
 */
struct websocket_context_t final
{
    std::string url;         // url
    SOCKET      sock;        // socket
    byte_array  buffer;      // 原始buffer（未解析）
    byte_array  data_buffer; // 解析出来的buffer
};

SF_JSONIFY(websocket_context_t, url, sock, buffer, data_buffer)

/**
 *  @brief 分块请求上下文
 */
struct http_server_req_multipart_data_context_t final
{
    SOCKET                                 sock;         // socket
    std::string                            boundary_str; // 分解字符串
    http_header_t                          header;       // 头
    http_request_line                      request_line; // 请求行
    std::vector<http_server_req_multipart> multipart;    // 分块信息
};

/**
 * @brief  cookie生存期类型
 */
enum class cookie_life_type
{
    session    = 0, // session生存期
    time_point = 1  // 时间点生存期
};

/**
 *  @brief cookie
 */
struct http_cookie_item_t final
{
    std::string                           key;                                           // 键
    std::string                           value;                                         // 值
    cookie_life_type                      life_type  = cookie_life_type ::session;       // 生存期类型
    std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now(); // 生存期
    std::string                           path       = "/";                              // 路径
    bool                                  secure     = false;                            // 安全性
    bool                                  http_only  = true;                             // http only
};

/**
 * @brief  响应类型
 */
enum class http_data_type
{
    normal,    // 普通类型
    file,      // 文件类型
    multipart, // multipart类型
    stream
};

/**
 * @brief
 * 响应文件信息（当响应类型为file或者响应类型为multipart，子类型为file时，使用改数据结构声明文件）
 */
struct http_file_info_t final
{
    std::string       filename;
    long long         begin;
    long long         end;
    long unsigned int file_size;
};

/**
 * @brief  multipart类型信息
 */
struct http_multipart_info_t final
{
    /**
     *  @brief multipart子类型
     */
    enum class multipart_info_type
    {
        form, // 表单
        file  // 文件
    };
    /**
     *  @brief 表单信息，当type为multipart_info_type::form时有效
     */
    struct form_info_t final
    {
        http_header_t header; // 响应头
        byte_array    body;   // 响应体
    };

    multipart_info_type
        type; // multipart子类型，当响应类型为multipart时有效
    form_info_t
        form_info; // 表单信息，当type为multipart_info_type::form时有效
    http_file_info_t
        file_info; // 响应文件信息，当type为multipart_info_type::file时有效
};

//////////////////////////////////////////////////////////////////////////////

/**
 * 数字转为16进制字符，如10 转为a
 * @param x 数字
 * @return 字符
 */
inline unsigned char to_hex(unsigned char x);

/**
 * 16进制字符转为数字，如a转为10
 * @param x 字符
 * @return 数字
 */
inline unsigned char from_hex(unsigned char x);

/**
 * url编码
 * @param str 字符串
 * @param decode_plus 是否编码‘+’
 * @return 编码后的字符串
 */
inline std::string url_encode(const std::string& str,
                              bool               decode_plus = true);

/**
 * url解码
 * @param str 字符串
 * @param decode_plus 是否解码‘+’
 * @return 解码后的字符串
 */
inline std::string url_decode(const std::string& str,
                              bool               decode_plus = true);

/**
 * 解析表单参数
 * @param param_str 要解析的参数字符串
 * @return 解析出来的参数
 */
http_param_t parse_param(std::string param_str);

/**
 * 服务器侧解析url
 * @param raw_url 原始url
 * @param url url
 * @param param 参数
 * @param frame 锚点信息
 */
inline void parse_server_req_url(const std::string& raw_url, std::string& url,
                                 http_param_t& param, std::string& frame);

/**
 * @brief 客户端侧解析url
 * 
 * @param raw_url 原始url
 * @param agreement 协议
 * @param host 主机名
 * @param port 端口
 * @param resource 请求资源
 */
inline void parse_client_req_url(std::string raw_url, std::string& agreement, std::string& host, short& port, std::string& resource);

/**
 * 生成http时间字符串
 * @param tp 时间点
 * @return http时间字符串
 */
inline std::string make_http_time_str(
    const std::chrono::system_clock::time_point& tp = std::chrono::system_clock::now() + std::chrono::hours(8));

/**
 * 转为http头格式格式
 * @param key 原始字符串
 * @return http头格式字符串
 */
inline std::string to_header_key_format(std::string key);

/**
 * 读取文件
 * @param filename 文件名
 * @param max_size 最大长度
 * @return 读取结果
 */
inline byte_array read_file(const std::string& filename,
                            size_t             max_size = 4096);

/**
 * base64编码
 * @param data 数据
 * @return 编码后的base64字符串
 */
inline std::string base64_encode(const byte_array& data);

/**
 * base64解码
 * @param data base64数据
 * @return 解码后的数据
 */
inline byte_array base64_decode(const std::string& data);

/**
 * sha1编码
 * @param data 数据
 * @return sha1编码后的数据
 */
inline byte_array sha1_encode(const byte_array& data);

/**
 * deflate压缩
 * @param input_buffer 缓冲区
 * @return 压缩后的字节数组
 */
inline byte_array deflate_compress(const byte_array& input_buffer);
} // namespace skyfire
