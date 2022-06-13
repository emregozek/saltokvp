#include "gtest/gtest.h"
#include <iostream>
#include "salto_ipc/salto_ipc.h"
#include "salto_ipc/salto_ipc_server.h"
#include "salto_ipc/salto_ipc_client.h"
#include <exception>

TEST(gtest_salto_ipc, simple_test_ipc) {
    try {
        salto_ipc ipc;

    } catch (std::exception& ex) {
        std::cerr << "Exception caught " << ex.what() << "\n";
    }
}

TEST(gtest_salto_ipc, simple_test_ipc_server) {
    try {
        salto_ipc_server ipc;

    } catch (std::exception& ex) {
        std::cerr << "Exception caught " << ex.what() << "\n";
    }
}

TEST(gtest_salto_ipc, simple_test_ipc_client) {
    try {
        salto_ipc_client ipc;

    } catch (std::exception& ex) {
        std::cerr << "Exception caught " << ex.what() << "\n";
    }
}

