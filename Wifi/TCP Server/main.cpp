#include "mbed.h"

#define DEFAULT_BAUD_RATE 115200 
#define SERVER_IP "192.168.4.1"
#define PORT 50001
//#define WIFI 
//#define SocketServer
WiFiInterface *wifi ; 
ATCmdParser *_parser ; 
BufferedSerial *_serial ; 
Thread sock_thread ; 

BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, DEFAULT_BAUD_RATE) ; 
TCPSocket server ;
TCPSocket* clientSocket;
char buf[64]; 

char ip_buf[32]; 
char gateway_buf[32]; 

char rxBuf_pc[80] ; 
char txBuf_pc[80] ;

int main()
{
    SocketAddress ip ; 
    SocketAddress gateway ;
     
    SocketAddress serverAddr(SERVER_IP, PORT); 
    
    sprintf(buf, "\r\nWiFi TCP Server example\r\n") ; 
    pc.write(buf, strlen(buf)) ; 

    /*
        Set SoftAP + Station mode 
    */

    _serial = new BufferedSerial(D8, D2, DEFAULT_BAUD_RATE) ; 
    _parser = new ATCmdParser(_serial, "\r\n"); 

    
    _parser->send("AT+RST"); 
    if (_parser->recv("OK")) { } 
    
    _parser->send("AT+CWQAP"); 
    if (_parser->recv("OK")) { } 

    _parser->send("AT+CWMODE=3");

    if (_parser->recv("OK")) {
        sprintf(txBuf_pc, "\nSet SoftAP + Station mode\r\n\r\n");
        pc.write(txBuf_pc, strlen(txBuf_pc)) ;
    } else {
        sprintf(txBuf_pc, "\Failed SoftAP + Station mode \r\n\r\n");
        pc.write(txBuf_pc, strlen(txBuf_pc)); 
        return -1;
    }
    
    /*
        Sucessfully configure for softAP 
        IP address: ~ 
        GW IP address: ~ 
    */
    
    _parser->send("AT+CWSAP=\"esp_jinil\",\"test1234\",1,3"); 
    
    if (_parser->recv("OK") ) { 
        sprintf(txBuf_pc, "Successfully configure for softAP\r\n"); 
        pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
    } 
    
    _parser->send("AT+CWJAP=\"esp_jinil\",\"test1234\"") ; 
    while(true) { 
        if ( _parser->recv("+DIST"))  { 
            ThisThread::sleep_for(20s); 
            break; 
        }     
    } 
    
    _parser->send("AT+CIPAP?"); 
    
    if ( _parser->recv("+CIPAP:ip:%s\r\n", ip_buf)) { 
        sprintf(txBuf_pc, "IP address: %s\r\n", ip_buf) ; 
        pc.write(txBuf_pc, strlen(txBuf_pc)); 
    } 
    
    if ( _parser->recv("+CIPAP:gateway:%s\r\n", gateway_buf)) {
        sprintf(txBuf_pc, "Gateway: %s\r\n", gateway_buf) ; 
        pc.write(txBuf_pc, strlen(txBuf_pc)); 
    } 
    
    /*
        success: set CIPMUX (Single)
    */ 
    _parser->send("AT+CIPMUX=1"); 

    if (_parser->recv("OK")) {
        sprintf(txBuf_pc, "\nsuccess: set CIPMUX\r\n");
        pc.write(txBuf_pc, strlen(txBuf_pc)) ;
    } else {
        sprintf(txBuf_pc, "\nFailed AT+CIPMUX=1\r\n");
        pc.write(txBuf_pc, strlen(txBuf_pc)); 
        return -1;
    }

    _parser->send("AT+CIPSERVER=1,50001"); 
    
    if (_parser->recv("OK")) {
        sprintf(txBuf_pc, "\nsuccess: set a TCP server\r\n");
        pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
    } else {
        sprintf(txBuf_pc, "\nFailed: set a TCP server\r\n");
        pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
    }

    int id; 
    while(1){ 
        
        if ( _parser->recv("%d, CONNECT", &id) ) { 
            
            sprintf(txBuf_pc, "connection request from a client\r\n") ; 
            pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
            
            while (1) { 
                int start = 0 ; 
                int len ; 
                char data[128]; 
                
                memset(data, 0, sizeof(data)) ;
                _parser->read(data, 127) ; 
                
                if ( strstr(data, "+IPD") != NULL ) { 
                
                    char * d = strstr(data, ":") + 1 ; 
                    
                    printf("%s", d) ;
                    
                    // ECHO 
                    _parser->send("AT+CIPSEND=%d,%d", id, strlen(d)) ; 
                
                    while (true) { 
                        if ( _parser->recv("OK") ) { 
                            _parser->write(d, strlen(d)); 
                            break; 
                        } 
                    } 
                }   
                
                // ID,CLOSED
                if ( strstr(data, "CLOSED") != NULL) { 
                    sprintf(txBuf_pc, "Connection closed\r\n");
                    pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
                    
                    sprintf(txBuf_pc, "Wait a connection\r\n");
                    pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
                    break; 
                } 
            } 
        } 
    }
}

