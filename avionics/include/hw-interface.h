#ifndef HW_INTERFACE_H
#define HW_INTERFACE_H

/**
  * Generic Hardware Interface
  */

enum class HardwareStatus {
    NOMINAL,
    FAILURE
};

class IHardware {
    public:
    /**
     * @brief initializes hardware
     */
    virtual void init() = 0;

    /**
     * @brief returns the status of the hardware
     * @return the hardware's status
     */
    HardwareStatus getStatus() {
        return status;
    }

    /**
     * @brief activates the hardware
     */
    virtual void activate() = 0;

    protected:
    HardwareStatus status;
};
#endif
