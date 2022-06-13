#include "kvp_server/kvp_server.h"
#include "kvp_store/kvp_store.h"
#include <functional>
#include <iostream>
#include <optional>
#include <thread>
#include <chrono>
#include <boost/interprocess/ipc/message_queue.hpp>

void kvp_server::dispatch(const nlohmann::json& cmd_val, bool ipc_event) {
    // input validation
    if (cmd_val.find("cmd") == cmd_val.end()) {
        return;
    }
    if (cmd_val.find("key") == cmd_val.end()) {
        return;
    }
    if (cmd_val["cmd"] == commands::set && cmd_val.find("value") == cmd_val.end()) {
        return;
    }
    bool send_resp {};
    std::string resp_val;
    try {
        // Find the command type and call the corresponding api from kvp_store class
        if (cmd_val["cmd"] == commands::set) {
            m_kvp_store.set(cmd_val["key"], cmd_val["value"]);
        } else if (cmd_val["cmd"] == commands::get) {
            // If GET operation was requested locally (stdin) prompt the result of GET command to stdout
            // Otherwise, send the result back to the client application with an IPC message
            resp_val = m_kvp_store.get(cmd_val["key"]);
            send_resp = ipc_event;
            if (!ipc_event) {
                std::cout << resp_val << "\n";
            }
        } else if (cmd_val["cmd"] == commands::del) {
            m_kvp_store.del(cmd_val["key"]);
        }
        // In case GET request was originated from client app, result of GET operation is sent back to the client
        if (send_resp) {
            auto success = get_ipc_clnt().send_message(nlohmann::json{{"result", resp_val}}.dump());
            if (!success) {
                std::cerr << "message couldnt be sent!\n";
            }          
        }

    } catch (std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << "\n";
    }
}
