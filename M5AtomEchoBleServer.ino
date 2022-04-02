#include <M5Atom.h>
#include "ble.h"
#include "playmp3.h"
#include "distance.h"

ble* myBle;
playmp3* myPlaymp3;
Distance* myDistance;

int mp3index = 0;



#define BAT_VOL_PIN 32
#define ANALOG_MAX_VALUE 4095.0
#define ANALOG_MAX_VOLTAGE 3.3
#define BAT_DIVIDE_RATE (66.0/99.0)
#define AVERAGE_SAMPLE 10

// for VL531X
#define SHUTDOWN_PIN -1
#define INTERRUPT_PIN 26
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 25
#define DISTANCE_THRESHOLD_LOW 200 // mm
#define DISTANCE_THRESHOLD_HIGH 500 // mm

uint32_t timeToGetVoltage = 0;

float getAnalogVoltage()
{
    int value = 0;
    for( int i = 0; i < AVERAGE_SAMPLE; i++)
    {
        value += analogRead(BAT_VOL_PIN);
        delay(10);
    }
    float voltage = (float)value / (float)AVERAGE_SAMPLE 
        / ANALOG_MAX_VALUE * ANALOG_MAX_VOLTAGE 
        / BAT_DIVIDE_RATE;
    char buf[32];
    sprintf(buf, "bat voltage: %1.2f[V]", voltage);
    Serial.println(buf);
    return voltage;
}

void setup()
{
    M5.begin(true, false, true);
    delay(100);
    Serial.begin(115200);
    M5.dis.clear();
    M5.dis.drawpix(0, CRGB(128, 0, 0));

    myBle = new ble();
    myBle->setManufactureData(mp3index);

    myPlaymp3 = new playmp3();

    pinMode(BAT_VOL_PIN, INPUT);

    myDistance = new Distance(I2C_SDA_PIN, I2C_SCL_PIN, -1, INTERRUPT_PIN);
    myDistance->setThreshold(DISTANCE_THRESHOLD_LOW, DISTANCE_THRESHOLD_HIGH, Distance::VL53L1X_WINDOW_MODE::inside);
    myDistance->start();
}

void loop()
{
    M5.update();

    if (myPlaymp3->isPlaying())
    {
        M5.dis.drawpix(0, CRGB(0, 0, 128));

        myPlaymp3->loop();

        delay(1);
    } else {
        M5.dis.drawpix(0, CRGB(128, 0, 0));

        if (M5.Btn.wasPressed())
        {
            mp3index++;
            if (myPlaymp3->getFileCount() <= mp3index) {
                mp3index = 0;
            }
            myBle->setManufactureData(mp3index);
            myPlaymp3->play(mp3index);
        }
        else if (myBle->isReceiveMessage())
        {
            char buffer[128];
            myBle->getMessage(buffer, sizeof(buffer));
            sscanf(buffer, "%d", &mp3index);
            if (myPlaymp3->getFileCount() <= mp3index) {
                mp3index = 0;
            }
            myBle->setManufactureData((unsigned char)mp3index);
            myPlaymp3->play(mp3index);
        }
        else if (myBle->isConnected())
        {
            if (timeToGetVoltage < millis()) {
                timeToGetVoltage = millis() + 1000;
                float voltage = getAnalogVoltage();
                char message[32];
                sprintf(message, "bat: %1.2f[V]", voltage);
                myBle->notify(message);
            }
        }

        if (myDistance->isRanged()) {
            if (0 < myDistance->getDistance()) {
                myPlaymp3->play(mp3index);
            }
        }

        delay(50);
    }
 }