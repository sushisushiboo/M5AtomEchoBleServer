#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

#include "distance.h"

Distance::Distance(const uint8_t i2c_sda_pin, const uint8_t i2s_scl_pin,
        const uint8_t shutdown_pin, const uint8_t interrupt_pin) {

    this->_interrupt_pin = interrupt_pin;
    Wire.begin(i2c_sda_pin, i2s_scl_pin);
    this->_vl53l1x = new SFEVL53L1X(Wire, shutdown_pin, interrupt_pin);
    if (0 <= interrupt_pin) {
        pinMode(interrupt_pin, INPUT);
    }
    if (0 != this->_vl53l1x->begin()) //Begin returns 0 on a good init
    {
        Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
        this->_vl53l1x = NULL;
        return;
    }

    Serial.println("VL531X sensor initial success.");

    Serial.print("Signal Threshold[kcps]: ");
    Serial.println(this->_vl53l1x->getSignalThreshold()); //Returns the signal threshold in kcps
    Serial.print("Sigma Threshold[mm]: ");
    Serial.println(this->_vl53l1x->getSigmaThreshold()); //Returns the current sigma threshold.
};

Distance::~Distance() {
};

void Distance::setThreshold(uint16_t min, uint16_t max, Distance::VL53L1X_WINDOW_MODE mode) {
    if (NULL != this->_vl53l1x) {
        this->_vl53l1x->setDistanceThreshold(min, max, (uint8_t)mode);
    }    
}

bool Distance::start() {
    this->_vl53l1x->startRanging();
    this->_ranging = true;
}

bool Distance::stop() {
    if (this->_ranging) {
        this->_vl53l1x->stopRanging();
    }
}

bool Distance::isRanged() {
    if (0 < digitalRead(this->_interrupt_pin)) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief 距離の取得
 * 無効な距離の場合は0
 * @return uint16_t 距離[mm]
 */
uint16_t Distance::getDistance() {
    if (!this->_vl53l1x->checkForDataReady()) {
        Serial.println("vl53l1x data not ready");
        return 0;
    }
    uint16_t distance = this->_vl53l1x->getDistance();;
    this->_vl53l1x->clearInterrupt();
    uint8_t rangeStatus = this->_vl53l1x->getRangeStatus(); 
#if 0
    switch(rangeStatus) {
        case 0: // good
            Serial.println("vl53l1x range status: good");
            break;
        case 1: // sigma fail
            Serial.println("vl53l1x range status: sigma fail");
            break;;
        case 2: // signal fail
            Serial.println("vl53l1x range status: signal fail");
            break;
        case 7: // wrapped target fail
            Serial.println("vl53l1x range status: wrapped target fail");
            break;
        default: // unknown
            Serial.println("vl53l1x range status: failed");
            break;
    }
#endif
    if (0 == rangeStatus) {
        // Serial.print("¥tvl53lx1 signal rate: ");
        // Serial.println(this->_vl53l1x->getSignalRate());
        Serial.print("vl53lx1 distance[mm]: ");
        Serial.println(distance);
    } else {
        distance = 0;
    }

    return distance;
}
