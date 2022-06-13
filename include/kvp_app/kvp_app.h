#ifndef _KVP_APP_INCLUDED_
#define _KVP_APP_INCLUDED_
#include <functional>

#include "json/json.hpp"

#include "salto_ipc/salto_ipc_server.h"
#include "salto_ipc/salto_ipc_client.h"

class kvp_app {
    public:
        /**
         * @brief implements constructor for kvp_app class.
         * initializes salto_ipc_server and salto_ipc_client objects internally.
         * 
         * Note: In case there is a problem encountered while constructing
         * ipc_server or ipc_client instances, this method may throw an exception.
         *
         * @param [in] in_queue_name    std::string type object to be
         *                              used as an incoming queue name of kvp_app
         *                              instance
         * @param [in] out_queue_name   std::string type object to be
         *                              used as an outgoing queue name of kvp_app
         *                              instance
         **/
        kvp_app(std::string in_queue_name, std::string out_queue_name) :
        m_ipc_srv {in_queue_name},
        m_ipc_clnt {out_queue_name} {}

        /**
         * @brief implements destructor for kvp_app class.
         **/
        virtual ~kvp_app() = default;

        using event_check_fn = std::function<bool()>;
        using event_process_fn = std::function<std::optional<nlohmann::json>(salto_ipc_server&)>;

        /**
         * @brief define a type for event descriptor.
         */
        struct event_desc {
            std::string type;/**< Event type specified as std::string */
            event_check_fn check;/**< std::function type callable to check whether there is an event to consume */
            event_process_fn process;/**< std::function type callable to consume the event */
        };

        
        static constexpr const int timeout_val = 100;/**< Event polling loop sleep duration */

        /**
         * @brief inserts an event_desc object into the event handler list.
         * These event_desc objects are then iterated over in the main event loop
         *
         * @param [in] event            event_desc type object that includes event_type string
         *                              and callable objects to check and process events
         */
        void register_event(const event_desc& event);

        /**
         * @brief implements the main event loop.
         * events are checked and if available then consumed by calling corresponding
         * callable objects in an infinite while loop
         */
        void run();

        /**
         * @brief function to return a handle to ipc_server instance
         *
         * @return salto_ipc_server     handle to the ipc server object instantiated 
         *                              in the kvp_app
         */
        salto_ipc_server& get_ipc_srv() {
            return m_ipc_srv;
        }

        /**
         * @brief function to return a handle to ipc_client instance
         *
         * @return salto_ipc_server     handle to the ipc client object instantiated 
         *                              in the kvp_app
         */
        salto_ipc_client& get_ipc_clnt() {
            return m_ipc_clnt;
        }

    protected:
        salto_ipc_server m_ipc_srv;
        salto_ipc_client m_ipc_clnt;
        std::vector<event_desc> m_events;

        virtual void dispatch(const nlohmann::json& cmd, bool ipc_event) = 0;

};
#endif /*_KVP_APP_INCLUDED_*/