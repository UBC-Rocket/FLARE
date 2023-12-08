#pragma once

#include <memory>

#include "hardware/ignitor.h"

class IgnitorCollection {
  public:

    /**
     * @brief Construct a new Ignitor Collection object
     */
    IgnitorCollection();

    Ignitor drogue;
    Ignitor main;

    /**
     * @brief Get the status_bitfield_ of IgnitorCollection
     * @return status_bitfield_
     */
    const uint8_t *getStatusBitfield() const { return status_bitfield_; }

    /**
     * @brief Get the status_ of IgnitorCollection
     * @return status_
     */
    RocketStatus getStatus() const { return status_; }

  private:
    uint8_t status_bitfield_[2];
    RocketStatus status_;
};
