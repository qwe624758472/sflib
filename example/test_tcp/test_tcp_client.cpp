#define SF_DEBUG
#include "network/sf_tcp_client.h"
#include "tools/sf_logger.hpp"

using namespace skyfire;
using namespace std;

int main()
{
    auto client = sf_tcp_client::make_instance(false);
    if (client->connect_to_server("127.0.0.1", 9988)) {
        sf_bind_signal(
            client, data_coming, [](const sf_pkg_header_t header, const byte_array& data) {
                sf_debug(header.type, header.length, to_string(data));
            },
            true);
        for (;;) {
            string tmp;
            getline(cin, tmp);
            if (tmp == "exit") {
                break;
            }
            client->send(5566, to_byte_array(tmp));
        }
        client->close();
    }
}