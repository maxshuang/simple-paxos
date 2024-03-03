#include "actor.h"
#include "event.h"
#include "catch.hpp"

class TestActor : public Actor
{
public:
    TestActor(int id) : Actor(), id_(id) {}
    void Receive(const Event &event) override {
        REQUIRE(event.dst_id == id_);
    }

private:
    int id_;
};