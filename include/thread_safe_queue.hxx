#ifndef THREAD_SAFE_QUEUE_HPP_
#define THREAD_SAFE_QUEUE_HPP_

#include <mutex>
#include <queue>
#include <memory>
#include <condition_variable>

template <typename T>
class threadsafe_queue {
    public:
        threadsafe_queue() = default;
        threadsafe_queue(const threadsafe_queue&);
        void push(T);
        void wait_and_pop(T&);
        std::shared_ptr<T> wait_and_pop();
        bool try_pop(T&);
        std::shared_ptr<T> try_pop();
        bool empty();

    private:
        std::queue<T> m_data_queue;
        std::mutex m_mutex;
        std::condition_variable m_data_condition;
};

#endif