#ifndef _SALTO_IPC_SERVER_INCLUDED_
#define _SALTO_IPC_SERVER_INCLUDED_

#include "salto_ipc/salto_ipc.h"
#include "json/json.hpp"
#include <optional>

class salto_ipc_server : public salto_ipc {
    private:
        static constexpr const char* default_queue_name = "saltokvp_q_server";
        
        std::optional<nlohmann::json> receive_message();

    public:

        salto_ipc_server(const std::string queue_name = default_queue_name) :
            salto_ipc(std::move(queue_name)) {}

        std::optional<nlohmann::json> check_events();
};

bool ipc_event_check();

std::optional<nlohmann::json> ipc_event_process(salto_ipc_server& srv);

struct commands {
    static constexpr const char* get = "GET";
    static constexpr const char* set = "SET";
    static constexpr const char* del = "DELETE";
};

bool stdin_event_check();
std::optional<nlohmann::json> stdin_event_process(salto_ipc_server& srv);


#endif /*_SALTO_IPC_SERVER_INCLUDED_*/