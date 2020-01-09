#ifndef HAL__PORT_H_56C3E43133274FDA9B613D6E471DDF40
#define HAL__PORT_H_56C3E43133274FDA9B613D6E471DDF40

namespace Hal {

class ISerial {
public:
    virtual void begin(long baud) = 0;
    virtual bool available() = 0;
    virtual int read() = 0;
    virtual void write(const uint8_t *buffer, size_t size) = 0;
    virtual void print(char *dat) = 0;
    virtual void println(char *dat) = 0;

    virtual operator bool() = 0;
}

} // namespace Hal


#endif
