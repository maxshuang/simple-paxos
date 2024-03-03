#include <thread>
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "queue.h"
#include "event.h"

TEST_CASE("Push and Pop events from BlockingQueue", "[BlockingQueue]")
{
    BlockingQueue queue(1000); // Create a BlockingQueue with capacity 3

    SECTION("Push and Pop a single event")
    {
        Event event1;
        queue.Push(event1);

        Event poppedEvent=queue.Peak();
        REQUIRE(queue.Size() == 1);
        queue.Pop();
        REQUIRE(queue.Size() == 0);
    }

    SECTION("Push and Pop multiple events")
    {
        Event event1;
        Event event2;
        Event event3;

        queue.Push(event1);
        queue.Push(event2);
        queue.Push(event3);
        queue.Push(event3);
        queue.Push(event3);
        queue.Push(event3);
        REQUIRE(queue.Size() == 6);

        Event poppedEvent1=queue.Peak();
        queue.Pop();
        Event poppedEvent2=queue.Peak();
        queue.Pop();
        Event poppedEvent3=queue.Peak();
        queue.Pop();
        REQUIRE(queue.Size() == 3);
    }

    // Add more test cases as needed
}

TEST_CASE("Push and Pop events from TimeoutQueue", "[TimeoutQueue]")
{
    TimeoutQueue queue(3); // Create a TimeoutQueue with capacity 3

    SECTION("Push and Pop a single event")
    {
        TimeoutEvent event1;
        queue.Push(event1);

        const TimeoutEvent& poppedEvent=queue.Peak();
        REQUIRE(queue.Size() == 1);
        queue.Pop();
        REQUIRE(queue.Size() == 0);
        REQUIRE(event1.timeout_ms == poppedEvent.timeout_ms);
    }

    SECTION("Push and Pop multiple events")
    {
        TimeoutEvent event1;
        event1.timeout_ms = 3;
        TimeoutEvent event2;
        event2.timeout_ms = 2;
        TimeoutEvent event3;
        event3.timeout_ms = 1;

        queue.Push(event1);
        queue.Push(event2);
        queue.Push(event3);
        REQUIRE(queue.Size() == 3);

        const TimeoutEvent& poppedEvent1=queue.Peak();
        REQUIRE(event3.timeout_ms == poppedEvent1.timeout_ms);
        queue.Pop();
        TimeoutEvent poppedEvent2=queue.Peak();
        REQUIRE(event2.timeout_ms == poppedEvent2.timeout_ms);
        queue.Pop();
        TimeoutEvent poppedEvent3=queue.Peak();
        REQUIRE(event1.timeout_ms == poppedEvent3.timeout_ms);
        queue.Pop();
        REQUIRE(queue.Size() == 0);
    }

    SECTION("Push and Pop multiple timestamp events")
    {   
        int64_t ts = current_time_ms();
        TimeoutEvent event1;
        event1.timeout_ms = ts+4000;
        TimeoutEvent event2;
        event2.timeout_ms = ts+1000;
        TimeoutEvent event3;
        event3.timeout_ms = ts+2000;

        queue.Push(event1);
        queue.Push(event2);
        queue.Push(event3);

        REQUIRE(!queue.HasTimeoutEvents());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        REQUIRE(queue.HasTimeoutEvents());
        TimeoutEvent poppedEvent=queue.Peak();
        REQUIRE(poppedEvent.timeout_ms==event2.timeout_ms);
        queue.Pop();
        REQUIRE(queue.Size() == 2);

        REQUIRE(!queue.HasTimeoutEvents());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        REQUIRE(queue.HasTimeoutEvents());
        poppedEvent=queue.Peak();
        REQUIRE(poppedEvent.timeout_ms==event3.timeout_ms);
        queue.Pop();
        REQUIRE(queue.Size() == 1);
        
        REQUIRE(!queue.HasTimeoutEvents());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        REQUIRE(!queue.HasTimeoutEvents());

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        REQUIRE(queue.HasTimeoutEvents());
        poppedEvent=queue.Peak();
        REQUIRE(poppedEvent.timeout_ms==event1.timeout_ms);
    }

    // Add more test cases as needed
}

TEST_CASE("Test Current Timestamp ms", "[Current Timestamp]")
{
    int64_t ts1 = current_time_ms();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int64_t ts2 = current_time_ms();
    REQUIRE(ts2 - ts1 >= 10);
    REQUIRE(ts2 - ts1 <= 100);
    // Add more test cases as needed
}