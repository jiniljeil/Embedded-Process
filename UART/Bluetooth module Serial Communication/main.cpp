#include "mbed.h"
#include <string.h> 
#define MAXIMUM_BUFFER_SIZE 80 
char buf[MAXIMUM_BUFFER_SIZE] ;

// PB3(D3) , PB5(D4) 
// 0001 0100 = 0x20 
PortOut RGLeds(PortB, 0x28); 

// TX - RX 
static UnbufferedSerial bt(PA_9, PA_10); // TX, RX
static UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX) ;

int cmd_len = 0; 
bool red_led, green_led ; // keyword check
bool red_led_on, green_led_on ;  // on check 
bool none ; 
void bt_handler() { 
    char c; 
    
    if ( bt.readable()) { 
        if ( bt.read(&c, 1) ) { 
            
            if ( c == '\r' ) {  
                buf[cmd_len] = '\0'; 
                
                // Input 
                pc.write(buf, cmd_len) ; 
                pc.write("\n\r", 2) ; 
                
                // Tokenization 
                char *ptr = strtok(buf, " ");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환

                while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
                {   
                    if ( red_led ) { 
                        
                        // RedLED on 
                        if ( !strcmp("on", ptr)) { 
                            pc.write("RedLED status: on\r\n", 19); 
                            bt.write("RedLED status: on\r\n", 19);
                            red_led_on = true; 
                            
                            if ( green_led_on ) { 
                                RGLeds = 5 << 3 ;
                            } else {
                                RGLeds = 1 << 3 ;  
                            } 
                        // RedLED off
                        } else if ( !strcmp("off", ptr)) { 
                            red_led_on = false; 
                            pc.write("RedLED status: off\r\n", 20);
                            bt.write("RedLED status: off\r\n", 20);
                            
                            if ( green_led_on ) { 
                                RGLeds = 4 << 3 ;  
                            } else { 
                                RGLeds = 0 ; 
                            } 
                        // RedLED status 
                        } else if ( !strcmp("status", ptr)) { 
                            // OFF, ON 
                            if ( red_led_on == false ) { 
                                pc.write("RedLED status: off\r\n", 20);
                                bt.write("RedLED status: off\r\n", 20);
                            } else { 
                                pc.write("RedLED status: on\r\n", 19);
                                bt.write("RedLED status: on\r\n", 19);
                            }  
                        } else { 
                            none = true; 
                            break; 
                        } 
                    } else if ( green_led ) { 
                        
                        // GreenLED on
                        if ( !strcmp("on", ptr)) { 
                            pc.write("GreenLED status: on\r\n", 21) ; 
                            bt.write("GreenLED status: on\r\n", 21) ; 
                            green_led_on = true; 
                            
                            if ( red_led_on ) { 
                                RGLeds = 5 << 3 ;  
                            } else {
                                RGLeds = 4 << 3 ; 
                            } 
                        // GreenLED off
                        } else if ( !strcmp("off", ptr)) { 
                            pc.write("GreenLED status: off\r\n", 22) ;
                            bt.write("GreenLED status: off\r\n", 22) ;
                            green_led_on = false; 
                            
                            if ( red_led_on ) {
                                RGLeds = 1 << 3 ; 
                            } else { 
                                RGLeds = 0 ; 
                            } 
                        // GreenLED status
                        } else if ( !strcmp("status", ptr)) { 
                            if ( green_led_on == false ) { 
                                pc.write("GreenLED status: off\r\n", 22) ;
                                bt.write("GreenLED status: off\r\n", 22) ;
                            } else { 
                                pc.write("GreenLED status: on\r\n", 21) ;
                                bt.write("GreenLED status: on\r\n", 21) ;
                            } 
                        } else { 
                            none = true; 
                            break; 
                        } 
                    } else {
                        if ( !strcmp("RedLED", ptr) ) {  
                            red_led = true; 
                        } 
                        else if ( !strcmp("GreenLED", ptr)) { 
                            green_led = true; 
                        } 
                        else {
                            none = true; 
                            break; 
                        } 
                    } 
                    ptr = strtok(NULL, " ");      // 다음 문자열을 잘라서 포인터를 반환
                } // while 
                
                if ( none ) { 
                    pc.write("Undefined command\r\n", 19) ;
                    bt.write("Undefined command\r\n", 19) ;  
                    none = false; // reset 
                }
                red_led = green_led = false ; // reset 
                
                cmd_len = 0 ; 
                memset(buf, '\0', sizeof(buf)); 
            } else if ( (c >= 'a' && c <= 'z') || ( c >= 'A' && c <= 'Z') || (c == ' ')) { 
                buf[cmd_len++] = c ; 
            } 
        } 
    } 
} 

int main()
{
    sprintf(buf, "Bluetooth Start!\r\n") ; 
    pc.write(buf, strlen(buf)); 
    
    bt.baud(9600) ; 
    bt.format(8, SerialBase::None, 1) ;
    
    pc.baud(9600) ; 
    pc.format(8, SerialBase::None, 1) ; 

    bt.attach(bt_handler, SerialBase::RxIrq); 
    
    while (1) { 
    } 
} 
