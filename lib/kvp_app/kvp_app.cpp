#include "kvp_app/kvp_app.h"
#include <iostream>
#include <thread>

namespace {

void prompt_userentry_char() {
    std::cout << "> ";
    std::cout.flush();
}

}

void kvp_app::register_event(const event_desc& event) {
    m_events.emplace_back(event);
}

void kvp_app::run() {
    prompt_userentry_char();
    // Main event polling/processing loop
    while (true) {
        //< iterate through each event that was registered into the event_list
        //< 1) Check if there is an event to be processed
        //< 2) If there is a process, call the corresponding process() function
        //< 3) In case a valid object is returned from process() function, call dispatch() function with this object
        std::for_each(m_events.begin(), m_events.end(),
            [this](auto& event) {
                if (event.check()) {
                    auto cmd_dispatch { event.process(m_ipc_srv) };
                    if (cmd_dispatch.has_value()) {
                        dispatch(cmd_dispatch.value(), (event.type == "ipc"));
                    }
                    if (event.type == "stdin") {
                        prompt_userentry_char();
                    }
                }
                //< Sleep 100ms after processing each event
                std::this_thread::sleep_for(std::chrono::milliseconds(timeout_val));
            }
        );
    }
}