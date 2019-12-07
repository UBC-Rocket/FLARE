#include "states/armed.h"

State::Armed::Armed() {

}

StateId State::Armed::getStateEnum(void){
    return State::ARMED;
}

StateId State::Armed::getNewState(StateInput data){

}

