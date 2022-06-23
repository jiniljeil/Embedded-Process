#include "mbed.h"

#define MAXIMUM_BUFFER_SIZE 80 
char buf[MAXIMUM_BUFFER_SIZE] ;

static DigitalOut led(LED1); 

// PA_2, PA_3
static BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;
bool flag = false; 
int num ; 
int len = 0; 
char c;  
// RX interrupt handler 
void rx_handler() {  
    led = !led ; // Toggle the LED. 
    
    if ( pc.readable() ) {
        flag = true ;  
    }
}

int main()
{
    pc.set_blocking(false); // non-critical in this example
    pc.sigio(rx_handler); // event handler 
    
    sprintf(buf, "Hello New Serial function in mbed-os v.%.1f\r\n", 6.0);
    pc.write(buf, strlen(buf)); 
    
    sprintf(buf, "Enter characters...\r\n"); 
    pc.write(buf, strlen(buf)); 
    
    while (1) {
        if ( flag ) { 
            if (  pc.read(&c, 1) ) {
                pc.write(&c, 1) ;
                    
                if ( c == '\r') { 
                    pc.write("\n", 1); 
                } 
                flag = false; 
            } 
        } 
    } 
} 
