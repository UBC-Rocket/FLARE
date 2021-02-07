#pragma once

#include <memory>

#include "hardware/ignitor.h"

class IgnitorCollection {
  public:
    IgnitorCollection();

    Ignitor drogue;
    Ignitor main;
    const uint8_t *getStatusBitfield() const { return status_bitfield_; }
    RocketStatus getStatus() const { return status_; }

  private:
    uint8_t status_bitfield_[2];
    RocketStatus status_;
};
