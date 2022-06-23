#include "mbed.h"
#define HTU21D_TEMP_READ 0xF3
#define HTU21D_HUMI_READ 0xF5 
#define HTU21G_USER_WRITE 0xE6
#define HTU21D_ADDR 0x40

I2C i2c(D14, D15); //D14, D15 (PB_8)
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200);
char buf[64];  

void getTemperature() { 
    char tx[1]; 
    char rx[2]; 
    
    int check = 0 ; 
    unsigned int raw_temperature = -1; 
    float temperature = -1;
    
    // Temperature 
    tx[0] = HTU21D_TEMP_READ ;
    if ( i2c.write((HTU21D_ADDR << 1) & 0xFE, tx, 1) ) { 
        sprintf(buf, "[Error] Temperature failed!\r\n"); 
        pc.write(buf, strlen(buf)); 
        
        return ; 
    } 
        
    while ( (check = i2c.read((HTU21D_ADDR << 1) | 0x01, rx, 2)) != 0 ) {} 
    
    raw_temperature = ((unsigned int) rx[0] << 8) | (unsigned int) rx[1];
    raw_temperature &= 0xFFFC; // 12 bit
    
    float temp = (raw_temperature / (float)65536); 
    temperature = -46.85 + (175.72 * temp); //2^16 = 65536 
    
    sprintf(buf, "Temperature = %.2f [C]\r\n", temperature); 
    pc.write(buf, strlen(buf)) ;
} 

void getHumidity() { 
    char tx[1]; 
    char rx[2]; 
    
    int check = 0 ; 
    unsigned int raw_humidity = -1; 
    float humidity = -1; 
    
    // Humidity
    tx[0] = HTU21D_HUMI_READ;
    if ( i2c.write((HTU21D_ADDR << 1) & 0xFE, tx, 1) ) {
        sprintf(buf, "[Error] Humidity failed!\r\n"); 
        pc.write(buf, strlen(buf)); 
        
        return ;
    } 
        
    while ( (check = i2c.read((HTU21D_ADDR << 1) | 0x01 , rx, 2)) != 0 ) { } 
    
    raw_humidity = ((unsigned int) rx[0] << 8) | (unsigned int) rx[1];
    raw_humidity &= 0xFFFC; // 12 bit 
            
    float temp = (raw_humidity / (float)65536); 
    humidity = -6 + (125 * temp);
    
    sprintf(buf, "Relative Humidity = %.2f [%%]\r\n", humidity); 
    pc.write(buf, strlen(buf)) ;
} 

int main()
{
    sprintf(buf, "I2C Test Program for HTU21D Sensor\r\n"); 
    pc.write(buf, strlen(buf)) ; 
    
    while (1) { 
        getTemperature() ;
        getHumidity() ; 
        ThisThread::sleep_for(3s); 
    }
}
