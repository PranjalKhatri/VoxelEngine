#pragma once

#include <queue>
#include <mutex>
#include <optional>

// Thread Safe Non blocking queue
template <typename T>
class CmdQueue {
   private:
    std::queue<T>      m_queue;
    mutable std::mutex m_mutex;

   public:
    // Pushes a command - thread-safe
    void push(T cmd) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(cmd));
    }

    // Tries to pop one element. Returns std::nullopt if empty.
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return std::nullopt;
        }

        T val = std::move(m_queue.front());
        m_queue.pop();
        return val;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
};
