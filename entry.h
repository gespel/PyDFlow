#pragma once
#include <vector>
#include "action.h"

class Entry {
public:
    Entry();
    std::vector<Action> getActions();
    bool addAction(Action input);
private:
    std::vector<Action> actions;
};