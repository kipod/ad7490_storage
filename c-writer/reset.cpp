#include <iostream>
#include <wiringPi.h>

int reset_esp()
{
    if (wiringPiSetup() == -1)
    {
        std::cerr << "Cannot setup wiringPi" << std::endl;
        return 1;
    }

    // Setup GPIO 0 as output - reset
    pinMode(0, OUTPUT);
    // Setup GPIO 1 as output - boot
    pinMode(1, OUTPUT);

    digitalWrite(0, LOW);
    delay(1000);
    digitalWrite(0, HIGH);
    // # release boot button
    // gpio write 1 1
    digitalWrite(1, HIGH);
    return 0;
}