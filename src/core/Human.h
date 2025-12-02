#pragma once

#include <mutex>
#include <string>

struct Human {
    explicit Human(std::string name);

    std::string name;
    std::string desire;
    std::mutex mtx;
};

std::string random_desire();
