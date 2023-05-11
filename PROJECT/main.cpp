/**
 * @file main.cpp
 * @author Ivy Nyaguthii (ivyjaynew@gmail.com)
 * @date 2023-05-10
 * 
 * @copyright Copyright (c) 2023, all rights reserved
 * 
 */
#include "PinNames.h"
#include "mbed.h"
#include "HCSR04.h"
#include "VL53L0X.h"
#include <cstdio>
using namespace std;

#define CM 0 

//NUCLEO F411RE

//=========================== PIN CONNECTIONS ========================================
// LIDAR
I2C i2c(D14,D15);   //i2c pins: D15 SCL and D14 SDA
VL53L0X vl53l0x(&i2c);

//ULTRASONIC
HCSR04 hcsr04(D3, D2); // Set HC-SR04 trigger and echo pins

//MOTOR
PwmOut motor_pwm(D9);

int main() {

    // Set the initial motor speed
    float speed = 0;

    // Set the motor PWM frequency and initial duty cycle
    motor_pwm.period(0.01);
    motor_pwm.write(0.1);

    //========================== READ SENSORS ============================
    // Initiate Ultrasonic
    //hcsr04.start();

    // Initiate Lidar and start continuous reading
    vl53l0x.init();
    vl53l0x.setModeContinuous();
    vl53l0x.startContinuous();


    while (1) {
        // Initiate Ultrasonic
        hcsr04.start();
        wait_us(500000); 
        //record ultrasonic
        int dist_hc = hcsr04.get_dist_cm(); // Measure distance with HC-SR04 in centimeters

        printf("HC-SRO4 %ucm \n", dist_hc);

        //record lidar
        uint16_t dist_vl= vl53l0x.getRangeMillimeters()/ 10; // Measure distance with VL53L0X in centimeters

        printf("VL53L0X %ucm \n", dist_vl);
        //cout << "Distance HC-SR04: " << dist << " cm" << endl;

        // Increase the motor speed gradually
        speed += 0.025;
        
        // Reset the speed to the minimum value when it reaches the maximum
        if (speed > 1.0) {
        speed = 0.1;
        }
        
        // Set the motor duty cycle to the current speed
    motor_pwm.write(speed);

    }

}   
 



#include "PinNames.h"
#include "mbed.h"
#include "HCSR04.h"
#include "VL53L0X.h"
#include <cstdio>
using namespace std;

#define CM 0 

#define NUM_SENSORS 2  // Number of sensors to use

//NUCLEO F411RE

//=========================== PIN CONNECTIONS ========================================
// LIDAR
I2C i2c(D14,D15);   //i2c pins: D15 SCL and D14 SDA
VL53L0X vl53l0x(&i2c);

//ULTRASONIC
HCSR04 hcsr04(D3, D2); // Set HC-SR04 trigger and echo pins

//MOTOR
PwmOut motor_pwm(D9);

//NUCLEO L432KC
//VL53L0X lidar(D13,D12); // Set VL53L0X I2C pins sda, scl 

DigitalOut led(LED1);

//=========================== SAMPLING INITIALISING =====================================
// Timer to call timer_interrupt every millisecond
Timer timer;             // Timer for measuring pulse duration
Ticker ticker;

//structure to hold variables for measurement calculations
struct Sensor {
    int readings[10];     // Array to store distance readings
    int index;            // Index of the current reading being taken
    int total;            // Total of all readings taken so far
    int average;          // Average of all readings taken so far
    int last_average;     // Average of previous 10 readings
    int motor_speed;      // Speed of motor vibrations
};

// Initialize two instances of the Sensor structure
Sensor a; //hcsr04
Sensor b; //vl53l0x

//============================ MEASURE DISTANCES FROM SENSORS ===============================

// Function to measure distance from HCSRO4
int measure_distance_a() {
    hcsr04.get_dist_cm(); 
    return 0;
}

// Function to measure distance from VL53L0X
int measure_distance_b() {
    vl53l0x.getRangeMillimeters();
    return 0;
}

int distance_a;
int distance_b;
int motor_speed;
//============================= TIMER FUNCTION ==============================================
// Interrupt function to measure distance and calculate average distance without using the wait() function
void timer_interrupt() {

    // Add reading to total and update index for sensor a
    a.total -= a.readings[a.index];
    a.readings[a.index] = distance_a;
    a.total += distance_a;
    a.index = (a.index + 1)% 10;

    //
    // Add reading to total and update index for sensor b
    b.total -= b.readings[b.index];
    b.readings[b.index] = distance_b;
    b.total += distance_b;
    b.index = (b.index + 1)% 10;

    // Calculate average every 10 milliseconds
    if (timer.elapsed_time().count() >= 10) {
        // Calculate average distance for ultrasonic
        a.average = a.total / 10;

        printf("HCSR04 %ucm \n", a.average);
        // If the distance readings have been constantly falling, vibrate the motor
        if (a.average < a.last_average) {
            a.motor_speed = 100 * (a.last_average - a.average);
        } else {
            a.motor_speed = 0;
        }

        // Calculate average distance for sensor b
        b.average = b.total / 10;
        printf("VL53L0X %ucm \n", b.average);
        
        // If the distance readings have been constantly falling, vibrate the motor
        if ((b.average < b.last_average) && (a.average < a.last_average)){
            motor_speed = 100 * ((b.last_average - b.average) + (a.last_average - a.average));
        } else {
            b.motor_speed = 0;
        }

        // Store last average for ultrasonic
        a.last_average = a.average;

        // Store last average for lidar
        b.last_average = b.average;
        
        // Reset timer
        timer.reset();
    }
}

int main() {

    //initialise ultrasonic
    hcsr04.start();
    
    //initialise lidar 
    vl53l0x.init();
    vl53l0x.setModeContinuous();
    vl53l0x.startContinuous();
    
    // Set the initial motor speed
    float speed = 0;

    while (1) {
    distance_a = hcsr04.get_dist_cm();
    //printf("Distance HC-SRO4 %ucm \n", a.average);
    //cout << "Distance HC-SR04: " << dist << " cm" << endl;
    //printf("Distance vl53l0x %4icm\n\r", b.average);

    distance_b = vl53l0x.getRangeMillimeters();

    // Start timer and ticker
    timer.start();
    ticker.attach(&timer_interrupt, 1ms); // Set up timer interrupt to trigger every millisecond

    }
}

*/



