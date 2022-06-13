#include "kvp_store/kvp_store.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <iterator>
#include <string>
#include <utility>

namespace {

void ltrim(std::string& str) {
    str.erase(str.begin(),
              std::find_if(
                  str.begin(),
                  str.end(),
                  [] (const auto& val) {
                      return !std::isspace(val);
                    }
                )
            );
}

void rtrim(std::string& str) {
    str.erase(std::find_if(
                  str.rbegin(),
                  str.rend(),
                  [] (const auto& val) {
                      return !std::isspace(val);
                    }
                ).base(),
                str.end()
            );
}

void trim(std::string& str) {
    ltrim(str);
    rtrim(str);
}

}

kvp_store::kvp_store(std::filesystem::path p, bool create) : m_file_store_path{std::move(p)} {
    std::ios_base::openmode om {std::ios_base::in | std::ios_base::out};
    std::ostringstream err_msg;
    if (!std::filesystem::exists(m_file_store_path)) {
        if (!create) {
            err_msg << "File with path: " << m_file_store_path.c_str() << " is not in the filesystem\n";
            throw std::runtime_error(err_msg.str());
        }
        om |= std::ios_base::trunc;
    }
    m_file_store.open(m_file_store_path, om);
    if (!m_file_store) {
        err_msg << "File with path: " << m_file_store_path.c_str() << " could not be opened\n";
        throw std::runtime_error(err_msg.str());
    }

    m_file_store.seekg(0);
    std::string line;
    std::size_t line_cnt{};
    while (std::getline(m_file_store, line)) {
        trim(line);

        if (line.empty()) {
            err_msg << "Empty line encountered at line: " << line << "of file: " << m_file_store_path.c_str() << "\n";
            throw std::runtime_error(err_msg.str());
        }
        auto first_char = line[0];
        auto used_line = first_char == '@';
        auto unused_line = first_char == '!';

        if (!used_line && !unused_line) {
            err_msg << "Malformed line encountered: " << line << ". Line should start with either @ character or ! character\n";
            throw std::runtime_error(err_msg.str());
        }

        if (used_line) {
            auto key_end_idx = line.find_first_of(' ');
            if (key_end_idx == std::string::npos) {
                err_msg << "Malformed line encountered: " << line << ". Line has only key, value is not found\n";
                throw std::runtime_error(err_msg.str());
            }
            std::string key {line.substr(1, key_end_idx - 1)};

            auto value_end_idx = line.find_last_not_of(' ');
            std::string value {line.substr(key_end_idx + 1, value_end_idx - key_end_idx)};
            //line_prop line_el { .pos = line_cnt, .cap = line.size() };
            // m_mem_store[key] = { .value = value, .line = line_el };
            m_mem_store[key] = { .value = value, .line = { .pos = line_cnt, .cap = line.size() } };
        } else if (unused_line) {
            m_unused_lines[line_cnt] = line.size();
        }
        ++line_cnt;
    }
    m_file_store.clear();
    m_line_cnt = line_cnt;

}

std::string kvp_store::get(const std::string& ckey) {
    if (ckey.empty()) {
        return std::string{};
    }

    auto key {ckey};

    trim(key);
    if (key.empty()) {
        return std::string{};
    }

    if (auto iter = m_mem_store.find(key); iter != m_mem_store.end()) {
        return m_mem_store[key].value;
    }

    return std::string{};
}

std::size_t kvp_store::line_length(const std::string& key, const std::string& value) {
    return std::string{'@'}.size() + key.size() + std::string{' '}.size() + value.size();
}

void kvp_store::skip_lines(std::size_t line_num) {
    std::ostringstream err_msg;
    if (!m_file_store) {
        err_msg << "File store is not accessible\n";
        throw std::runtime_error(err_msg.str());
    }
    const auto c_line_num {line_num};
    // set the position to the beginning
    m_file_store.seekg(0);
    while (!m_file_store.eof() && line_num > 0) {
        m_file_store.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        --line_num;
    }
    m_file_store.tellg();
    m_file_store.clear();
    if (line_num > 0) {
        err_msg << "File has only "<< c_line_num - line_num + 1 << " lines, cannot skip to the line " << c_line_num << "\n";
        throw std::runtime_error(err_msg.str());
    }
}

void kvp_store::set(const std::string& ckey, const std::string& cvalue) {
    if (ckey.empty() || cvalue.empty()) {
        return;
    }
    auto key {ckey};
    auto value {cvalue};

    // sanitize input
    trim(key);
    trim(value);
    if (key.empty() || value.empty()) {
        return;
    }

    std::size_t line_number {};
    bool reuse_line {};

    std::size_t reused_line_length {};
    std::size_t new_line_length {line_length(key, value)};

    // looking for the key in the in-memory cache
    if (auto iter_existing_entry = m_mem_store.find(key); iter_existing_entry != m_mem_store.end()) {

        // key found, retrieve value and line number of existing entry
        auto existing_entry_value {iter_existing_entry->second.value};
        auto existing_entry_line_num {iter_existing_entry->second.line.pos};
        auto existing_entry_line_cap {iter_existing_entry->second.line.cap};

        // in case value is same as previous value, bail out early as same key/value pair is attempted be re-inserted
        if (existing_entry_value == value) {
            return;
        }

        // new value is smaller or equal than previous value, update the value of the existing entry in the file
        reused_line_length = existing_entry_line_cap;
        reuse_line = new_line_length <= existing_entry_line_cap;
        if (reuse_line) {
            line_number = existing_entry_line_num;
        } else {
            del(key);
        }
    }

    // Reusing a line with same key was not possible, lets look for any unused line that our new entry can fit in
    if (!reuse_line) {

        // Search for an entry in m_unused_lines map and return the first line large enough to accomadate our new entry
        if (auto iter_unused_entry = std::find_if(
            m_unused_lines.begin(),
            m_unused_lines.end(),
            [new_line_length] (const auto& entry_len) {
                return entry_len.second >= new_line_length;
            }
        ); iter_unused_entry != m_unused_lines.end()) {
            reuse_line = true;
            line_number = iter_unused_entry->first;
            reused_line_length = iter_unused_entry->second;
            m_unused_lines.erase(iter_unused_entry);
        }
    }

    // move file pointer to the beginning of the line to be reused
    if (reuse_line) {
        skip_lines(line_number);
    // move file pointer to the end of file where new entry will be inserted
    } else {
        m_file_store.seekp(0, std::ios_base::end);
        line_number = m_line_cnt;
        // new line inserted, increment m_line_count data member
        ++m_line_cnt;
    }

    // insert valid key value pair with ' ' character as delimiter in between key and value
    m_file_store << "@" << key << " " << value;

    // in case existing line is reused, fill the rest of the line with whitespaces
    if (reuse_line) {
        std::size_t n_ws_chars {reused_line_length - line_length(key, value)};
        while (n_ws_chars > 0) {
            m_file_store.put(' ');
            --n_ws_chars;
        }
    // entry is added in the end of the file, insert a new line character
    } else {
        m_file_store << "\n";
    }
    // flush the changes in to the file
    m_file_store.flush();
    m_file_store.clear();

    // insert our new key/value pair entry in the in-memory cache
    auto line_cap = reuse_line ? reused_line_length : new_line_length;
    m_mem_store[key] = { .value = value, .line = { .pos = line_number, .cap = line_cap } };
}

void kvp_store::del(const std::string& ckey) {
    auto key {ckey};
    // sanitize
    trim(key);

    // empty key, early bail out
    if (key.empty()) {
        return;
    }

    auto iter {m_mem_store.find(key)};
    if (iter == m_mem_store.end()) {
        return;
    }

    // retrieve value and line number of of kvp to be deleted
    auto line_number {iter->second.line.pos};
    auto line_cap {iter->second.line.cap};

    // go to the beginning of the line that is to be marked as an unused line
    skip_lines(line_number);
    m_file_store.put('!');
    m_file_store.flush();

    // insert new entry in the unused lines map
    m_unused_lines[line_number] = line_cap;

    // erasing this kvp entry from in-memory cache
    m_mem_store.erase(iter);
}

void kvp_store::squish() {
    // create path for squished version of the file
    std::filesystem::path squished_path;
    if (m_file_store_path.has_filename() && m_file_store_path.filename().has_stem()) {
        squished_path = m_file_store_path.filename().stem().c_str() + std::string{"_squished.txt"};
    } else {
        squished_path = "squished.txt";
    }

    std::ostringstream err_msg;
    if (std::filesystem::exists(squished_path)) {
        err_msg << "File with path: " << squished_path.c_str() << " is already in the filesystem\n";
        throw std::runtime_error(err_msg.str());
    }

    // open a new file to store only the used lines
    std::fstream squished_file {squished_path, (std::ios_base::in | std::ios_base::out | std::ios_base::trunc)};
    if (!squished_file) {
        err_msg << "File with path: " << m_file_store_path.c_str() << " could not be opened\n";
        throw std::runtime_error(err_msg.str());
    }
    // set the position to the beginning
    m_file_store.seekg(0);
    std::string line;
    std::size_t line_cnt {};
    // iterate over all the lines from current file and store only the used lines in the squished file
    while (std::getline(m_file_store, line)) {
        trim(line);

        if (line.empty()) {
            err_msg << "Empty line encountered at line: " << line << "of file: " << m_file_store_path.c_str() << "\n";
            squished_file.close();
            throw std::runtime_error(err_msg.str());
        }
        if (line[0] != '@' && line[0] != '!') {
            err_msg << "Malformed line encountered: " << line << "of file: " << m_file_store_path.c_str() << "\n";
            squished_file.close();
            throw std::runtime_error(err_msg.str());
        }
        if (line[0] == '@') {
            squished_file << line << "\n";
            ++line_cnt;
        }
    }
    squished_file.flush();
    squished_file.clear();

    m_file_store.clear();

    // close both squished file and active file
    if (!m_file_store) {
        std::cerr << "Something went wrong during std::move of fstream obj\n";
        throw std::runtime_error{"Something went wrong during std::move of fstream obj\n"};
    }

    m_file_store.close();

    if (!squished_file) {
        err_msg << "File with path: " << squished_path.c_str() << " could not be closed\n";
        throw std::runtime_error(err_msg.str());
    }

    squished_file.close();

    // rename squished file to be the new active storage file
    std::error_code ec;
    std::filesystem::rename(squished_path, m_file_store_path, ec);
    if (ec) {
        err_msg << "Rename operation from: " << squished_path.c_str() << " to: " << m_file_store_path.c_str() << " has failed\n";
        throw std::runtime_error(err_msg.str());   
    }

    // update m_line_cnt with the new line count
    m_line_cnt = line_cnt;
    // empty the map that store unused lines (as there is no unused lines anymore)
    m_unused_lines.clear();
    // in-memory cache of key/value pairs need to be repopulated as position of the used lines might have been changed
    m_mem_store.clear();

    m_file_store.open(m_file_store_path, (std::ios_base::in | std::ios_base::out));
    if (!m_file_store) {
        err_msg << "File with path: " << m_file_store_path.c_str() << " could not be opened\n";
        throw std::runtime_error(err_msg.str());
    }
    
    m_file_store.seekg(0);
    line_cnt = 0;
    // iterate over file to populate in-memory cache
    while (std::getline(m_file_store, line)) {
        trim(line);
        if (line.empty()) {
            err_msg << "Empty line encountered at line: " << line << "of file: " << m_file_store_path.c_str() << "\n";
            throw std::runtime_error(err_msg.str());
        }
        if (line[0] != '@') {
            err_msg << "Malformed line encountered: " << line << "of file: " << m_file_store_path.c_str() << "\n";
            throw std::runtime_error(err_msg.str());
        }
        auto key_end_idx = line.find_first_of(' ');
        if (key_end_idx == std::string::npos) {
            err_msg << "Malformed line encountered: " << line << ". Line has only key, value is not found\n";
            throw std::runtime_error(err_msg.str());
        }
        std::string key {line.substr(1, key_end_idx - 1)};
        std::string value {line.substr(key_end_idx + 1)};
        // update in-cache memory
        m_mem_store[key] = { .value = value, .line = { .pos = line_cnt, .cap = line.size() } };
        ++line_cnt;
    }
    
    m_file_store.clear();
}
kvp_store::~kvp_store() {
    m_file_store.close();
}
double kvp_store::backing_store_util_rate() const {
    return (m_line_cnt - m_unused_lines.size()) * 1.0 / m_line_cnt;
}
