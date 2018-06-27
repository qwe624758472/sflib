/*
 * sf_rpcserver RPC服务器
 */

#pragma once

#include "sf_tcpserver.h"
#include "sf_nocopy.h"
#include "sf_serialize.h"
#include "sf_meta.h"
#include <string>
#include <functional>
#include <tuple>
#include <memory>


namespace skyfire {

    template<typename _BaseClass = sf_empty_class>
    class sf_rpcserver : public sf_nocopy<_BaseClass> {
    private:

        std::shared_ptr<sf_tcpserver> __tcp_server__ = sf_tcpserver::make_server();

        std::vector<std::function<void(SOCKET, int, const std::string &, const byte_array &)>> __func__vec__;

        template<typename _Type>
        void __send_back(SOCKET sock, int id_code, const std::string &id_str, _Type data) {
            __tcp_server__->send(sock, id_code, sf_serialize(id_str) + sf_serialize(data));
        }


        void __on_data_coming(SOCKET sock, const pkg_header_t &header, const byte_array &data) {
            std::string id;
            byte_array param;
            size_t pos = sf_deserialize(data, id, 0);
            for (auto &p : __func__vec__) {
                p(sock, header.type, id, byte_array(data.begin() + pos, data.end()));
            }
        }


    public:

        /**
         * @brief reg_rpc_func 注册远程调用函数
         * @param id 标识
         * @param func 函数
         */
        template<typename _Func>
        void reg_rpc_func(const std::string &id, _Func func) {
            if constexpr (std::is_bind_expression<_Func>::value) {
                static_assert(!sf_check_param_reference<_Func>::value, "Param can't be reference");
                static_assert(!sf_check_param_pointer<_Func>::value, "Param can't be pointer");
                static_assert(!sf_check_return_reference<_Func>::value, "Return can't be reference");
                static_assert(!sf_check_return_pointer<_Func>::value, "Return can't be pointer");


                using _Ret = typename sf_function_type_helper<_Func>::return_type;
                using _Param = typename sf_function_type_helper<_Func>::param_type;

                auto f = [=](SOCKET s, int id_code, const std::string &id_str, const byte_array &data) {
                    if (id == id_str) {
                        _Param param;
                        sf_deserialize(data, param, 0);
                        _Ret ret = sf_invoke(func, param);
                        __send_back(s, id_code, id_str, ret);
                    }
                };
                __func__vec__.push_back(f);

            } else {
                static_assert(!sf_check_param_reference<decltype(std::function(func))>::value,
                              "Param can't be reference");
                static_assert(!sf_check_param_pointer<decltype(std::function(func))>::value, "Param can't be pointer");
                static_assert(!sf_check_return_reference<decltype(std::function(func))>::value,
                              "Return can't be reference");
                static_assert(!sf_check_return_pointer<decltype(std::function(func))>::value,
                              "Return can't be pointer");


                using _Ret = typename sf_function_type_helper<decltype(std::function(func))>::return_type;
                using _Param = typename sf_function_type_helper<decltype(std::function(func))>::param_type;

                auto f = [=](SOCKET s, int id_code, const std::string &id_str, const byte_array &data) {
                    if (id == id_str) {
                        _Param param;
                        sf_deserialize(data, param, 0);
                        if constexpr (std::is_same<_Ret, void>::value) {
                            sf_invoke(func, param);
                            __send_back(s, id_code, id_str, '\0');
                        } else {
                            _Ret ret = sf_invoke(func, param);
                            __send_back(s, id_code, id_str, ret);
                        }
                    }
                };
                __func__vec__.push_back(f);
            }

        }

        /**
         * @brief make_server 创建RPC服务器
         * @return 服务器对象
         */
        static std::shared_ptr<sf_rpcserver> make_server() {
            return std::make_shared<sf_rpcserver>();
        }

        /**
         * @brief sf_rpcserver 构造函数
         */
        sf_rpcserver() {
            sf_bind_signal(sf_rpcserver::__tcp_server__,
                           data_coming,
                           [=](SOCKET sock, const pkg_header_t &header, const byte_array &data) {
                               __on_data_coming(sock, header, data);
                           },
                           false);
        }

        /**
         * @brief listen 监听
         * @param ip ip
         * @param port 端口
         * @return 是否成功
         */
        bool listen(const std::string &ip, unsigned short port) {
            return __tcp_server__->listen(ip, port);
        }

        /**
         * @brief close 关闭RPC服务器
         */
        void close() {
            __tcp_server__->close();
        }

    };
}
