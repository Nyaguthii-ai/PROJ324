#include "mbed.h"
#include "HCSR04.h"
#include "VL53L0X.h"
//ISSUE: THIS INSTANTIATED TWICE WHICH CAUSED THE ERROR L3912W: Option 'legacyalign' is deprecated.
//#include <cstdint>
//#include <cstdio>
//#include <iostream>
using namespace std;

/*
 * This example shows how to use continuous mode to take
 * range measurements with the VL53L0X.
 */

//DigitalOut hsens_power(SENS_POWER_ON,1); // power on hSens terminals on CORE2
I2C i2c(D14,D15);
VL53L0X sensor(&i2c);

uint16_t measure = 0;
int zero = 0;
int total = 0;

int main()
{
    printf("Start...\r\n");
    sensor.init();
    printf("Initialisation completed!\r\n");
    // Start continuous back-to-back mode (take readings as
    // fast as possible).  To use continuous timed mode
    // instead, provide a desired inter-measurement period in
    // ms (e.g. sensor.startContinuous(100)).
    sensor.init();
    sensor.setModeContinuous();
    sensor.startContinuous();

 while(1)
   {
    printf("%4imm\n\r", sensor.getRangeMillimeters());
   }
}
/*
#define CM 0 

//A5(PA6) = I2C1_SCL
//A4(PA5) = I2C1_SDA

HCSR04 hcsr04(D3, D2); // Set HC-SR04 trigger and echo pins
VL53L0X lidar(PA_6,PA_5); // Set VL53L0X I2C pins sda, scl 

DigitalOut led(LED1);

int main() {
    while (1) {

        hcsr04.start();
        wait_us(500000); 

        int dist = hcsr04.get_dist_cm(); // Measure distance with HC-SR04 in centimeters

        uint16_t distance_vl53l0x = lidar.readRangeContinuousMillimeters() / 10; // Measure distance with VL53L0X in centimeters

        //cout << "Distance HC-SR04: " << dist << " cm" << endl;
        //printf("Distance HC-SRO4 %u \n",dist);

        //printf("Distance VL53L0X %u \n",distance_vl53l0x);
 

        if (dist < 20.0 || distance_vl53l0x < 20) { // If there is an obstacle within 20cm
            led = 1; // Turn on LED
        } else {
            led = 0; // Turn off LED
        }

        wait_us(100000); // Wait for 100ms before taking another measurement
    }
}
*/
