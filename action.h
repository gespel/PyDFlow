#pragma once
#include <string>


class Action {
public:
    Action(std::string srcMac, std::string dstMac);
    std::string getDstMac();
    std::string getSrcMac();
private:
    std::string dstMac;
    std::string srcMac;
};