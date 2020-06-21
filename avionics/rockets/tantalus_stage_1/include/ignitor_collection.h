#pragma once

#include "hardware/ignitor.h"

class IgnitorCollection {
  public:
    IgnitorCollection();

    Ignitor drogue;
    Ignitor main;
    const uint8_t *getStatusBitfield() const { return status_bitfield_; }

  private:
    uint8_t status_bitfield_[2];
};

typedef std::unique_ptr<IgnitorCollection> IgnitorCollectionPtr;
IgnitorCollectionPtr getIgnitors();
