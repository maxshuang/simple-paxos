#pragma once
#include <atomic>
#include "event.h"
#include "queue.h"

class Actor{
public:
    Actor():running_(false), message_queue_(kMaxQueueCapacity){}
    virtual ~Actor(){}
    void Run() {
        running_ = true;
        while (running_) {
            Event event = message_queue_.Peak();
            Receive(event);
            message_queue_.Pop();
        }
    }
    void Stop(){ running_ = false; }
    void Send(const Event& event){
        message_queue_.Push(event);
    }
    virtual void Receive(const Event& event)=0;
private:
    static const int kMaxQueueCapacity = 100000;
    std::atomic_bool running_;
    BlockingQueue   message_queue_;
};