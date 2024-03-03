#include <iostream>
#include <map>
#include "learner.h"

void Learner::Receive(const Event &event)
{
    switch (event.type)
    {
    case EventType::kLearn:
        onLearn(event);
        break;
    default:
        std::cout << "learner id:" << id_ << ", wrong event type:" << static_cast<int>(event.type) << std::endl;
        break;
    }
}

void Learner::onLearn(const Event &event)
{
    Learn request = DecodeLearn(event);
    if (request.learner_id == id_ && request.accepted_proposal_number > accepted_proposal_number_[request.acceptor_id])
    {
        accepted_proposal_number_[request.acceptor_id] = request.accepted_proposal_number;
        accepted_proposal_value_[request.acceptor_id] = request.accepted_proposal_value;
    }
    checkChosenValue();
}

void Learner::checkChosenValue()
{
    std::map<int, int> value_count;
    for (auto &kv : accepted_proposal_value_)
    {
        if (kv.second != -1)
        {
            value_count[kv.second]++;
        }
    }

    for (auto &p : value_count)
    {
        if (p.second >= quarum_)
        {
            chosen_value_ = p.first;
            return;
        }
    }
}