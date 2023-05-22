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
//set threshold to 5m
#define threshold 500 

#define ARRAY_SIZE 2

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

void compare();

//=========================== SAMPLING INITIALISING =====================================

//structure to hold variables for measurement calculations
struct Sensor {
    float averages[ARRAY_SIZE];         // Array to store averages
    float differences[ARRAY_SIZE - 1]; //store moving differences
    int index;                        // Index of the current reading being taken
    float total;                     // Total of all readings taken so far
    float sum;                      //sum of readings taken 
    float average;                 // Average of all readings taken so far
    float average_difference;    // Average of differences
};

// Initialize two instances of the Sensor structure
Sensor hc; //hcsr04
Sensor vl; //vl53l0x

//intialise variables to store readings variables
float hc_reading, vl_reading;
int motor_speed;

int index = 0;

// Check if differences are constantly declining
bool isDeclining = true;

//============================ MEASURE DISTANCES FROM SENSORS ===============================
void takereadings() {

    hc.total = 0;
    vl.total = 0;
    hc.index = 0;
    vl.index = 0;

    //take ten readings then average them out 
    for (int i = 0; i < 5; i++)
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
    printf("HCSR04 %.2fcm \n", hc.average); //print ultrasonic readings every 10ms
    vl.average = vl.total / vl.index;
    printf("VL53L0X %.2fcm \n", vl.average); //print lidar

    //call the compare function to check if values are falling
    compare();

}

//using a thread and semaphore, call the semaphore in the ISR
//event queue and call the sampling every 1ms avoids the ISR

//==========================COMPARE AVERAGE FUNCTION ==========================================
// Compare function takes the averages stored in the array and decides whether readings are falling or rising
void compare() {

    //refresh
    hc.sum = 0;
    vl.sum = 0;
    motor_speed = 0; // Deactivate motor
    wait_us(5000);
    
    // Update window with new value
    hc.averages[index] = hc.average;
    //printf("array %.2fcm \n", hc.averages[]);
    vl.averages[index] = vl.average;
    //update index
    index = (index + 1) % ARRAY_SIZE;


    //check the difference between the averages
    hc.average_difference = hc.averages[index + 1] - hc.averages[index];
    vl.average_difference = vl.averages[index + 1] - vl.averages[index];

   
    //if either of the sensors values are increasing then decline is false
    if((hc.averages[index + 1] < threshold) || (vl.averages[index + 1]< threshold) &&    //if the averages are less than 5m   
        ((hc.average_difference < 100 )|| (vl.average_difference >100))){  //drop is larger than 1m
        //((hc.averages[index + 1] < hc.averages[index])|| (vl.averages[index + 1] < vl.averages[index]))){  //new average is lower 
        isDeclining = true;
        
    } else {
        isDeclining = false;
    }
     
    //return isDeclining;
}

void feedback(){   
// Check if our bool statement is met
if (isDeclining) {
    motor_speed = -0.1 * (hc.average_difference + vl.average_difference); // Activate motor
    motor_pwm.write(motor_speed);
    wait_us(1);
    motor_speed = 0;
    printf("feedback is sent\n");

} else {
    motor_speed = 0; // Deactivate motor
    motor_pwm.write(motor_speed);
    printf("feedback not needed\n");
    }
}

//Creates a queue with the default size
EventQueue mainQueue;
EventQueue workerQueue;
Thread t1; //main thread for sampling
Thread t2; //secondary thread for feedback 

int main() {

    //initialise lidar 
    vl53l0x.init();
    vl53l0x.setModeContinuous();
    vl53l0x.startContinuous();
    
    // Set the motor PWM frequency and initial duty cycle
    motor_pwm.period(0.1);
    motor_pwm.write(0.1);

    // Start the event queue for sampling
    t1.start(callback(&mainQueue, &EventQueue::dispatch_forever));
    // Attach functions to event queue
    ThisThread::sleep_for(20000s);
    mainQueue.call_every(100ms, takereadings);

    // Start the event queue for feedback
    t2.start(callback(&workerQueue, &EventQueue::dispatch_forever));
    // Attach functions to event queue
    ThisThread::sleep_for(20000s);
    mainQueue.call_every(1s, feedback);

    while (1) {
        //empty while looop
    }
    
}

//*/
//cout << "Distance HC-SR04: " << dist << " cm" << endl;


 /*
    // Calculate moving difference
    //this takes the difference of the new average from the old average
    for (int i = 0; i < (ARRAY_SIZE - 1); i++) {
        hc.differences[i] = hc.averages[i + 1] - hc.averages[i];
        vl.differences[i] = vl.averages[i + 1] - vl.averages[i];

        // add up the differences
        hc.sum += hc.differences[i];
        vl.sum += vl.differences[i];

    printf("array %.2fcm \n", hc.differences[i]);

    }
    //calculate average
    hc.average_difference = hc.sum / (ARRAY_SIZE - 1);

    vl.average_difference = vl.sum / (ARRAY_SIZE - 1);

    */
