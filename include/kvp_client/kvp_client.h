#ifndef _KVP_CLIENT_INCLUDED_
#define _KVP_CLIENT_INCLUDED_
#include <string>
#include "salto_ipc/salto_ipc_client.h"
#include "salto_ipc/salto_ipc_server.h"
#include "boost/interprocess/ipc/message_queue.hpp"
#include "kvp_app/kvp_app.h"

class kvp_client : public kvp_app {
    public:
        /**
         * @brief implements constructor for kvp_server class.
         * initializes instance of kvp_app (base class)
         * 
         * Note: In case there is a problem encountered while constructing
         * kvp_app instance, this method may throw an exception.
         *
         * @param [in] in_queue_name    optional std::string type object for
         *                              specifying queue name for incoming queue
         *                              This parameter is used to construct kvp_app base class
         *                              instance
         * @param [in] out_queue_name    optional std::string type object for
         *                              specifying queue name for outgoing queue
         *                              This parameter is used to construct kvp_app base class
         *                              instance
         **/
        kvp_client(std::string in_queue_name = default_in_queue,
                std::string out_queue_name = default_out_queue) :
            kvp_app {in_queue_name, out_queue_name}
            { }

    private:
        static constexpr const char* default_in_queue = "saltokvp_q_client";
        static constexpr const char* default_out_queue = "saltokvp_q_server";

        static void dispatch_in_msg(const nlohmann::json& cmd_val);

        void dispatch_out_msg(const nlohmann::json& cmd_val);
        
        virtual void dispatch(const nlohmann::json& cmd, bool ipc_event) override final;

};

#endif /*_KVP_CLIENT_INCLUDED_*/