#pragma once
#include <map>
#include <thread>
#include "event.h"
#include "queue.h"
#include "actor.h"

class NetworkBase : public Actor
{
public:
    NetworkBase(const std::map<int, Actor *> &actors) : Actor(), actors_(actors){};
    virtual ~NetworkBase(){};
    void Receive(const Event &event);

private:
    virtual void onEvent(const Event &event);

protected:
    std::map<int, Actor *> actors_;
};

// SimulatedNetwork is a network that simulates the network delay, packet loss and packet duplication.
class SimulatedNetwork : public NetworkBase
{
public:
    SimulatedNetwork(const std::map<int, Actor *> &actors, float packet_loss_rate=0.0, float packet_duplication_rate=0.0,
                     int max_delay_ms=1000, int min_delay_ms=50) : NetworkBase(actors),
                                                           packet_loss_rate_(packet_loss_rate), packet_duplication_rate_(packet_duplication_rate),
                                                           max_delay_ms_(max_delay_ms), min_delay_ms_(min_delay_ms), timeout_queue_(kMaxQueueCapacity)
    {
        timeout_thread_ = std::thread(&SimulatedNetwork::dealTimeout, this);
    };
    virtual ~SimulatedNetwork(){};

private:
    virtual void onEvent(const Event &event);
    void dealTimeout();
    bool simulatePacketLoss();
    void simulateNetworkDelay(TimeoutEvent &event);
    void simulatePacketDuplication(TimeoutEvent &event);

    static const int kMaxQueueCapacity = 100000;
    float packet_loss_rate_ = 0.0;
    float packet_duplication_rate_ = 0.0;
    int max_delay_ms_ = 1000;
    int min_delay_ms_ = 50;
    TimeoutQueue timeout_queue_;
    std::thread timeout_thread_;
};