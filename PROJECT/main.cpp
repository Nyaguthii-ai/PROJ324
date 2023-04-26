#include "PinNames.h"
#include "mbed.h"
#include "HCSR04.h"
#include "VL53L0X.h"
using namespace std;

#define CM 0 

//A5(PA6) = I2C1_SCL
//A4(PA5) = I2C1_SDA

//old nucleo
I2C i2c(D14,D15);
VL53L0X lidar(&i2c);
HCSR04 hcsr04(D3, D2); // Set HC-SR04 trigger and echo pins

//new nucleo
//VL53L0X lidar(D13,D12); // Set VL53L0X I2C pins sda, scl 

DigitalOut led(LED1);

int main() {

    lidar.init();
    lidar.setModeContinuous();
    lidar.startContinuous();

    while (1) {

        hcsr04.start();
        //wait_us(500000); 

        int dist = hcsr04.get_dist_cm(); // Measure distance with HC-SR04 in centimeters

        uint16_t distance_vl53l0x = lidar.getRangeMillimeters() / 10; // Measure distance with VL53L0X in centimeters

        //cout << "Distance HC-SR04: " << dist << " cm" << endl;
        printf("Distance HC-SRO4 %ucm \n",dist);

        printf("%4icm\n\r", distance_vl53l0x);


        if (dist < 20.0 || distance_vl53l0x < 20) { // If there is an obstacle within 20cm
            led = 1; // Turn on LED
        } else {
            led = 0; // Turn off LED
        }

        wait_us(100000); // Wait for 100ms before taking another measurement
    }
}
