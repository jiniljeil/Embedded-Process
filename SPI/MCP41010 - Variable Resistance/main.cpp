#include "mbed.h"

SPI spi(D11, D12, D13) ; 
DigitalOut cs(PA_4);

UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ; 
AnalogIn ain0(A0) ; 

char buf[64] ; 
volatile bool flag = false ; 
int len = 0 ; 

void rx_handler() { 
    char c ; 
    if ( pc.read(&c, 1)) {
        pc.write(&c, 1) ;
        
        if ( c == '\r' ) { 
            flag = true; 
            pc.write("\n", 1); 
            buf[len] = '\0'; 
            len = 0 ; 
        } else { 
            buf[len++] = c;  
        } 
    }   
} 

void MCP41010_Control_Read_ADC() { 
    int val = atoi((char*)buf); 
    
    if ( val < 0 || val > 255) {
        sprintf(buf, "You should enter the number range of [0 ~ 255]\r\n"); 
        pc.write(buf, strlen(buf)); 
        return ; 
    } 
    
    sprintf(buf, "Potentiometer = %d (%.0f [mV])\r\n", val, (3300 * val) / 255.0 ) ; 
    pc.write(buf, strlen(buf)) ;
    
    // 0x11 = (C1, C0 = '01', P0 = 1)
    char tx_buf[2] = { 0x11, (char)val };
     
    cs = 0 ; 
    spi.write(tx_buf, 2, 0x0, 0) ;
    cs = 1 ; 
    
    ThisThread::sleep_for(500ms); 
         
    sprintf(buf, "ADC value = %.0f [mV]\r\n", ain0.read() * 3300) ; 
    pc.write(buf, strlen(buf)); 
} 
int main()
{
    pc.baud(115200); 
    cs = 1 ; // /CS 
    spi.format(8, 3); // CPOL = 1, CPHA = 1 
    spi.frequency(1000000); // SPI speed: 1Mbps
    
    sprintf(buf, "\r\nTest SPI Program\r\n") ; 
    pc.write(buf, strlen(buf)); 
    pc.attach(rx_handler) ; 
    
    while(1) { 
        flag = false; 
        sprintf(buf, "Enter the value for the potentiometer [0, 255]: "); 
        pc.write(buf, strlen(buf)); 
        
        // User input 
        while (!flag) { } 
        
        MCP41010_Control_Read_ADC() ; 
    } 
    
}
