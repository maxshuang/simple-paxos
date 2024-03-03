#include <iostream>
#include "acceptor.h"

void Acceptor::Receive(const Event &event)
{
    switch (event.type)
    {
    case EventType::kPrepareRequest:
        onPrepare(event);
        break;
    case EventType::kAcceptRequest:
        onAccept(event);
        break;
    default:
        std::cout << "acceptor id:" << id_ << ", wrong event type:" << static_cast<int>(event.type) << std::endl;
        break;
    }
}

void Acceptor::onPrepare(const Event &event)
{
    PrepareRequest request = DecodePrepareRequest(event);
    /*
    std::cout << "onPrepare, proposer id:" << request.proposer_id
              << ", acceptor id:" << id_
              << ", proposal number:" << request.proposal_number << std::endl;
    */
    PrepareResponse response;
    response.proposer_id = request.proposer_id;
    response.acceptor_id = id_;
    response.proposal_number = request.proposal_number;
    if (request.proposal_number > promised_proposal_number_)
    {
        promised_proposal_number_ = request.proposal_number;
        response.accepted_proposal_number = accepted_proposal_number_;
        response.accepted_proposal_value = accepted_proposal_value_;
        response.error = 0;
        //std::cout << "Accept Prepare, proposer id:" << request.proposer_id
        //      << ", acceptor id:" << id_
        //      << ", proposal number:" << request.proposal_number << std::endl;
    }
    else
    {
        response.error = 1;
    }

    net_->Send(EncodePrepareResponse(response));
}

void Acceptor::onAccept(const Event &event)
{
    AcceptRequest request = DecodeAcceptRequest(event);
    /*
    std::cout << "onAccept, acceptor id:" << id_
              << ", proposer id:" << request.proposer_id
              << ", proposal number:" << request.proposal_number
              << ", proposal value:" << request.proposal_value
              << std::endl;
    */
    AcceptResponse response;
    response.proposer_id = request.proposer_id;
    response.acceptor_id = id_;
    response.proposal_number = request.proposal_number;
    if (request.proposal_number >= promised_proposal_number_)
    {
        promised_proposal_number_ = request.proposal_number;
        accepted_proposal_number_ = request.proposal_number;
        accepted_proposal_value_ = request.proposal_value;
        notifyLearners();
        response.error = 0;
        //std::cout << "Accept Proposal, proposer id:" << request.proposer_id
        //      << ", acceptor id:" << id_
        //      << ", proposal number:" << request.proposal_number 
        //      << ", proposal value:" << request.proposal_value
        //    << std::endl;
    }
    else
    {
        response.error = 1;
    }
    /*
    std::cout << "onAccept response, acceptor id:" << id_
              << ", proposer id:" << request.proposer_id
              << ", proposal number:" << request.proposal_number
              << ", proposal value:" << request.proposal_value
              << ", error:" << response.error
              << std::endl;
    */
    net_->Send(EncodeAcceptResponse(response));
}

void Acceptor::notifyLearners()
{
    Learn learn;
    learn.acceptor_id = id_;
    learn.accepted_proposal_number = accepted_proposal_number_;
    learn.accepted_proposal_value = accepted_proposal_value_;
    for (int learner : learners_)
    {
        learn.learner_id = learner;
        net_->Send(EncodeLearn(learn));
    }
}