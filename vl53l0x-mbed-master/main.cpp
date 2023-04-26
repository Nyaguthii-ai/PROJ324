/*
 * This example shows how to use continuous mode to take
 * range measurements with the VL53L0X.
 */
#include <mbed.h>
#include <VL53L0X.h>

DigitalOut hsens_power(SENS_POWER_ON,1); // power on hSens terminals on CORE2
VL53L0X sensor(SENS1_PIN4, SENS1_PIN3);

int main()
{
    printf("Start...\r\n");
    sensor.init();
    printf("Initialisation completed!\r\n");
    sensor.setTimeout(500);
    // Start continuous back-to-back mode (take readings as
    // fast as possible).  To use continuous timed mode
    // instead, provide a desired inter-measurement period in
    // ms (e.g. sensor.startContinuous(100)).
    sensor.startContinuous(0);
    while (1)
    {
        printf("%u\r\n", sensor.readRangeContinuousMillimeters());
        if (sensor.timeoutOccurred())
        {
            printf("TIMEOUT!\r\n");
        }
    }
}