#include "kvp_client/kvp_client.h"
#include <sstream>
#include <iostream>
#include "json/json.hpp"
#include <thread>

void kvp_client::dispatch_in_msg(const nlohmann::json& cmd_val) {
    // prompt the result of "GET" operation back to the stdout
    std::cout << cmd_val["result"].get<std::string>() << std::endl;
    std::cout.flush();
}

void kvp_client::dispatch_out_msg(const nlohmann::json& cmd_val) {
    // input validation
    if (cmd_val["cmd"] == commands::set && cmd_val.find("value") == cmd_val.end()) {
        return;
    }
    if ((cmd_val["cmd"] != commands::set) && (cmd_val["cmd"] != commands::get) && (cmd_val["cmd"] != commands::del)) {
        return;
    }
    auto msg_str {cmd_val.dump()};
    // send message to server application
    auto success = get_ipc_clnt().send_message(msg_str);
    if (!success) {
        std::cerr << "Message could not be sent\n";
    }
}

void kvp_client::dispatch(const nlohmann::json& cmd_val, bool ipc_event) {
    bool out_msg {};
    bool in_msg {};
    // look for specific keys for
    // 1) decide whether it is stdin event or ipc event
    // 2) validate input (in case expected keys for each event type is missing in json object, bail out early)
    if (cmd_val.find("cmd") != cmd_val.end() && cmd_val.find("key") != cmd_val.end()) {
        out_msg = true;
    } else if (cmd_val.find("result") != cmd_val.end()) {
        in_msg = true;
    }
    if (!out_msg && !in_msg) {
        return;
    }

    try {
        // call the corresponding dispatch function
        if (in_msg) {
            dispatch_in_msg(cmd_val);
        } else {
            dispatch_out_msg(cmd_val);
        }

    } catch (std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << "\n";
    }
}