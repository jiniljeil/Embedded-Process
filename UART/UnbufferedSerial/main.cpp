#include "mbed.h"

#define MAXIMUM_BUFFER_SIZE 80 
char buf[MAXIMUM_BUFFER_SIZE] ;

static DigitalOut led(LED1); 

static UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX) ;

void rx_handler()
{
    char c ;
    
    // Toggle the LED 
    led = !led ; 
    
    if (pc.read(&c, 1)) { 
        pc.write(&c, 1) ; 
        if ( c == '\r' ) { 
            pc.write("\n", 1); 
        }
    } 
} 

int main()
{
    pc.baud(115200); 
    pc.format(8, SerialBase::None, 1) ;
    
    // Register a callback to prcess a Rx (receive ) interrupt
    bt.attach(bt_handler, SerialBase::RxIrq); 
    pc.attach(rx_handler, SerialBase::RxIrq); 
    
    
    sprintf(buf, "Hello New Serial function in mbed-os v.%.1f\r\n", 6.0);
    pc.write(buf, strlen(buf)); 
    
    sprintf(buf, "Enter characters...\r\n"); 
    pc.write(buf, strlen(buf)); 
    
    while (1) { 
    } 
} 
