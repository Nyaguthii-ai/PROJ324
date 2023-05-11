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

//MOTOR
PwmOut motor_pwm(D9);

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


    // Set the motor PWM frequency and initial duty cycle
    motor_pwm.period(0.01);
    motor_pwm.write(0.1);

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

    // Set the initial motor speed
    float speed = 0;

    // Set the motor PWM frequency and initial duty cycle
    motor_pwm.period(0.01);
    motor_pwm.write(0.1);

    while (1) {

        hcsr04.start();
        wait_us(500000); 

        int dist = hcsr04.get_dist_cm(); // Measure distance with HC-SR04 in centimeters

        uint16_t distance_vl53l0x = lidar.readRangeContinuousMillimeters() / 10; // Measure distance with VL53L0X in centimeters

        //cout << "Distance HC-SR04: " << dist << " cm" << endl;
        //printf("Distance HC-SRO4 %u \n",dist);

        //printf("Distance VL53L0X %u \n",distance_vl53l0x);
 
// Increase the motor speed gradually
    //speed += 0.1;
        
    // Reset the speed to the minimum value when it reaches the maximum
    if (speed > 1.0) {
        speed = 0.1;
    }
        
    // Set the motor duty cycle to the current speed
    motor_pwm.write(speed);

        wait_us(100000); // Wait for 100ms before taking another measurement
    }
}
*/

