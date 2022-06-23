#include "SRF05.h"
#include "mbed.h"

SRF05::SRF05(PinName trigger, PinName echo) 
    : _trig(trigger), _echo(echo) {   
    
    _echo.rise(callback(this, &SRF05::_startT));
    _echo.fall(callback(this, &SRF05::_endT));
} 

SRF05::~SRF05() { 
    _timer.stop(); 
    _timer.reset(); 
} 

// ISR for the ECHO rising edge 
void SRF05::_startT(void) {
    _timer.reset();
    while( _echo == 0 ) { } 
    _timer.start();
}  
// ISR for the ECHO falling edge 
void SRF05::_endT(void) {
    while( _echo == 1 ) { } 
    _timer.stop();
    
    int _time = _timer.read_us() ;
    _pulseDuration = _pulseDuration + _time ; 
    _distance = _time/58.0 ; // timer1.elapsed_time().count() * 0.017 
    
    _done = 1 ; 
}
// ISR for ticker 
void SRF05::_ticker_cb(void){ 
    // send a trigger pulse, 20uS long
    if ( _repeat == true ) { 
        _done = 0; 
        
        _trig = 1;
        wait_us(20) ; 
        _trig = 0;
    } 
}

// start measure measurement 
void SRF05::start(float period) { 
    _repeat = true ; 
    _period = period ;  
    _ticker.attach(callback(this, &SRF05::_ticker_cb), _period);  
} 
void SRF05::start(void) {
    _trig = 1 ;
    wait_us(20) ; 
    _trig = 0 ;
} 
        
void SRF05::stop(void) { // stop measuring
    if ( _repeat == true ) _repeat = false; 
} 
        
// return distance in cm and clear the done status 
float SRF05::getDistance(void) { 
    if ( _done == 1 ) {
        _done = 0; 
        return (_distance);
    }
    
    return -1; 
} 

// return the echo pulse duration in us 
int SRF05::getPulseDuration(void) {
    
    if ( _done == 1 ) {
        _done = 0; 
        return _pulseDuration ; 
    }
    
    return -1 ; 
}
        
// get a status whether measurement is done or not 
int SRF05::getStatus(void) { 
    return _done; 
} 



