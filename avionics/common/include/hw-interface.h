#pragma once
/**
 * Generic Hardware Interface
 */

#include "status_enums.h"

class IIgnitor {
  public:
    /**
     * @brief initializes hardware
     */
    // This should be done in the constructor, not as a seperate 2-phase
    // initializer.
    //  virtual void init() = 0;

    /**
     * @brief returns the status of the hardware
     * @return the hardware's status
     */
    HardwareStatus getStatus() { return status; }

    /**
     * @brief returns true if parachute works, false otherwise
     * @return boolean
     */
    // virtual bool isWorking() = 0;

    /**
     * @brief fires the ignitor
     */
    virtual void fire() = 0;

  protected:
    HardwareStatus status;
};
