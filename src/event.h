#pragma once
#include <string>
#include <cstring>

enum class EventType
{
    kNone = 0,
    kPrepareStart,
    kPrepareRequest,
    kPrepareResponse,
    kPrepareTimeout,
    kAcceptRequest,
    kAcceptResponse,
    kAcceptTimeout,
    kLearn,
};

struct Event
{
    EventType type;
    int src_id;
    int dst_id;
    std::string data;
};

struct PrepareStart
{
    int proposer_id;
};

inline Event EncodePrepareStart(const PrepareStart &start)
{
    Event event;
    event.type = EventType::kPrepareStart;
    event.src_id = start.proposer_id;
    event.dst_id = start.proposer_id;
    return event;
}

inline PrepareStart DecodePrepareStart(const Event &event)
{
    PrepareStart start;
    start.proposer_id = event.src_id;
    return start;
}

struct PrepareRequest
{
    int proposer_id;
    int acceptor_id;
    int proposal_number;
};

inline Event EncodePrepareRequest(const PrepareRequest &request)
{
    Event event;
    event.type = EventType::kPrepareRequest;
    event.src_id = request.proposer_id;
    event.dst_id = request.acceptor_id;
    event.data.assign((char *)&request, sizeof(request));
    return event;
}

inline PrepareRequest DecodePrepareRequest(const Event &event)
{
    PrepareRequest request;
    memcpy(&request, event.data.data(), event.data.size());
    return request;
}

struct PrepareResponse
{
    int proposer_id;
    int acceptor_id;
    int proposal_number;
    int accepted_proposal_number;
    int accepted_proposal_value;
    int error;
};

inline Event EncodePrepareResponse(const PrepareResponse &response)
{
    Event event;
    event.type = EventType::kPrepareResponse;
    event.src_id = response.acceptor_id;
    event.dst_id = response.proposer_id;
    event.data.assign((char *)&response, sizeof(response));
    return event;
}

inline PrepareResponse DecodePrepareResponse(const Event &event)
{
    PrepareResponse response;
    memcpy(&response, event.data.data(), event.data.size());
    return response;
}

struct PrepareTimeout
{
    int proposer_id;
    int proposal_number;
};

inline Event EncodePrepareTimeout(const PrepareTimeout &timeout)
{
    Event event;
    event.type = EventType::kPrepareTimeout;
    event.src_id = timeout.proposer_id;
    event.dst_id = timeout.proposer_id;
    event.data.assign((char *)&timeout, sizeof(timeout));
    return event;
}

inline PrepareTimeout DecodePrepareTimeout(const Event &event)
{
    PrepareTimeout timeout;
    memcpy(&timeout, event.data.data(), event.data.size());
    return timeout;
}

//////////////////////////////////////////////////////////////////////////////////

struct AcceptRequest
{
    int proposer_id;
    int acceptor_id;
    int proposal_number;
    int proposal_value;
};

inline Event EncodeAcceptRequest(const AcceptRequest &request)
{
    Event event;
    event.type = EventType::kAcceptRequest;
    event.src_id = request.proposer_id;
    event.dst_id = request.acceptor_id;
    event.data.assign((char *)&request, sizeof(request));
    return event;
}

inline AcceptRequest DecodeAcceptRequest(const Event &event)
{
    AcceptRequest request;
    memcpy(&request, event.data.data(), event.data.size());
    return request;
}

struct AcceptResponse
{
    int proposer_id;
    int acceptor_id;
    int proposal_number;
    int error;
};

inline Event EncodeAcceptResponse(const AcceptResponse &response)
{
    Event event;
    event.type = EventType::kAcceptResponse;
    event.src_id = response.acceptor_id;
    event.dst_id = response.proposer_id;
    event.data.assign((char *)&response, sizeof(response));
    return event;
}

inline AcceptResponse DecodeAcceptResponse(const Event &event)
{
    AcceptResponse response;
    memcpy(&response, event.data.data(), event.data.size());
    return response;
}

struct AcceptTimeout
{
    int proposer_id;
    int proposal_number;
};

inline Event EncodeAcceptTimeout(const AcceptTimeout &timeout)
{
    Event event;
    event.type = EventType::kAcceptTimeout;
    event.src_id = timeout.proposer_id;
    event.dst_id = timeout.proposer_id;
    event.data.assign((char *)&timeout, sizeof(timeout));
    return event;
}

inline AcceptTimeout DecodeAcceptTimeout(const Event &event)
{
    AcceptTimeout timeout;
    memcpy(&timeout, event.data.data(), event.data.size());
    return timeout;
}

/////////////////////////////////////////////////////////////////////

struct Learn
{
    int acceptor_id;
    int learner_id;
    int accepted_proposal_number;
    int accepted_proposal_value;
};

inline Event EncodeLearn(const Learn &learn)
{
    Event event;
    event.type = EventType::kLearn;
    event.src_id = learn.acceptor_id;
    event.dst_id = learn.learner_id;
    event.data.assign((char *)&learn, sizeof(learn));
    return event;
}

inline Learn DecodeLearn(const Event &event)
{
    Learn learn;
    memcpy(&learn, event.data.data(), event.data.size());
    return learn;
}