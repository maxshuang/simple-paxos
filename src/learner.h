#pragma once
#include <vector>
#include <map>
#include "actor.h"
#include "event.h"

class Learner: public Actor{
public:
    Learner(int id, const std::vector<int>& acceptors):Actor(), id_(id), chosen_value_(-1), 
        acceptor_count_(acceptors.size()), quarum_((acceptor_count_ + 1) / 2), acceptors_(acceptors){
        for(int i: acceptors_){
            accepted_proposal_value_[i] = -1;
            accepted_proposal_number_[i] = -1;
        }
    }
    ~Learner(){};
    void Receive(const Event& event);
    int ChosenValue() const {return chosen_value_;}
    std::map<int, int> AcceptedProposalValue() const {return accepted_proposal_value_;}

private:
    void onLearn(const Event& event);
    void checkChosenValue();

    int id_;
    int chosen_value_;
    int acceptor_count_;
    int quarum_;
    std::vector<int> acceptors_;
    std::map<int, int> accepted_proposal_value_;
    std::map<int, int> accepted_proposal_number_;
};