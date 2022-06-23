#include "mbed.h"
#include "SRF05.h"

SRF05 srf05(ARDUINO_UNO_D3, ARDUINO_UNO_D2); 
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ; 

char buffer[80]; 
int _count = 0 ; 
Timeout _tout ; 
void timeout() { 
    if ( _count == 3 ) { 
        _tout.detach() ; 
    } else {
        if ( srf05.getStatus() ) {
            sprintf(buffer, "Distance = %.2f [cm]\r\n", srf05.getDistance()); 
            pc.write(buffer, strlen(buffer)); 
        } 
        _count++;   
    } 
}

int main()
{
    sprintf(buffer, "Mbed OS version %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION) ; 
    pc.write(buffer, strlen(buffer)) ; 
    
    sprintf(buffer, "Welcome to Utrasonic Sensor Lab. 9-4!\r\n");  
    pc.write(buffer, strlen(buffer)) ; 
    
    
    srf05.start(); 
    
    ThisThread::sleep_for(3s) ;
    
    if ( srf05.getStatus() ) { 
        sprintf(buffer, "Distance = %.2f [cm]\r\n", srf05.getDistance()); 
        pc.write(buffer, strlen(buffer)); 
    } 
    
    srf05.start(3) ; 
    
    for (int i = 0 ; i < 5 ; i++) { 
        ThisThread::sleep_for(4s);
        
        if ( srf05.getStatus() ) {
            sprintf(buffer, "Distance = %.2f [cm]\r\n", srf05.getDistance()); 
            pc.write(buffer, strlen(buffer)); 
        } 
        
        if ( srf05.getPulseDuration() ){ 
            sprintf(buffer, "PulseDuration = %d [us]\r\n", srf05.getPulseDuration()); 
            pc.write(buffer, strlen(buffer)); 
        } 
    } 
    
    srf05.stop(); 
    return 0 ;
}