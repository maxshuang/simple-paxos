#include "catch.hpp"
#include "network.h"
#include "event.h"
#include "test_actor.h"

TEST_CASE("Network Send event to the correct actor", "[Network Send]")
{
    // Create two actors
    std::map<int, Actor *> actors;
    TestActor actor1(1);
    actors[1] = &actor1;
    TestActor actor2(2);
    actors[2] = &actor2;

    // Create a Network object
    NetworkBase network(actors);

    // Create and Send an event
    Event event;
    event.src_id = 1;
    event.dst_id = 2;
    network.Send(event);
    network.Send(event);
    network.Send(event);
    network.Send(event);
    network.Send(event);
    network.Send(event);

    // Create and Send an event
    Event event2;
    event.src_id = 2;
    event.dst_id = 1;
    network.Send(event2);
    network.Send(event2);
    network.Send(event2);
    network.Send(event2);
}