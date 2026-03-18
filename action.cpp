#include "action.h"

Action::Action(std::string srcMac, std::string dstMac) {
    this->srcMac = srcMac;
    this->dstMac = dstMac;
}

std::string Action::getSrcMac() {
    return this->srcMac;
}

std::string Action::getDstMac() {
    return this->dstMac;
}