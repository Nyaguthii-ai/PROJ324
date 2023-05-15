/**
 * @file main.cpp
 * @author Ivy Nyaguthii (ivyjaynew@gmail.com)
 * @date first edit: 2023-03-10
 * @date latest edit: 2023-05-15
 * 
 * @copyright all rights reserved to the author 
 * 
 */

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

//structure to hold variables for measurement calculations
struct Sensor {
    float averages[10];     // Array to store averages
    int index;            // Index of the current reading being taken
    float total;            // Total of all readings taken so far
    float average;          // Average of all readings taken so far
    float last_average;     // Average of previous 10 readings
    float motor_speed;      // Speed of motor vibrations
};

// Initialize two instances of the Sensor structure
Sensor hc; //hcsr04
Sensor vl; //vl53l0x

//intialise variables to store readings variables
float hc_reading, vl_reading;
int motor_speed;

//============================ MEASURE DISTANCES FROM SENSORS ===============================
void takereadings() {

    hc.total = 0;
    vl.total = 0;
    hc.index = 0;
    vl.index = 0;

    //take ten readings then average them out 
    for (int i = 0; i < 10; i++)
    {

        //initialise ultrasonic
        hcsr04.start();
        wait_us(5000);
    
        //measure distance from HCSRO4
        hc_reading = hcsr04.get_dist_cm(); 
        //measure distance from VL53L0X in cm
        vl_reading = (vl53l0x.getRangeMillimeters())/10;

        hc.total += hc_reading; //sum up the ultrasonic values 
        vl.total += vl_reading; //sum up lidar values

        //update the indexes after counts
        hc.index ++;
        vl.index ++;
    }

    //take the averages
    hc.average = hc.total / hc.index;
    printf("HCSR04 %.3fcm \n", hc.average); //print ultrasonic readings every 10ms
    vl.average = vl.total / vl.index;
    printf("VL53L0X %.3fcm \n", vl.average); //print lidar

    //store avergaes in the array
   // for (int i = 0; i < 10; i++) {
    //   hc.average += hc.averages[i];
    //   vl.average += vl.averages[i];
   // }
}

//using a thread and semaphore, call the semaphore in the ISR
//event queue and call the sampling every 1ms avoids the ISR

//==========================COMPARE AVERAGE FUNCTION ==========================================
// Compare function takes the averages stored in the array and decides whether readings are falling or rising
void compare() {

    // If the distance readings have been constantly falling, vibrate the motor
    /*
    if (a.average < a.last_average) {
        a.motor_speed = 100 * (a.last_average - a.average);
    } else {
        a.motor_speed = 0;
    }
    */
    // Calculate average distance for sensor b
     
}
//Creates a queue with the default size
EventQueue mainQueue;
EventQueue workerQueue;
Thread t1;
Thread t2;

int main() {

    //initialise lidar 
    vl53l0x.init();
    vl53l0x.setModeContinuous();
    vl53l0x.startContinuous();
    
    // Set the initial motor speed
    float speed = 0;

    // Start the event queue
    t1.start(callback(&mainQueue, &EventQueue::dispatch_forever));
    // Attach functions to event queue
    mainQueue.call_every(100ms, takereadings);
    workerQueue.call_in(10ms, compare);

    
    while (1) {
        //hc.index = 0;
        //takereadings();
        //printf("working \n"); //print ultrasonic readings every 10ms
        //printf("VL53L0X %ucm \n", hc.average); //print lidar
    }
    
}

//*/
//cout << "Distance HC-SR04: " << dist << " cm" << endl;


