#ifndef SYNCPP_WAIT_GROUP_H
#define SYNCPP_WAIT_GROUP_H
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace syncpp 
{

class wait_group 
{
public:

    wait_group() = default;

    ~wait_group() = default;
    
    void add(int count)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _wait_count += count; 
    }
    
    void done()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _wait_count--;
        lock.unlock();
        _condition_variable.notify_one();
    }
    
    void wait()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition_variable.wait(lock, 
        [&]
        {
            return _wait_count == 0;
        });
    }

    template<class Rep,class Period> 
    void wait_for(const std::chrono::duration<Rep, Period>& duration)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition_variable.wait_for(lock,duration, 
        [&]
        {
            return _wait_count == 0;
        });
    }

    template<class Clock,class Duration>
    void wait_until(const std::chrono::time_point<Clock, Duration>& timeout)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition_variable.wait_until(lock,timeout, 
        [&]
        {
            return _wait_count == 0;
        });
    }
    
private:
    std::mutex _mutex;
    std::condition_variable _condition_variable;
    int _wait_count;
};

}

#endif