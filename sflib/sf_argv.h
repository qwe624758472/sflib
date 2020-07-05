
/**
* @version 1.0.0
* @author skyfire
* @file sf_argv.h

* 
*/

#pragma once

#include "sf_stdc++.h"
#include "sf_json.hpp"
#include "sf_error.h"
#include "sf_multi_value.h"

namespace skyfire {

/**
 * @brief 参数解析时的动作
 *
 */
enum class sf_argv_action {
    store, //保存值
    store_true, //保存为true
    store_false, // 保存为false
    count // 保存数量
};

/**
 * @brief 参数选项
 *
 */
struct sf_argv_opt_t {
    std::string short_name {}; // 短名称，如:-i
    std::string long_name {}; // 长名称，如:--id
    std::string json_name {}; // 保存在json中的键名称，如：id
    sf_json_type type = sf_json_type::string; // 参数类型（参见sf_json_type）
    bool required = true; // 是否必须
    sf_argv_action action { sf_argv_action::store }; // 动作
    sf_json default_value {}; // 默认值
};

using sf_argv_result_t = sf_multi_value<sf_err,sf_json>;

/**
 * @brief 参数解析类
 *
 */
class sf_argparser : public std::enable_shared_from_this<sf_argparser> {
private:
    std::unordered_map<std::string, std::shared_ptr<sf_argparser>> sub_parsers_;
    std::vector<sf_argv_opt_t> position_arg_;
    std::vector<sf_argv_opt_t> none_position_arg__;
    std::pair<std::string, std::string> prefix_  {"-", "--"};
    std::string help_;

    sf_argv_result_t parse_argv__(const std::vector<std::string>& argv) ;

    sf_argparser(const std::string& help = "");

public:
    /**
     * 创建一个解析器对象
     * @param help 帮助信息（TODO 输出help信息）
     * @param prefix 前缀
     * @return 解析器对象
     */
    static std::shared_ptr<sf_argparser> make_parser(
        const std::string& help = "");

    /**
     * @brief 添加一个参数
     *
     * @param opt 参数信息
     * @return true 添加成功
     * @return false 添加失败
     */
    bool add_argument(sf_argv_opt_t opt);

    /**
     * @brief 添加一个参数
     *
     * @param short_name 短名称
     * @param long_name 长名称
     * @param type 类型
     * @param required 是否必须
     * @param default_value 默认值
     * @param json_name json键名称
     * @param action 动作
     * @return true 添加成功
     * @return false 添加失败
     */
    bool add_argument(std::string short_name, std::string long_name,
        sf_json_type type = sf_json_type::string,
        bool required = true, sf_json default_value = sf_json(),
        std::string json_name = "",
        sf_argv_action action = sf_argv_action::store);

    /**
     * @brief 添加子解析器
     *
     * @param name 解析器名称（会被用于键名称）
     * @param sub_parser 子解析器
     */
    void add_sub_parser(const std::string& name,
        std::shared_ptr<sf_argparser> sub_parser);

    /**
     * @brief 解析参数
     *
     * @param argc 参数数量
     * @param argv 参数列表
     * @param skip0 跳过参数0
     * @return sf_argv_result_t 解析结果
     */
    sf_argv_result_t parse_argv(int argc, const char** argv, bool skip0 = true) ;

    /**
     * @brief 解析参数
     *
     * @param args 参数列表
     * @return sf_argv_result_t 解析结果
     */
    sf_argv_result_t parse_argv(const std::vector<std::string>& args, bool skip0 = true) ;

    friend void prepare_parser__(std::shared_ptr<sf_argparser> &parser);
};

} // namespace skyfire