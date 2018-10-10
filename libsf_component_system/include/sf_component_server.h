#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_rpcserver.h"
#include "sf_component_context.h"

#include <list>
#include <memory>
#include <string>
#include <functional>
#include <shared_mutex>

namespace skyfire
{
    class sf_component_server : public sf_nocopy<sf_object>
    {
        // 组件注册信号
        SF_REG_SIGNAL(component_reg, SOCKET, const std::string&);
        // 组件反注册信号
        SF_REG_SIGNAL(component_unreg, SOCKET, const std::string&);

    private:
        std::map<std::string, byte_array> public_area__;
        std::shared_ptr<sf_rpcserver> rpc_server__ = sf_rpcserver::make_server();
        std::map<SOCKET, sf_component_context_t> component_context__;
        std::shared_mutex mu_public__;
        std::shared_mutex mu_private__;
        std::string data_dir__;

        byte_array get_private_data(SOCKET sock, std::string key) ;
        void delete_private_data(SOCKET sock, std::string key) ;
        void clear_private_data(SOCKET sock) ;
        void set_private_data(SOCKET sock,std::string key, byte_array value);
        bool has_private_data(SOCKET sock,std::string key) ;

        bool reg_component(SOCKET sock, std::string name);

        std::list<std::string> get_component_list(SOCKET sock) ;

        bool save_private_data(SOCKET sock);
        bool load_private_data(SOCKET sock);

        sf_component_server(const std::string& data_dir);

    public:



        bool listen(const std::string& ip, unsigned short port);
        std::list<sf_component_context_t> get_component_context();
        void unreg_component(std::string name);

        void clear_public_area();
        byte_array get_public_data(std::string key);
        void set_public_data(std::string key, byte_array value);
        void delete_public_data(std::string key);
        bool has_public_data(std::string key) ;

        bool save_data();
        bool load_data();

        bool save_public_data();
        bool load_public_data();
    };
}