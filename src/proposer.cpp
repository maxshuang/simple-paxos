#include <iostream>
#include <future>
#include <algorithm>
#include "proposer.h"
#include "network.h"
#include "id_generator.h"

IdGenerator g_id_generator;

Proposer::Proposer(int id, int value, int timeout, const std::vector<int> &acceptors)
    : Actor(),
      id_(id),
      value_(value),
      proposal_number_(0),
      acceptor_count_(acceptors.size()),
      quorum_(acceptor_count_ / 2 + 1),
      timeout_(timeout),
      prepare_ok_count_(0),
      prepare_error_count_(0),
      accept_ok_count_(0),
      accept_error_count_(0),
      accepted_highest_proposal_number_(-1),
      accepted_highest_proposal_value_(-1),
      acceptors_(acceptors),
      state_(State::kNone),
      net_(nullptr)

{
    for (int i : acceptors_)
    {
        prepare_ok_[i] = false;
        accept_ok_[i] = false;
    }
}

void Proposer::Start()
{
    if (State::kNone == state_)
    {
        this->Send(EncodePrepareStart(PrepareStart{.proposer_id = id_}));
    }
}

void Proposer::Receive(const Event &event)
{
    switch (event.type)
    {
    case EventType::kPrepareStart:
        onPrepareStart(event);
        break;
    case EventType::kPrepareResponse:
        onPrepareReply(event);
        break;
    case EventType::kAcceptResponse:
        onAcceptReply(event);
        break;
    case EventType::kPrepareTimeout:
        onPrepareTimeout(event);
        break;
    case EventType::kAcceptTimeout:
        onAcceptTimeout(event);
        break;
    default:
        std::cout << "proposal id:" << id_ << ", wrong event type:" << static_cast<int>(event.type) << std::endl;
        break;
    }
}

void Proposer::prepare()
{
    prepare_ok_count_ = 0;
    prepare_error_count_ = 0;
    for (int i : acceptors_)
    {
        prepare_ok_[i] = false;
    }
    accepted_highest_proposal_number_ = -1;
    accepted_highest_proposal_value_ = -1;

    proposal_number_ = g_id_generator.GenerateId();
    std::cout << "start prepare, proposal id:" << id_ << ", proposal number:" << proposal_number_ << std::endl;
    PrepareRequest request;
    request.proposer_id = id_;
    request.proposal_number = proposal_number_;
    std::vector<int> acceptors = randomSelectAcceptors();
    for (int i : acceptors)
    {
        request.acceptor_id = i;
        net_->Send(EncodePrepareRequest(request));
    }
    state_ = State::kWaitPrepareReply;
    setTimeoutTimer(proposal_number_);
}

void Proposer::setTimeoutTimer(int cur_proposal_number)
{
    std::future<void> timer = std::async(std::launch::async, [this, cur_proposal_number]()
                                         {
        std::this_thread::sleep_for(std::chrono::seconds(this->timeout_));
        if(this->proposal_number_ != cur_proposal_number)
        {
            return;
        }
        if (state_ == State::kWaitPrepareReply) {
            PrepareTimeout timeout;
            timeout.proposer_id = this->id_;
            timeout.proposal_number = cur_proposal_number;
            this->Send(EncodePrepareTimeout(timeout));
        }else if(state_ == State::kWaitAcceptReply ){
            AcceptTimeout timeout;
            timeout.proposer_id = this->id_;
            timeout.proposal_number = cur_proposal_number;
            this->Send(EncodeAcceptTimeout(timeout));
        } });
}

void Proposer::accept()
{
    accept_ok_count_ = 0;
    accept_error_count_ = 0;
    for (int i : acceptors_)
    {
        accept_ok_[i] = false;
    }
    if (accepted_highest_proposal_number_ != -1)
    {
        value_ = accepted_highest_proposal_value_;
    }

    AcceptRequest request;
    request.proposer_id = id_;
    request.proposal_number = proposal_number_;
    request.proposal_value = value_;
    std::vector<int> acceptors = randomSelectAcceptors();
    for (int i : acceptors)
    {
        request.acceptor_id = i;
        net_->Send(EncodeAcceptRequest(request));
    }
    state_ = State::kWaitAcceptReply;
    setTimeoutTimer(proposal_number_);
}

void Proposer::onPrepareStart(const Event &event)
{
    PrepareStart start = DecodePrepareStart(event);
    if (start.proposer_id == id_ && State::kNone == state_)
    {
        state_ = State::kPrepare;
        prepare();
    }
}

void Proposer::onPrepareTimeout(const Event &event)
{
    PrepareTimeout timeout = DecodePrepareTimeout(event);
    if (timeout.proposal_number == proposal_number_ && State::kWaitPrepareReply == state_)
    {
        std::cout << "prepare timeout, restart prepare, proposal id:" << id_ << std::endl;
        state_ = State::kPrepare;
        prepare();
    }
}

void Proposer::onAcceptTimeout(const Event &event)
{
    AcceptTimeout timeout = DecodeAcceptTimeout(event);
    if (timeout.proposal_number == proposal_number_ && State::kWaitAcceptReply == state_)
    {
        std::cout << "accept timeout, restart prepare, proposal id:" << id_ << std::endl;
        state_ = State::kPrepare;
        prepare();
    }
}

std::vector<int> Proposer::randomSelectAcceptors()
{
    std::random_shuffle(acceptors_.begin(), acceptors_.end());
    return std::vector<int>(acceptors_.begin(), acceptors_.begin() + quorum_);
}

void Proposer::onPrepareReply(const Event &event)
{
    PrepareResponse response = DecodePrepareResponse(event);
    /*
    std::cout << "onPrepareReply, proposal id:" << id_
              << ", acceptor id:" << response.acceptor_id
              << ", proposal number:" << response.proposal_number
              << ", accepted proposal number:" << response.accepted_proposal_number
              << ", accepted proposal value:" << response.accepted_proposal_value
              << std::endl;
    */
    if (response.proposer_id == id_ && response.proposal_number == proposal_number_ && state_ == State::kWaitPrepareReply &&
        prepare_ok_[response.acceptor_id] == false)
    {
        prepare_ok_[response.acceptor_id] = true;
        if (response.error == 0)
        {
            prepare_ok_count_++;
            setAcceptedHighestProposal(response);
            if (prepare_ok_count_ == quorum_)
            {
                state_ = State::kAccept;
                accept();
            }
        }
        else
        {
            // std::cout << "prepare error, proposal id:" << id_ << ", acceptor id:" << response.acceptor_id
            //           << ", proposal number:" << response.proposal_number << std::endl;
            prepare_error_count_++;
            if (prepare_error_count_ == quorum_)
            {
                std::cout << "too many prepare error, restart prepare, proposal id:" << id_ << std::endl;
                state_ = State::kPrepare;
                prepare();
            }
        }
    }
}

void Proposer::setAcceptedHighestProposal(const PrepareResponse &response)
{
    if (response.accepted_proposal_number != -1 && response.accepted_proposal_number > accepted_highest_proposal_number_)
    {
        accepted_highest_proposal_number_ = response.accepted_proposal_number;
        accepted_highest_proposal_value_ = response.accepted_proposal_value;
    }
}

void Proposer::onAcceptReply(const Event &event)
{
    AcceptResponse response = DecodeAcceptResponse(event);
    /*
    std::cout << "onAcceptReply, proposal id:" << id_
              << ", acceptor id:" << response.acceptor_id
              << ", proposal number:" << response.proposal_number << std::endl;
    */
    if (response.proposer_id == id_ && response.proposal_number == proposal_number_ && state_ == State::kWaitAcceptReply &&
        accept_ok_[response.acceptor_id] == false)
    {
        accept_ok_[response.acceptor_id] = true;
        if (response.error == 0)
        {
            accept_ok_count_++;
            if (accept_ok_count_ == quorum_)
            {
                // NOTE: This state will end the whole actor
                // Need outside learner to re-trigger the proposer
                state_ = State::kNone;
            }
        }
        else
        {
            // std::cout << "acceptor error, proposal id:" << id_ << ", acceptor id:" << response.acceptor_id
            //           << ", proposal number:" << response.proposal_number << std::endl;
            accept_error_count_++;
            if (accept_error_count_ == quorum_)
            {
                std::cout << "too many accept error, restart prepare, proposal id:" << id_ << std::endl;
                state_ = State::kPrepare;
                prepare();
            }
        }
    }
}
