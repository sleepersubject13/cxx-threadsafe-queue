#include "thread_safe_queue.hxx"

template <typename T>
threadsafe_queue<T>::threadsafe_queue(const threadsafe_queue& rhs) {
    std::lock_guard<std::mutex> lock {m_mutex};
    m_data_queue = rhs.m_data_queue;
}

template <typename T>
bool threadsafe_queue<T>::empty() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_data_queue.empty();
}

template <typename T>
void threadsafe_queue<T>::push(T value_to_push) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_data_queue.push(value_to_push);
    m_data_condition.notify_one();
}

template <typename T>
void threadsafe_queue<T>::wait_and_pop(T& out) {
    // First, we acquire a unique lock. A std::unique_lock
    // doesn't really own its mutex, which means we can
    // explicitly call the 'lock' and 'unlock' methods
    // on our std::mutex
    std::unique_lock<std::mutex> lock{m_mutex};
    // Next, from within the lambda expression, we check to see if
    // there's some data available in the queue. If not, 'wait' puts
    // the current thread to sleep, releases the lock, thereby alowing
    // the producer thread to potentially push to the queue.
    //
    // If it does, however find the queue to have data, it passes the value
    // at the front of the queue to the output parameter 'out', and then
    // pops that item off the top of the queue. Execution reaches the closing
    // brace, and our mutex is autmatically unlocked.
    //
    //
    // NOTE: If you're using a C++17 compiler, you can use the '=' default capture
    // instead of 'this' to capure 'm_data_queue' (or any of this template class' member
    // variables for that matter). This is because using the former will implicity imply
    // capturing 'this'. But since C++20, capturing a class' member variables by means 
    // of '=' has been deprecated.
    m_data_condition.wait(lock, [this] () -> bool {
        return !m_data_queue.empty();
    });
    out = m_data_queue.front();
    m_data_queue.pop();
    // Automatically release lock on our std::mutex
}

template <typename T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop() {
    std::unique_lock<std::mutex> lock{m_mutex};
    // Let me wait...
    m_data_condition.wait(lock, [this] () -> bool {
        // I've been woken up, let me check if the queue has any data
        // available. If there's not any data, I'll just go back to sleep
        return !m_data_queue.empty();
    });
    // If an exception is thrown during the execution of std::make_unique<>,
    // code execution never reaches the critical section of popping the queue
    // and as such, our data remains intact.
    std::shared_ptr<T> target_value {std::make_unique<T>(m_data_queue.front())};
    m_data_queue.pop();
    return target_value;
}

template <typename T>
bool threadsafe_queue<T>::try_pop(T& out) {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (m_data_queue.empty()) {
        return false;
    }
    out = m_data_queue.front();
    m_data_queue.pop();
}

template <typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop() {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (m_data_queue.empty()) {
        return nullptr;
    }
    std::shared_ptr<T> target_data {std::make_unique<T>(m_data_queue.front())};
    m_data_queue.pop();
    return target_data;
}