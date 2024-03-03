#pragma once
#include <mutex>

class IdGenerator {
public:
    IdGenerator() : id_(1) {}
    int GenerateId(){
        std::lock_guard<std::mutex> lock(mtx_);
        return id_++;
    }
private:
    int id_;
    std::mutex mtx_;
};

extern IdGenerator g_id_generator;