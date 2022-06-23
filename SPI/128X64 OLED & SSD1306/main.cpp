
#include "mbed.h"
#include "Adafruit_SSD1306.h"
#define HTU21D_TEMP_READ 0xF3
#define HTU21D_HUMI_READ 0xF5 
#define HTU21G_USER_WRITE 0xE6
#define HTU21D_ADDR 0x40

I2C i2c(D14, D15); //D14, D15 (PB_8)

//Adafruit_SSD1306_Spi(PinName MOSI, PinName CLK, PinName CS, 
//                         PinName DC, PinName RST, 
//                         uint8_t rawHeight = 32, uint8_t rawWidth = 128)
Adafruit_SSD1306_Spi gOLED(D11, D13, D10 /*CS*/, D4 /*DC*/, D7 /*RST*/, 64) ; 

float getTemperature() { 
    char tx[1]; 
    char rx[2]; 
    
    int check = 0 ; 
    unsigned int raw_temperature = -1; 
    float temperature = -1;
    
    // Temperature 
    tx[0] = HTU21D_TEMP_READ ;
    if ( i2c.write((HTU21D_ADDR << 1) & 0xFE, tx, 1) ) { 
        return -1; 
    } 
        
    while ( (check = i2c.read((HTU21D_ADDR << 1) | 0x01, rx, 2)) != 0 ) {} 
    
    raw_temperature = ((unsigned int) rx[0] << 8) | (unsigned int) rx[1];
    raw_temperature &= 0xFFFC; // 12 bit
    
    float temp = (raw_temperature / (float)65536); 
    temperature = -46.85 + (175.72 * temp); //2^16 = 65536 
    
    return temperature; 
} 

float getHumidity() { 
    char tx[1]; 
    char rx[2]; 
    
    int check = 0 ; 
    unsigned int raw_humidity = -1; 
    float humidity = -1; 
    
    // Humidity
    tx[0] = HTU21D_HUMI_READ;
    if ( i2c.write((HTU21D_ADDR << 1) & 0xFE, tx, 1) ) {
        return -1;
    } 
        
    while ( (check = i2c.read((HTU21D_ADDR << 1) | 0x01 , rx, 2)) != 0 ) { } 
    
    raw_humidity = ((unsigned int) rx[0] << 8) | (unsigned int) rx[1];
    raw_humidity &= 0xFFFC; // 12 bit 
            
    float temp = (raw_humidity / (float)65536); 
    humidity = -6 + (125 * temp);
    
    return humidity ; 
} 

int main()
{
    ThisThread::sleep_for(3s) ; 
    gOLED.clearDisplay();  
    
    gOLED.setTextSize(2); 
    gOLED.printf("Jinil-Kim\r\n\n"); 
    
    gOLED.setTextSize(1); 
    gOLED.printf("Temperature:\r\n\n");  
    gOLED.printf("Humidity:"); 
    
    while(1) { 
        gOLED.setTextCursor(13 * 6, 4 * 8) ; 
        gOLED.printf("%4.1f c", getTemperature()); 
        
        gOLED.setTextCursor(13 * 6, 6 * 8) ; 
        gOLED.printf("%4.1f %%", getHumidity()); 
        
        gOLED.display() ;
        ThisThread::sleep_for(3s); 
    } 
    
}
