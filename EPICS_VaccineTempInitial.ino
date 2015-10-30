
/*
Multi-sensor temperatore monitor: displays the temperature from each sensor on the LCD
adapted from http://www.instructables.com/id/Arduino-LCD-Thermometer-with-LM35-Temp-Sensor/?ALLSTEPS
LCD: http://www.dfrobot.com/wiki/index.php?title=Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)
*/

#include <OneWire.h>
#include <DallasTemperature.h> 
#include <Wire.h>
#include <Adafruit_MCP23017.h> 
#include <Adafruit_RGBLCDShield.h>
#include <LiquidCrystal.h>

#define numVials 4
#define RED 0x1
#define GREEN 0x2

#define maxTemp 10
#define minTemp -5

double tempC;
double sample[8]; //for future avr temp calculation
double maxi = 100, mini = 0; //LM35 with simple setup
double avrTempC;

//currently assumes one pin per vial, index begins at 0
int Pin[numVials];

//how to scale this up? cant we connect several pins to one oneWire bus
//setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire0(Pin[0]);
OneWire oneWire1(Pin[1]);
OneWire oneWire2(Pin[2]);
OneWire oneWire3(Pin[3]);

//pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensor0(&oneWire0);
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
DallasTemperature sensor3(&oneWire3);
DallasTemperature sensor[4] = {sensor0, sensor1, sensor2, sensor3};

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//setup for the LCD
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// read the buttons
int read_LCD_buttons() {
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // threshold for V1.1
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 return btnNONE;
}

void setup() {
  
for(int i = 0; i < numVials; i++){
  Pin[numVials] = 4 + i;
}

Serial.begin(9600); //opens serial port, sets data rate to 9600 bps
lcd.begin(16, 2); //column, row

for(int i = 0; i < numVials; i++){
  sensor[i].begin();
}

lcd.setCursor(2, 0); 
lcd.print("EPICS 2015"); 
lcd.setCursor(2, 1); 
lcd.print("Thermometer"); 
delay(5000); //in ms, delay to read text
lcd.clear(); 
lcd.setCursor(2, 0); 
lcd.print("LCD Displays"); 
lcd.setCursor(1, 1);
lcd.print(" Average Temp "); 
delay(5000);
lcd.clear();

}

void loop() {

  //go through each pin
  for(int i = 0; i < numVials; i++){
    Serial.println(""); 
    Serial.print("DS18B20 Raw data: ");
    
      //get 8 samples
      for(int j = 0; j < numVials; j++){
        tempC = 0;  //reset var
        sensor[i].requestTemperatures();
        sample[j] = sensor[i].getTempCByIndex(0);
        
        if(sample[j] > maxTemp){
          lcd.setBacklight(RED);
          } 
          else {lcd.setBacklight(GREEN);}

        //specify zone number zone number
        String zone = String(i);
        lcd.setCursor(0, 0); 
        lcd.print("Zone "); 
        lcd.print(zone);
        lcd.print(" temp is:");
        lcd.setCursor(1, 1); 
        lcd.print(" Celcius "); 
        lcd.setCursor(12, 1); 
        lcd.print(sample[j]);
        tempC = tempC + sample[j]; 
        delay(800);
      }

    Serial.println("");
    Serial.println("");
    avrTempC = tempC / 8.0;

    //set new min and max
    if(avrTempC > maxi){
      maxi = avrTempC;
    }
    
    if(avrTempC < mini){
      mini = avrTempC;
    }

    Serial.println("New measurement:");
    Serial.print(" Average temperature in Celcius is " ); 
    Serial.println(tempC);
    Serial.print(" Recorded max temperature in Celcius is " ); 
    Serial.println(maxi);
    Serial.print(" Recorded min temperature in Celcius is " ); 
    Serial.println(mini);
    }
  }
   
