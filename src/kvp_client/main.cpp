#include "kvp_client/kvp_client.h"

#include <iostream>

int main(int argc, char **argv) {
    try {
        kvp_client kvp_cl;

        // register events for event loop to check and process
        kvp_cl.register_event(kvp_client::event_desc{"ipc", ipc_event_check, ipc_event_process});
        kvp_cl.register_event(kvp_client::event_desc{"stdin", stdin_event_check, stdin_event_process});
        
        kvp_cl.run();

    } catch (std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << "\n";
    }

    return 0;
}
