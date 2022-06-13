#include "gtest/gtest.h"
#include <iostream>
#include "kvp_client/kvp_client.h"
#include <exception>

TEST(gtest_kvp_client, simple_test_ctor_no_arg) {
    try {
        kvp_client clnt;
    } catch (std::exception& ex) {
        std::cerr << "Ëxception caught "<< ex.what() << "\n";
    }
}

TEST(gtest_kvp_client, simple_test_ctor_with_args) {
    try {
        kvp_client clnt {"test_in_queue", "test_out_queue"};
    } catch (std::exception& ex) {
        std::cerr << "Ëxception caught "<< ex.what() << "\n";
    }
}