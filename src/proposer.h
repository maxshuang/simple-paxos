#pragma once
#include <thread>
#include <map>
#include <condition_variable>
#include "event.h"
#include "queue.h"
#include "actor.h"
#include "network.h"

class Proposer : public Actor
{
public:
    Proposer(int id, int value, int timeout, const std::vector<int> &acceptors);
    ~Proposer(){}
    void SetNetwork(Actor *net) { net_ = net; }
    void Receive(const Event &event);
    void Start();

private:
    enum class State
    {
        kNone = 0,
        kPrepare,
        kWaitPrepareReply,
        kAccept,
        kWaitAcceptReply,
    };

    void prepare();
    void accept();
    void onPrepareStart(const Event &event);
    void onPrepareReply(const Event &event);
    void onAcceptReply(const Event &event);
    void onPrepareTimeout(const Event &event);
    void onAcceptTimeout(const Event &event);

    void setTimeoutTimer(int cur_proposal_number);
    std::vector<int> randomSelectAcceptors();
    void setAcceptedHighestProposal(const PrepareResponse &event);

    int id_;
    int value_;
    int proposal_number_;
    int acceptor_count_;
    int quorum_;
    int timeout_;
    int prepare_ok_count_;
    int prepare_error_count_;
    int accept_ok_count_;
    int accept_error_count_;
    int accepted_highest_proposal_number_;
    int accepted_highest_proposal_value_;
    std::map<int, bool> prepare_ok_;
    std::map<int, bool> accept_ok_;
    std::vector<int> acceptors_;
    State state_;

    Actor *net_;
};