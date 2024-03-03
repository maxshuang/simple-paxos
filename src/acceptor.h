#pragma once 
#include "actor.h"
#include "network.h"

class Acceptor:public Actor{
public:
    Acceptor(int id, const std::vector<int>& learners): Actor(), id_(id), accepted_proposal_number_(-1), 
        accepted_proposal_value_(-1), promised_proposal_number_(-1), learners_(learners), net_(nullptr){};
    ~Acceptor(){};
    void SetNetwork(Actor *net) {net_ = net;}
    void Receive(const Event& event);

private:
    void onPrepare(const Event& event);
    void onAccept(const Event& event);
    void notifyLearners();

    int id_;
    int accepted_proposal_number_;
    int accepted_proposal_value_;
    int promised_proposal_number_;
    std::vector<int> learners_;
    Actor *net_;
};