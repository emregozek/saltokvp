#ifndef _SALTO_IPC_INCLUDED_
#define _SALTO_IPC_INCLUDED_
#include <boost/interprocess/ipc/message_queue.hpp>

class salto_ipc {
    private:
        static constexpr const std::size_t default_queue_size = 100;

    protected:
        static constexpr const char* default_queue_name = "saltokvp_q_server";

        std::string m_queue_name = default_queue_name;
        std::size_t m_queue_size = default_queue_size;
        std::size_t m_msg_size = default_msg_size;
    public:
        static constexpr const std::size_t default_msg_size = 1024;

        boost::interprocess::message_queue m_msg_queue;

        salto_ipc(const std::string queue_name = default_queue_name, std::size_t queue_size = default_queue_size, std::size_t msg_size = default_msg_size) :
            m_queue_name {std::move(queue_name)},
            m_queue_size {queue_size},
            m_msg_size {msg_size},
            m_msg_queue {boost::interprocess::open_or_create, m_queue_name.c_str(), m_queue_size, m_msg_size} {}

        virtual ~salto_ipc();

};

#endif /*_SALTO_IPC_INCLUDED_*/