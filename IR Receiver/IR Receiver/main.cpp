#include "mbed.h"
#define NEC_T 562.5

#define IR_9000us_UL 9500
#define IR_9000us_LL 8500 
#define IR_4500us_UL 5000
#define IR_4500us_LL 4000
#define IR_2250us_UL 2500 
#define IR_2250us_LL 2000
#define IR_1_UL_us   2000
#define IR_1_LL_us   1100
#define IR_0_UL_us   1000
#define IR_0_LL_us   400
#define IR_repeat_timeout_ms 120

typedef enum {
    Idle,
    Receiving,
    Received
} State;

State state ;
// char buf[64]; 
uint8_t buf[64]; 
char hex_data[9];
char pc_buf[128]; 
Timer _timer ; 
Timeout _timeout ; 
Ticker _ticker ; 
InterruptIn ir(D4) ; 
int bitcount ; 

int repeat_count ; 
int repeat_timeout_count; 
bool repeat_timeout_flag; 

bool final ; 
int cnt = 0 ;
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ; 
int data; 
int k ; 
char hex[17] = "0123456789ABCDEF"; 


int t1, t2, t3, d1, d2 ; 
int f1, f2 ; 
void init_state() { 
    final = false; 
    t1 = t2 = t3 = d1 = d2 = -1 ; 
    repeat_count = 0 ; 
    state = Idle ; 
    _timer.stop();
    _timer.reset();
    repeat_timeout_count = 0 ;

    for (int i = 0 ; i < sizeof(buf)/sizeof(char) ; i++) { 
        buf[i] = 0;
    }   
}

void isr_timeout() { 
    
    if (state == Receiving) {
        sprintf(pc_buf, "[Frame format error]: data receiving time: %d\r\n", d2 - d1); 
        pc.write(pc_buf, strlen(pc_buf)); 
        state = Idle;
        t1 = t2 = t3 = d1 = d2 = -1; 
    }
} 

void isr_falling() { 
    
    if ( state == Idle ) { 
        
        if ( t1 < 0 ) { 
            _timer.start(); 
            t1 = (_timer.elapsed_time()).count(); // _timer.read_us(); 
        } else { 
            t3 = (_timer.elapsed_time()).count();
            
            int pulse_burst = t2 - t1 ; 
            int space = t3 - t2 ; 
                
            if ( (pulse_burst >= IR_9000us_LL && pulse_burst <= IR_9000us_UL) 
                    && (space >= IR_4500us_LL && space <= IR_4500us_UL)  ) { 
    
                state = Receiving ; 
                bitcount = 0 ;
                repeat_count = 0 ; 
            } else if ( (pulse_burst >= IR_9000us_LL && pulse_burst <= IR_9000us_UL) 
                    && (space >= IR_2250us_LL && space <= IR_2250us_UL)) { 
                if( repeat_timeout_count > IR_repeat_timeout_ms ) { 
                    repeat_timeout_flag = true; 
                } else { 
                    repeat_timeout_count = 0 ; 
                    repeat_count++; 
                } 
                state = Received ; 
                bitcount = 0 ; 
                t1 = t2 = t3 = d1 = d2 = -1 ;
                
            } else { 
                init_state() ; 
            } 
        } 
    } else if ( state == Receiving ) { 
        d2 = (_timer.elapsed_time()).count();
        
        int data_time = d2 - d1 ; 
        
        // NEC_T * 3 
        if ( data_time >= IR_1_LL_us && data_time <= IR_1_UL_us ) { 
            buf[bitcount / 8] |= (1 << (bitcount % 8));
        }
        // NEC_T * 1
        else if ( data_time >= IR_0_LL_us && data_time <= IR_0_UL_us) { 
            buf[bitcount / 8] &= ~(1 << (bitcount % 8));
        } 
        bitcount++; 

        if ( bitcount >= 32 ) { 
            state = Received ; 
            t1 = t2 = t3 = d1 = d2 = -1; 
            f1 = (_timer.elapsed_time()).count(); ; 
        } 
        
        // Timeout check
        _timeout.detach(); 
        _timeout.attach(&isr_timeout, 2800us) ; // NEC_T * 5 = 2812.5us
    }   
} 

void isr_rising() { 
    
    if ( state == Idle ) { 
        // if falling isr was occured
        if ( t1 >= 0 ) { 
            t2 = (_timer.elapsed_time()).count();
        } else { 
            init_state() ; 
        } 
    } else if ( state == Receiving ) { 
        d1 = (_timer.elapsed_time()).count();
    } else if ( state == Received ) { 
        f2 = (_timer.elapsed_time()).count();
        int final_time = f2 - f1; 
        if ( final_time >= 300 && final_time <= 700) { 
            final = true; 
        } 
    } 
} 


void isr_repeat_timeout() { 
    repeat_timeout_count++; 
}
int main()
{
    sprintf(pc_buf, "Mbed OS version %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION) ; 
    pc.write(pc_buf, strlen(pc_buf)) ; 
    
    sprintf(pc_buf, "Welcome to Utrasonic Sensor Lab. 10-1!\r\n");  
    pc.write(pc_buf, strlen(pc_buf)) ; 
    
    init_state();
    ir.fall(&isr_falling);
    ir.rise(&isr_rising);
    ir.mode(PullUp);

    _ticker.attach(&isr_repeat_timeout, 1ms); 
    while(1) {
        
        if ( state == Received ) { 
            pc.write("Received\r\n",10); 
            if ( repeat_timeout_flag ) { 
                repeat_timeout_flag = false; 
                sprintf(pc_buf, "[Error] Repeatition code error\r\n") ; 
                pc.write(pc_buf, strlen(pc_buf)) ;
            } 
            else if ( repeat_count > 0 ) { 
                sprintf(pc_buf, "Repetition Code\r\n"); 
                pc.write(pc_buf, strlen(pc_buf)); 
            } else { 
                if ( bitcount >= 32 ) { 
                    sprintf(pc_buf, "[HexData]: 0x%02x%02x%02x%02x\r\n", buf[0], buf[1], buf[2], buf[3]) ; 
                    pc.write(pc_buf, strlen(pc_buf)) ;
                } 
            } 
            init_state(); 
        }   
    } 
    return 0; 
}