#include "mbed.h"
#include "SRF05.h"
SRF05 srf05(ARDUINO_UNO_D3, ARDUINO_UNO_D2); 
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ; 

char buffer[80]; 

int main()
{
    sprintf(buffer, "Mbed OS version %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION) ; 
    pc.write(buffer, strlen(buffer)) ; 
    
    sprintf(buffer, "\r\nWelcome to Utrasonic Sensor Lab. 9-2!\r\n");  
    pc.write(buffer, strlen(buffer)) ; 
    
    while(1) { 
        sprintf(buffer, "Distance = %.2f [cm]\r\n", srf05.read()); 
        pc.write(buffer, strlen(buffer)); 
        ThisThread::sleep_for(3000ms) ; // 3 sec 
    }  
    
    return 0 ;
}
