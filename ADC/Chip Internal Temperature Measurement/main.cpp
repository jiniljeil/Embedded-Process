#include "mbed.h"

BufferedSerial pc(USBTX, USBRX, 115200) ;
AnalogIn tempSensor(ADC_TEMP); 
char buf[80]; 

int main()
{
    float meas ; 
    sprintf(buf, "\r\n*** ADC Example (Internal Temp Sensor) ***\r\n"); 
    pc.write(buf, strlen(buf)); 
    
    while (true) {
        meas = tempSensor.read() ; 
        meas = (meas * 3300 - 760)/2.5 + 25; 
        sprintf(buf, "Internal Temperature = %.2f [C]\r\n", meas) ; 
        pc.write(buf, strlen(buf)); 
        
        ThisThread::sleep_for(chrono::seconds(5));
    }
}
