#include "mbed.h"

DigitalOut trigger(ARDUINO_UNO_D3) ; 
DigitalIn echo(ARDUINO_UNO_D2);  
Timer timer1 ; 

BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ; 
char buffer[80]; 

int main()
{
    float distance ; 
    
    sprintf(buffer, "Mbed OS version %d.%d.%d\r\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION) ; 
    pc.write(buffer, strlen(buffer)) ; 
    
    sprintf(buffer, "\r\nWelcome to Utrasonic Sensor Lab!\r\n");  
    pc.write(buffer, strlen(buffer)) ; 
    
    // initialization 
    trigger = 0 ;  
    
    while(1) { 
        // trigger sonar to send a ping 
        trigger = 1 ; 
        wait_us(20) ; 
        trigger =  0 ; 
        
        timer1.reset() ; 
        while (echo == 0) { } // wait for echo high 
        timer1.start() ;      // when echo high, start time 
        while (echo == 1) { } // wait for echo low 
        timer1.stop() ; 
        
        // subtract software overhead time delay and scale to cm 
        distance = timer1.elapsed_time().count() * 0.017 ; 
        sprintf(buffer, "The distance is %.2f[cm]\r\n", distance) ; 
        pc.write(buffer, strlen(buffer)); 
        
        ThisThread::sleep_for(5s) ; // 5 sec 
    }  
    
    return 0 ;
}
