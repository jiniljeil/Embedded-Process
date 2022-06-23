#include "mbed.h"

#define SERVER_IP "192.168.0.19"
#define SERVER_PORT 50000 

UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ; 
WiFiInterface *wifi ; 

TCPSocket socket ;
Thread sock_thread ; 

char rxBuf_pc[80] ; 
char txBuf_pc[80] ;

int index = 0 ; 
volatile int flag ; 

void rx_cb(void) {
    char ch ; 
    if (pc.read(&ch, 1)) { 
        pc.write(&ch, 1) ;
        
        if ( ch == '\r') { 
            rxBuf_pc[index] = '\0'; 
            index = 0 ; 
            flag = 1; 
        } else { 
            if ( ch != '\n') rxBuf_pc[index++] = ch ; 
        }
    }  
} 

void rx_thread() { 
    char * buf = (char *)malloc(sizeof(char) * 1024); 
    
    while (true) { 
        nsapi_size_or_error_t size = socket.recv(buf, 1024) ; 
        
        // When failing, the value fo size variabel is less than zero. 
        if ( size < 0 ) { 
            // When not receiving the data in the case of Non-blocking socket or setting timeout.
            // NSAPI_ERROR_WOULD_BLOCK(-3001) 
            if ( size == NSAPI_ERROR_WOULD_BLOCK) continue ; 
            
            sprintf(txBuf_pc, "Close or Error while receiving data (%d)\r\n", size) ; 
            pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
            
            return ;
        }   
        
        // turn into valid C string 
        buf[size] = '\0'; 
        
        sprintf(txBuf_pc, "\r\nRx data: (%d) %s \r\n", size, buf) ; 
        pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
    } 
} 

int main()
{
    SocketAddress sockAddr ; 
    SocketAddress serverAddr(SERVER_IP, SERVER_PORT); 
    
    sprintf(txBuf_pc, "\r\nWiFi TCP Client example\r\n") ; 
    pc.write(txBuf_pc, strlen(txBuf_pc)); 
    pc.attach(rx_cb); 
    
    wifi = WiFiInterface::get_default_instance() ; 
    if ( !wifi ) { 
        sprintf(txBuf_pc, "ERROR: No WiFiInterface found.\n"); 
        pc.write(txBuf_pc, strlen(txBuf_pc)); 
        while(1); 
    }   
    
    sprintf(txBuf_pc, "Connecting to %s...\r\n", MBED_CONF_APP_WIFI_SSID); 
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;
    
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2); 
    if ( ret != 0 ) { 
        sprintf(txBuf_pc, "Connection error!!\r\n") ;
        pc.write(txBuf_pc, strlen(txBuf_pc)); 
        return -1; 
    } 
    
    sprintf(txBuf_pc, "Success!!\r\n"); 
    pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
    
    sprintf(txBuf_pc, "RSSI: %d\r\n", wifi->get_rssi()); 
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;
    
    sprintf(txBuf_pc, "MAC: %s\r\n", wifi->get_mac_address()); 
    pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
    
    wifi->get_ip_address(&sockAddr) ; 
    
    sprintf(txBuf_pc, "IP: %s, ", sockAddr.get_ip_address()) ; 
    pc.write(txBuf_pc, strlen(txBuf_pc)); 
    
    wifi->get_netmask(&sockAddr) ; 
    
    sprintf(txBuf_pc, "Netmask: %s, ", sockAddr.get_ip_address()) ; 
    pc.write(txBuf_pc, strlen(txBuf_pc)); 
    
    wifi->get_gateway(&sockAddr); 
    
    sprintf(txBuf_pc, "Gateway: %s\r\n", sockAddr.get_ip_address()) ; 
    pc.write(txBuf_pc, strlen(txBuf_pc)); 
    
    // Open a TCP socket on the network interface
    socket.open(wifi); 
    
    // Create a TCP connection to a server 
    int response = socket.connect(serverAddr) ; 
    
    if ( response != 0 ) { 
        sprintf(txBuf_pc, "Error connecting: %d\r\n", response) ; 
        pc.write(txBuf_pc, strlen(txBuf_pc)); 
        socket.close(); 
        return -1; 
    } 
    
    sock_thread.start(&rx_thread) ; 
    
    while (true) { 
        flag = 0 ; 
        sprintf(txBuf_pc, "Enter characters to send to a server: "); 
        pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
        
        while (flag != 1) { } 
    
        if ( rxBuf_pc[0] == 'q' || rxBuf_pc[0] == 'Q' ) { 
            break ;        
        } else { 
            int len = strlen(rxBuf_pc) ; 
            
            sprintf(txBuf_pc, "Sent: %s (%d)\r\n", rxBuf_pc, len) ; 
            pc.write(txBuf_pc, strlen(txBuf_pc)) ; 
             
            rxBuf_pc[len++] = 0x0D; // '\r' (CR)
            rxBuf_pc[len++] = 0x0A; // '\n' (LF)
            
            socket.send((const char *) rxBuf_pc, len); 
            ThisThread::sleep_for(0.5); 
        } 
        
    } 
    
    // remove the socket
    socket.close() ; 
    wifi->disconnect(); 
    
    sock_thread.join() ;
    
    sprintf(txBuf_pc, "RX sock_thread joined!!\r\n") ; 
    pc.write(txBuf_pc, strlen(txBuf_pc)); 
    sprintf(txBuf_pc, "\nDone\r\n") ; 
    pc.write(txBuf_pc, strlen(txBuf_pc));  
    
    while(1) ; 
 }
