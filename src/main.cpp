#include <vector>
#include <iostream>
#include "proposer.h"
#include "acceptor.h"
#include "learner.h"
#include "network.h"
#include "event.h"

void outputAcceptorState(Learner* learner)
{
    auto accepted_proposal_value = learner->AcceptedProposalValue();
    for (auto &kv : accepted_proposal_value)
    {
        std::cout << "acceptor:" << kv.first << " proposal_value:" << kv.second << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    int proposer_count = 3;
    int acceptor_count = 5;
    int learner_count = 2;

    std::vector<int> proposers;
    std::vector<int> acceptors;
    std::vector<int> learners;
    int cur = 0;
    for (int i = 0; i < proposer_count; i++, cur++)
    {
        proposers.push_back(cur);
    }
    for (int i = 0; i < acceptor_count; i++, cur++)
    {
        acceptors.push_back(cur);
    }
    for (int i = 0; i < learner_count; i++, cur++)
    {
        learners.push_back(cur);
    }

    std::map<int, Actor *> actors;
    for (int i : proposers)
    {
        actors[i] = new Proposer(i, i, 3, acceptors);
    }
    for (int i : acceptors)
    {
        actors[i] = new Acceptor(i, learners);
    }
    for (int i : learners)
    {
        actors[i] = new Learner(i, acceptors);
    }

    SimulatedNetwork net(actors);
    for (int i : proposers)
    {
        dynamic_cast<Proposer *>(actors[i])->SetNetwork(&net);
    }
    for (int i : acceptors)
    {
        dynamic_cast<Acceptor *>(actors[i])->SetNetwork(&net);
    }

    std::thread net_thread(&Actor::Run, &net);

    // every role has a independent thread
    std::vector<std::thread> threads;
    for (auto &kv : actors)
    {
        threads.push_back(std::thread(&Actor::Run, kv.second));
    }

    // send prepare request
    Learner *learner = dynamic_cast<Learner *>(actors[learners[0]]);
    //while (learner->ChosenValue() == -1)
    while(true)
    {
        std::cout << "chosen value:" << learner->ChosenValue() << std::endl;
        outputAcceptorState(learner);
        
        for (int i : proposers)
        {
            dynamic_cast<Proposer *>(actors[i])->Start();
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    //outputAcceptorState(learner);
    //std::cout << "chosen value:" << learner->ChosenValue() << std::endl;

    // stop and release resources
    // [NOTE]： blocking here, need deal with it
    for (auto &a : actors)
    {
        a.second->Stop();
    }
    net.Stop();

    for (auto &t : threads)
    {
        t.join();
    }
    net_thread.join();

    for (auto &a : actors)
    {
        delete a.second;
    }

    return 0;
}