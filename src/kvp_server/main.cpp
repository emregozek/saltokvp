#include <iostream>
#include "kvp_server/kvp_server.h"
#include "salto_ipc/salto_ipc_server.h"
#include <csignal>


int main(int argc, char **argv) {
    try {
        kvp_server kvp_srv;

        // register events for event loop to check and process
        kvp_srv.register_event(kvp_server::event_desc{"ipc", ipc_event_check, ipc_event_process});
        kvp_srv.register_event(kvp_server::event_desc{"stdin", stdin_event_check, stdin_event_process});
        
        kvp_srv.run();

    } catch (std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << "\n";
    }

    return 0;
}

