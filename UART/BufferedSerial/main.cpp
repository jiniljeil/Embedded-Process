#include "mbed.h"

#define MAXIMUM_BUFFER_SIZE 80 
char buf[MAXIMUM_BUFFER_SIZE] ;

static DigitalOut led(LED1); 

static BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;

int main()
{
    printf("mbed-os version: %d.%d.%d\r\n", 
        MBED_MAJOR_VERSION, MBED_MINOR_VERSION,MBED_PATCH_VERSION);
    
    sprintf(buf, "Hello New Serial function in mbed-os v.%.1f\r\n", 6.0);
    pc.write(buf, strlen(buf)); 
    
    sprintf(buf, "Enter characters...\r\n"); 
    pc.write(buf, strlen(buf)); 
    
    while (1) { 
        led = !led ; 
        
        // Echo the input bak to the terminal 
        int num = pc.read(buf, sizeof(buf)) ; 
        pc.write(buf,num) ; 
        if ( buf[0] =='\r') { 
            pc.write("\n", 1); 
        } 
    } 
} 
