#ifndef _SALTO_IPC_CLIENT_INCLUDED_
#define _SALTO_IPC_CLIENT_INCLUDED_

#include "salto_ipc/salto_ipc.h"
#include "json/json.hpp"

class salto_ipc_client : public salto_ipc {
    private:
        static constexpr const char* default_queue_name = "saltokvp_q_client";

    public:

        salto_ipc_client(const std::string queue_name = default_queue_name) :
            salto_ipc(std::move(queue_name)) {}

        ~salto_ipc_client();

        bool send_message(const std::string& msg);

};

#endif /*_SALTO_IPC_CLIENT_INCLUDED_*/