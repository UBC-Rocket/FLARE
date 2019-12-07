#ifndef STATE_INPUT_STRUCT_H
#define STATE_INPUT_STRUCT_H

struct StateInput {
    bool ignitor_good = false;
    float altitude;
};

struct StateAuxilliaryInfo {
    bool in_abort = false;
}

#endif
