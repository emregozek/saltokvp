#include "salto_ipc/salto_ipc.h"
#include "salto_ipc/salto_ipc_server.h"
#include "salto_ipc/salto_ipc_client.h"
#include "json/json.hpp"
#include <iostream>
#include <optional>

namespace {

enum class parse_state {
    cmd,
    key,
    val
};

void report_error_and_cleanup(std::istringstream& iss, parse_state& state) {
    std::cerr << "Incorrect input format, supported input types:\n";
    std::cerr << "- SET <key> <value>\n";
    std::cerr << "- GET <key>\n";
    std::cerr << "- DELETE <key>\n";
    iss.ignore();
    state = parse_state::cmd;
}

}

std::optional<nlohmann::json> salto_ipc_server::receive_message() {
    try {
        char buffer[m_msg_size];
        std::size_t len {};
        unsigned int prio {};
        
        if (m_msg_queue.try_receive(buffer, m_msg_size, len, prio)) {
            return nlohmann::json::parse(buffer, buffer + len);
        }

    } catch (boost::interprocess::interprocess_exception& ex) {
        std::cerr << "exception caught: " << ex.what() << "\n";
        return std::nullopt;
    }
    return std::nullopt;
}

salto_ipc::~salto_ipc() {
    boost::interprocess::message_queue::remove(m_queue_name.c_str());
}

std::optional<nlohmann::json> salto_ipc_server::check_events() {
    try {
        auto msg_opt {receive_message()};
        if (!msg_opt.has_value()) {
            return std::nullopt;
        }
        return msg_opt.value();

    } catch (const boost::interprocess::interprocess_exception& ex) {
        std::cerr << "exception caught = " << ex.what() << "\n";
    }
    return std::nullopt;
}

std::optional<nlohmann::json> ipc_event_process(salto_ipc_server& srv) {
    return srv.check_events();
}

bool salto_ipc_client::send_message(const std::string& msg) {
    try {
        m_msg_queue.send(msg.c_str(), msg.length() + 1, 0);

    } catch (boost::interprocess::interprocess_exception& ex) {
        std::cerr << "exception caught: " << ex.what() << "\n";
        return false;
    }
    return true;
}

salto_ipc_client::~salto_ipc_client() {
    boost::interprocess::message_queue::remove(m_queue_name.c_str());
}

bool ipc_event_check() {
    return true;
}

bool stdin_event_check() {
    fd_set fdst;

    FD_ZERO(&fdst);

    FD_SET(STDIN_FILENO, &fdst);

    struct timeval time_val {.tv_sec = 0, .tv_usec = 0};

    select(1, &fdst, nullptr, nullptr, &time_val);

    return (FD_ISSET(0, &fdst) != 0);
}
//< function to parse the input from stdin
std::optional<nlohmann::json> stdin_event_process(salto_ipc_server& srv) {

    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) {
        return std::nullopt;
    }
    std::istringstream iss{line};

    parse_state state = parse_state::cmd;
    std::string cmd;
    std::string key;
    std::string value;
    bool completed{};
    while (iss) {
        std::string word;
        iss >> word;
        if (word.empty() || completed) {
            report_error_and_cleanup(iss, state);
            return std::nullopt;
        }
        switch (state) {
            case parse_state::cmd:
                if (word != commands::get && word != commands::set && word != commands::del) {
                    report_error_and_cleanup(iss, state);
                    return std::nullopt;
                }
                cmd = word;
                state = parse_state::key;
                break;
            case parse_state::key:
                key = word;
                if ((cmd == commands::get) || cmd == commands::del) {
                    completed = true;
                    iss.ignore();
                } else {
                    state = parse_state::val;
                }
                break;
            case parse_state::val:
                value = word;
                completed = true;
                iss.ignore();
                break;
            default:
                iss.ignore();
                throw std::runtime_error{"Internal parsing error, exiting\n"};
        }
    }
    return nlohmann::json{{"cmd", cmd},{"key", key},{"value", value}};
}
