#include <M5Atom.h>
#include "ble.h"
#include "playmp3.h"

ble* myBle;
playmp3* myPlaymp3;

unsigned char value = 0;

void setup()
{
    M5.begin(true, false, true);
    delay(100);
    Serial.begin(115200);
    M5.dis.clear();
    M5.dis.drawpix(0, CRGB(128, 0, 0));

    myBle = new ble();
    myBle->setManufactureData(value);

    myPlaymp3 = new playmp3();
}

void loop()
{
    M5.update();

    if (M5.Btn.wasPressed())
    {
        value = (0 < value ? 0 : 1); // toggle
        myBle->setManufactureData(value);
        if (0 < value)
        {
            M5.dis.drawpix(0, CRGB(0, 0, 128));
        } else {
            M5.dis.drawpix(0, CRGB(128, 0, 0));
        }

        myPlaymp3->play(value);
    }

    if (myPlaymp3->isPlaying())
    {
        myPlaymp3->loop();
        delay(1);
    } else {
        delay(100);
    }
}