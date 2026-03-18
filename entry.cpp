#include "entry.h"
#include "action.h"

Entry::Entry() {
    
}

std::vector<Action> Entry::getActions() {
    return this->actions;
}

bool Entry::addAction(Action input) {
    this->actions.push_back(input);
    return true;
}