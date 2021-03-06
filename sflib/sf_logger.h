
/**
* @version 1.0.0
* @author skyfire
* @file sf_logger.h
*/

/*
 * logger日志打印
 */

#pragma once

#include "sf_stdc++.h"

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include "sf_json.h"
#include "sf_random.hpp"
#include "sf_single_instance.hpp"
#include "sf_utils.hpp"

namespace skyfire
{

/**
 * @brief  日志等级
 */

constexpr int SF_DEBUG_LEVEL = 0;
constexpr int SF_INFO_LEVEL  = 1;
constexpr int SF_WARN_LEVEL  = 2;
constexpr int SF_ERROR_LEVEL = 3;
constexpr int SF_FATAL_LEVEL = 4;

/**
 *  @brief 日志信息
 */
struct logger_info_t
{
    int         level;     // 日志等级
    std::string time;      // 时间
    int         line;      // 行号
    std::string file;      // 文件名称
    std::string thread_id; // 线程号
    std::string func;      // 函数名称
    std::string msg;       // 消息
};

SF_JSONIFY(logger_info_t, level, time, line, file, thread_id, func, msg);

/**
 * 各种level对应的字符串
 */
inline std::unordered_map<int, std::string> logger_level_str__ {
    { SF_DEBUG_LEVEL, "DEBUG" },
    { SF_INFO_LEVEL, "INFO " },
    { SF_WARN_LEVEL, "WARN " },
    { SF_ERROR_LEVEL, "ERROR" },
    { SF_FATAL_LEVEL, "FATAL" },
};

/**
 *  @brief 默认日志格式
 */
constexpr char default_log_format[]
    = "{level} {time} {thread} {file}:{line} {func} --> {msg}\n";

/**
 * 日志类
 */
class logger
{
public:
    SF_SINGLE_TON(logger)

    /**
     * 添加指定等级的回调函数
     * @param level 等级
     * @param func 函数
     * @return id号（可用于移除回调）
     */
    int add_level_func(int                                             level,
                       std::function<void(const logger_info_t&, bool)> func, bool color = true);

    /**
     * 添加指定等级日志输出流
     * @param level 等级
     * @param os 输出流
     * @param format 格式化字符串
     * @return id号（可用于移除回调）
     */
    int add_level_stream(int level, std::ostream* os,
                         std::string format = default_log_format, bool color = true);

    /**
     * 添加指定等级日志输出文件
     * @param level 等级
     * @param filename 文件名
     * @param format 格式化字符串
     * @return id号（可用于移除回调）
     */
    int add_level_file(int level, const std::string& filename,
                       std::string format = default_log_format, bool color = true);

    /**
     * 根据id删除过滤器
     * @param key id
     */
    void remove_filter(int key);

    template <typename T>
    void logout(int level, const std::string& file, int line,
                const std::string& func, const T& dt);

    template <typename... T>
    void logout(int level, const std::string& file, int line,
                const std::string& func, const T&... dt);

    void stop_logger();

    static void empty_func__() {}

    static std::string format(std::string          format_str,
                              const logger_info_t& log_info, bool color);

private:
    struct log_attr
    {
        std::function<void(const logger_info_t&, bool)> callback;
        bool                                            colored = true;
    };
    std::deque<logger_info_t> log_deque__;
    std::condition_variable   cond__;
    std::mutex                deque_mu__;
    std::unordered_map<
        int, std::unordered_map<int, log_attr>>
        logger_func_set__;
    std::atomic<bool>            run__ { true };
    std::recursive_mutex         func_set_mutex__;
    std::shared_ptr<std::thread> log_thread__ = nullptr;

    bool check_key_can_use__(int key);

    int make_random_logger_id__();

    logger();

    ~logger();

    template <typename T, typename... U>
    void logout__(std::ostringstream& oss, logger_info_t& log_info,
                  const T& tmp, const U&... tmp2);

#ifdef QT_CORE_LIB
    template <typename... U>
    void logout__(std::ostringstream& oss, logger_info_t& log_info,
                  const QString& tmp, const U&... tmp2);
    // template<>
    void logout__(std::ostringstream& oss, logger_info_t& log_info,
                  const QString& tmp);
#endif
    template <typename T>
    void logout__(std::ostringstream& oss, logger_info_t& log_info,
                  const T& tmp);
};

#ifdef SF_DEBUG
#define sf_debug(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_DEBUG_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#else
#define sf_debug(...) 
#endif

#define sf_info(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_INFO_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_warn(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_WARN_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_err(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_ERROR_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_fatal(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_FATAL_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)

} // namespace skyfire
