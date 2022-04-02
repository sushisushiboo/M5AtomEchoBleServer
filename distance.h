#include <Arduino.h>

class SFEVL53L1X;
class Distance {
public:
    Distance(const uint8_t i2c_sda_pin, const uint8_t i2c_scl_pin,
        const uint8_t shutdown_pin = -1, const uint8_t interrupt_pin = -1);
    ~Distance();

    enum VL53L1X_WINDOW_MODE {
        below_low = 0, //0: Interrupt triggered on measured distance below lowThresh.
        above_high, //1: Interrupt triggered on measured distance above hiThresh.
        outside, //2: Interrupt triggered on measured distance outside of bounds.
        inside //3: Interrupt triggered on measured distance inside of bounds.
    };
    void setThreshold(uint16_t min, uint16_t max, Distance::VL53L1X_WINDOW_MODE mode);
    bool start();
    bool stop();
    bool isRanged();
    uint16_t getDistance();

private:
    uint8_t _interrupt_pin;
    bool _ranging = false;
    SFEVL53L1X* _vl53l1x;
};
