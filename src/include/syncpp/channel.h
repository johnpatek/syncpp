#ifndef SYNCPP_CHANNEL_H
#define SYNCPP_CHANNEL_H

#include <array>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

namespace syncpp
{

enum channel_state
{
    open_read,
    open_write,
    closed_read,
    closed_write
};

enum channel_status
{
    good,
    timeout,
    closed
};

template<typename Type>class channel 
{
public:
    channel()
    {
        _state = open_write;
    }
    
    ~channel()
    {
        if (_state != closed_read && _state != closed_write)
        {
            close();
        }
    }

    void close()
    {
        if (_state == open_write)
        {
            _state = closed_write;
        }
        else if (_state == open_read)
        {
            _state = closed_read;
        }
        else
        {
            throw std::runtime_error("channel is already closed");
        }
        _read_condition_variable.notify_all();
    }

    void send(const Type& value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _write_condition_variable.wait(lock,
        [&]
        { 
            return _state != open_read; 
        });
        if (_state == open_write)
        {
            _data = value;
            _state = open_read;
            lock.unlock();
            _read_condition_variable.notify_one();
        }
        else
        {
            throw std::runtime_error("unable to send to a closed channel");
        }
    }

    void send(Type&& value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _write_condition_variable.wait(lock,
        [&]
        { 
            return _state != open_read; 
        });
        if (_state == open_write)
        {
            _data = std::move(value);
            _state = open_read;
            lock.unlock();
            _read_condition_variable.notify_one();
        }
        else
        {
            throw std::runtime_error("unable to send to a closed channel");
        }
    }

    bool receive(Type& receiver)
    {
        bool result(false);
        std::unique_lock<std::mutex> lock(_mutex);
        _read_condition_variable.wait(lock,
        [&]
        { 
            return _state != open_write; 
        });
        if(_state == open_read || _state == closed_read)
        {
            receiver = std::move(_data);
            _state = (_state == open_read)?open_write:closed_write;
            result = true;
        }

        if (_state == open_write)
        {
            lock.unlock();
            _write_condition_variable.notify_one();
        }

        return result;
    }
/*
        template<class Rep,class Period> 
    bool receive_for(Type& receiver, const std::chrono::duration<Rep, Period>& duration)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition_variable.wait_for(lock,duration,_state == open_readable || _state == closed);
        if(_state == open_read)
        {
            receiver = std::move(_data);
            _state = open_write;
            result = true;
            lock.unlock();
            _write_condition_variable.notify_one();
        }
    }

    template<class Clock,class Duration>
    bool receive_until(Type& receiver, const std::chrono::time_point<Clock, Duration>& timeout)
    {
        std::unique_lock<std:shared_timed_
            receiver = std::move(_data);
            _state = open_write;
            result = true;
            lock.unlock();
            _write_condition_variable.notify_one();
        }
    }
*/
private:
    int _state;
    std::mutex _mutex;
    std::condition_variable_any _write_condition_variable;
    std::condition_variable_any _read_condition_variable;
    Type _data;
};

}

#endif