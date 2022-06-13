#ifndef _KVP_SERVER_INCLUDED_
#define _KVP_SERVER_INCLUDED_

#include "json/json.hpp"
#include "kvp_store/kvp_store.h"
#include <functional>
#include "salto_ipc/salto_ipc_server.h"
#include "salto_ipc/salto_ipc_client.h"
#include "kvp_app/kvp_app.h"

class kvp_server : public kvp_app {
    public:
        /**
         * @brief implements constructor for kvp_server class.
         * initializes instance of kvp_app (base class) and
         * kvp_store.
         * 
         * Note: In case there is a problem encountered while constructing
         * kvp_app instance or kvp_store instance, this method may
         * throw an exception.
         *
         * @param [in] in_queue_name    optional std::string type object for
         *                              specifying queue name for incoming queue
         *                              This parameter is used to construct kvp_app base class
         *                              instance
         * @param [in] out_queue_name    optional std::string type object for
         *                              specifying queue name for outgoing queue
         *                              This parameter is used to construct kvp_app base class
         *                              instance
         * @param [in] kvp_filename     optional std::string type object to be
         *                              used as a filename of a backing store file
         *                              of kvp_store object.
         **/
        kvp_server(std::string in_queue_name = default_in_queue,
                std::string out_queue_name = default_out_queue,
                std::string kvp_filename = default_file_name) :
            kvp_app {in_queue_name, out_queue_name},
            m_kvp_filename {std::move(kvp_filename)},
            m_kvp_store {std::move(default_file_name), true}
            { }

    private:
        static constexpr const char* default_in_queue = "saltokvp_q_server";
        static constexpr const char* default_out_queue = "saltokvp_q_client";
        static constexpr const char* default_file_name = "saltokvp.txt";

        std::string m_kvp_filename;
        kvp_store m_kvp_store;

        virtual void dispatch(const nlohmann::json& cmd, bool ipc_event) override final;
};


#endif /*_KVP_SERVER_INCLUDED_*/