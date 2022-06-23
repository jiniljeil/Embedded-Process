#include "mbed.h"
#define PWM_MIN 550 
#define PWM_MAX 2150

PwmOut servoMotor(D9); // PB_10, D6 
UnbufferedSerial pc(PA_2, PA_3, 115200) ; 

char rx_buffer[10] ; 
char tx_buffer[80] ; 
int index = 0 ; 
volatile int flag ; 

void rx_ISR(void) 
{ 
    char ch ; 
    pc.read(&ch, 1) ; 
    pc.write(&ch, 1) ;
    
    rx_buffer[index++] = ch ; 
    if ( ch == '\r' ) { 
        pc.write("\n", 1) ; 
        rx_buffer[--index] = '\0'; // change CR to 0 
        index = 0 ; 
        flag = 1;  
    } 
} 

void control_servo() { 
    int num ; 
    int on ; 
    
    num = atoi((const char *) rx_buffer) ; 
    on = PWM_MIN + (PWM_MAX - PWM_MIN) * num / 180 ; 
    sprintf(tx_buffer, "Pulse ON time = %d (%d)\r\n", on, num); 
    pc.write(tx_buffer, strlen(tx_buffer)) ; 
    if ( on < PWM_MIN ) on = PWM_MIN ; 
    if ( on > PWM_MAX ) on = PWM_MAX ; 
    servoMotor.pulsewidth_us(on); 
} 
int main()
{
    sprintf(tx_buffer, "PWM Test Program!\r\n"); 
    pc.write(tx_buffer, strlen(tx_buffer)) ; 
    pc.attach(rx_ISR) ;
    
    servoMotor.period_ms(20); // PWM period = 20ms 
    servoMotor.pulsewidth_us(PWM_MIN) ; // for 0 degree 
    
    while(1) {
        flag = 0; 
        sprintf(tx_buffer, "Enter the rotation degree [0 - 180]: ") ; 
        pc.write(tx_buffer, strlen(tx_buffer)) ; 
        while( flag != 1 ) ; 
        
        control_servo();   
    } 
    
}
