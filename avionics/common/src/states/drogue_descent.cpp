#include "states/drogue_descent.h"

StateId State::DrogueDescent::getNewState(const StateInput &input,
                                          StateAuxilliaryInfo &) {
    int8_t num_checks = 0;
    if (input.altitude < MAIN_DEPLOY_ALTITUDE_) {
        num_checks++;
    } else {
        num_checks = 0;
    }

    if (num_checks >= DEPLOY_CHECKS_) {
        return post_descent_;
    } else {
        return StateId::DROGUE_DESCENT;
    }
}
