#include <iostream>
#include <thread>
#include "network.h"

void NetworkBase::Receive(const Event &event)
{
    return onEvent(event);
}

void NetworkBase::onEvent(const Event &event)
{
    // direct dispatching
    // std::cout << "onEvent1, from " << event.src_id << " to " << event.dst_id << std::endl;
    if (actors_.find(event.dst_id) != actors_.end())
    {
        actors_[event.dst_id]->Send(event);
        return;
    }
    std::cout << "not find actor: " << event.dst_id << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////
void SimulatedNetwork::onEvent(const Event &event)
{
    if (simulatePacketLoss())
    {
        return;
    }

    TimeoutEvent timeout_event;
    timeout_event.event = event;
    timeout_event.timeout_ms = current_time_ms();

    simulateNetworkDelay(timeout_event);
    simulatePacketDuplication(timeout_event);
}

bool SimulatedNetwork::simulatePacketLoss()
{
    return rand() % 100 < packet_loss_rate_ * 100;
}

void SimulatedNetwork::simulateNetworkDelay(TimeoutEvent &event)
{
    int delay = rand() % (max_delay_ms_ - min_delay_ms_) + min_delay_ms_;
    event.timeout_ms += delay;
    timeout_queue_.Push(event);
    //std::cout << "simulateNetworkDelay, from " << event.event.src_id << " to " << event.event.dst_id
    //          << ", delay:" << delay << std::endl;
}

void SimulatedNetwork::simulatePacketDuplication(TimeoutEvent &event)
{
    if (rand() % 100 < packet_duplication_rate_ * 100)
    {
        int delay = rand() % (max_delay_ms_ - min_delay_ms_) + min_delay_ms_;
        event.timeout_ms += delay;
        timeout_queue_.Push(event);
        //std::cout << "simulatePacketDuplication, from " << event.event.src_id << " to " << event.event.dst_id
        //          << ", ts:" << event.timeout_ms << std::endl;
    }
}

void SimulatedNetwork::dealTimeout()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        while (timeout_queue_.HasTimeoutEvents())
        {
            //std::cout << "dealTimeout before, queue size:" << timeout_queue_.Size() << std::endl;
            const TimeoutEvent &timeout_event = timeout_queue_.Peak();
            actors_[timeout_event.event.dst_id]->Send(timeout_event.event);
            //std::cout << "dealTimeout, from " << timeout_event.event.src_id << " to " << timeout_event.event.dst_id
            //          << ", ts:" << timeout_event.timeout_ms << std::endl;
            timeout_queue_.Pop();
            //std::cout << "dealTimeout after, queue size:" << timeout_queue_.Size() << std::endl;
        }
    }
}
