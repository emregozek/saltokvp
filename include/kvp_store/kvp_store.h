#ifndef _KVP_STORE_INCLUDED_
#define _KVP_STORE_INCLUDED_

#include <filesystem>
#include <fstream>
#include <string>
#include <map>

class kvp_store {
    public:

        /**
         * @brief Construct a new kvp_store object by loading the key-value file
         * @p path into memory. if the file at @p path does not exist,
         * create an empty key-value file at @p path only if @p create is true,
         * and otherwise throw @p std::runtime_error.
         * 
         * @param [in] path         Path to the key-value file to be loaded
         *                          or created, if one does not exist.
         * @param [in] create       if the key-value file at @p filepath does not
         *                          exist, create it.
         */
        kvp_store(std::filesystem::path p, bool create = false);

        /* destroy the kvp_store instance */
        virtual ~kvp_store();

        /**
         * @brief Get a value from the key value store based on the @p key. If no
         * such value is found, throws a @p std::runtime_error instance
         * 
         * @param key [in]          The key, value of which shall be retrieved
         * @return std::string      the value corresponding to @p key
         */
        std::string get(const std::string& ckey);

        /**
         * @brief Set the value corresponding to a key. If no key exists, insert
         * one.
         *
         * @note This function also updates the non volatile backing storage. If
         * it returns, it is guaranteed that the backing storage is updated. It
         * might throw an exception if a file system operation fails, e.g. due to
         * disk failure
         *
         * @param [in] key          The key, value of which should be set.
         * @param [in] value        The value to be set, corresponding to @p key
         */        
        void set(const std::string& ckey, const std::string& cvalue);

        /**
         * @brief Delete the entry corresponding to a key.
         * 
         * @param [in] key          The key, entry of which (value and all) should
         *                          be removed from the key-value store.
         * 
         * @note This function also updates the non volatile backing storage. If
         * it returns, it is guaranteed that the backing storage is updated. It
         * might throw an exception if a file system operation fails, e.g. due to
         * disk failure
         */
        void del(const std::string& ckey);

        /**
         * @brief when lines are deleted from a kvp_store object, their lines are
         * marked as "unused". This means after a lot of @p del operations, the
         * file on disk might contain many unused lines. this function
         * goes through the file to remove those unused lines to reduce on-disk
         * size of the backing store
         * 
         */
        void squish();

        /**
         * @brief calculates the utilization rate of the backing storage 
         * by dividing used lines to total number of lines. It can be used to
         * by client when implementing a policy on when to call squish() function.
         * 
         * @return double      utilization rate value between 0.0 and 1.0
         */
        double backing_store_util_rate() const;

    private:
        static std::size_t line_length(const std::string& key, const std::string& value);

        void skip_lines(std::size_t line_num);

        /**
         * @brief line_prop structure keeps position (row index) and capacity
         * of a line in backing storage
         */
        struct line_prop {
            std::size_t pos;
            std::size_t cap;
        };
        struct elem {
            std::string value;
            line_prop line;
        };
        std::map<std::string, elem> m_mem_store;
        std::map<std::size_t, std::size_t> m_unused_lines;
        std::fstream m_file_store;
        std::filesystem::path m_file_store_path;
        std::size_t m_line_cnt = 0;
};

#endif /*_KVP_STORE_INCLUDED_*/