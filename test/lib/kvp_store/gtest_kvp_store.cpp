#include "gtest/gtest.h"
#include <iostream>
#include "kvp_store/kvp_store.h"
#include <filesystem>

TEST(gtest_kvp_store, simple_test) {
    const char* filename = "simple_test.txt";
    try {
        {
            kvp_store mykvp {std::filesystem::path {filename}, true};
            mykvp.set("a", "1");
            EXPECT_STREQ(mykvp.get("a").c_str(), "1");
            mykvp.del("a");
            mykvp.set("b", "2");
        }
        {
            kvp_store mykvp {std::filesystem::path {filename}, false};
            EXPECT_STREQ(mykvp.get("b").c_str(), "2");
        }
        std::remove(filename);
    } catch (std::runtime_error& ex) {
        std::cout << "exception caught = "<< ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, set_value_with_internal_whitespaces) {
    const char* filename = "set_value_with_internal_whitespaces.txt";
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};
        mykvp.set("ABC", "A A");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "A A");
        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, set_key_with_surrounding_whitespaces) {
    const char* filename = "set_key_with_surrounding_whitespaces.txt";
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};
        mykvp.set("       ABC   ", "1");
        EXPECT_STREQ(mykvp.get("  ABC ").c_str(), "1");
        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, set_value_with_surrounding_whitespaces) {
    const char* filename = "set_value_with_surrounding_whitespaces.txt";
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};
        mykvp.set("ABC", "         123               ");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "123");
        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, delete_existing_key) {
    const char* filename = "delete_existing_key.txt";
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};
        mykvp.set("ABC", "123");
        mykvp.del("ABC");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "");
        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, delete_nonexisting_key) {
    const char* filename = "delete_nonexisting_key.txt";
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};
        mykvp.set("ABC", "123");
        mykvp.del("ABC1");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "123");
        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, reinserting_existing_key_value) {
    const char* filename = "reinserting_existing_key_value.txt";
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};
        mykvp.set("ABC", "123");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "123");
        mykvp.set("ABC", "123");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "123");

        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, reinserting_existing_key_with_new_value) {
    const char* filename = "reinserting_existing_key_with_new_value.txt";
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};

        mykvp.set("ABC", "123");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "123");
        mykvp.set("ABC", "456");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "456");
        mykvp.set("ABC", "456789");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "456789");
        mykvp.set("ABC", "456");
        EXPECT_STREQ(mykvp.get("ABC").c_str(), "456");

        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}

TEST(gtest_kvp_store, util_rate_and_squish) {
    const char* filename = "util_rate_and_squish.txt";
    const int n_of_elements_inserted = 100;
    const int n_of_elements_deleted = 36;
    const int value_calc_coeff = 10;
    try {
        kvp_store mykvp {std::filesystem::path {filename}, true};
        for (std::size_t i {}; i < n_of_elements_inserted; ++i) {
            mykvp.set(std::to_string(i), std::to_string(i * value_calc_coeff));
        }
        for (std::size_t i {}; i < n_of_elements_inserted; ++i) {
            EXPECT_STREQ(mykvp.get(std::to_string(i)).c_str(), std::to_string(i * value_calc_coeff).c_str());
        }
        for (std::size_t i {}; i < n_of_elements_deleted; ++i) {
            mykvp.del(std::to_string(i));
        }
        auto rate {mykvp.backing_store_util_rate()};
        EXPECT_DOUBLE_EQ(rate, (n_of_elements_inserted - n_of_elements_deleted) * 1.0 / n_of_elements_inserted);

        mykvp.squish();

        EXPECT_DOUBLE_EQ(mykvp.backing_store_util_rate(), 1.0);

        std::remove(filename);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }
}
