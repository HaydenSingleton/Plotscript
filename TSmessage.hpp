#ifndef TSMESSAGE_HPP
#define TSMESSAGE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class TSmessage {
    public:
        void push(const T & value){
            std::unique_lock<std::mutex> lock(mew);
            quew.push(value);
            lock.unlock();
            cond.notify_one();
        };

        bool empty() const{
            std::lock_guard<std::mutex> lock(mew);
            return quew.empty();
        };

        bool try_pop(T & value){
            std::lock_guard<std::mutex> lock(mew);
            if (quew.empty())
                return false;
            value = quew.front();
            quew.pop();
            return true;
        };

        void wait_and_pop(T & value){
            std::unique_lock<std::mutex> lock(mew);
            while (quew.empty()) {
                cond.wait(lock);
            }
            value = quew.front();
            quew.pop();
        };

    private:
        std::queue<T> quew;
        mutable std::mutex mew;
        std::condition_variable cond;
};


#endif