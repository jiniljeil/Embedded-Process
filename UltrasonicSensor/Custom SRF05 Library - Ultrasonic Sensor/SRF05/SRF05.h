#ifndef MBED_ULTRASONIC_H
#define MBED_ULTRASONIC_H
#include "mbed.h" 

class SRF05
{ 
    public:
        /**initiates the class with the specified trigger pin, echo pin 
        */
        SRF05(PinName trigPin, PinName echoPin) ; 
        
        ~SRF05() ; 
        
        // start measure measurement 
        void start(float period) ; 
        void start(void) ; 
        
        void stop(void) ; // stop measuring
        
        // return distance in cm and clear the done status 
        float getDistance(void) ; 
        
        // return the echo pulse duration in us 
        int getPulseDuration(void) ; 
        
        // get a status whether measurement is done or not 
        int getStatus(void) ; 
    
    private: 
        DigitalOut _trig ; 
        InterruptIn _echo ; 
        
        Timer _timer ; 
        Ticker _ticker ; 
        
        float _period ;         // timeout value in sec, or retrigger time in sec if repeat == true 
        bool _repeat ;          // if true, measure repeatedly with the time interval timeout 
        
        float _distance ; 
        int _pulseDuration ; 
            
        void _startT(void) ;    // ISR for the ECHO risigin edge 
        void _endT(void) ;      // ISR for the ECHO falling edge 
        void _ticker_cb(void) ; // ISR for ticker 
        int _done ;             // end of measure 
}; 
#endif