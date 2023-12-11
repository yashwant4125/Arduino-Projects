
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  //for LCD display with I2C interface
#include <Keypad.h>             //for interfacing keypad matrix

// Keypad configuration starts

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {3, 4, 5, 6}; //pins of arduino for keypad rows
byte colPins[COLS] = {7, 8, 9, 10};  //pins of arduino for keypad columns
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //to create keypad matrix

// Keypad configuration ends

char key ;
String highTemp ; //high temp data
String lowTemp ; //low temp data

float temp, low, high ;

byte data_count = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // create lcd object and LCD address to 0x27 for a 16 chars and 2 line display

void initial_message() //to display initial message 
{  
  lcd.setCursor(0,0) ;
  lcd.print("Room Temperature");
  lcd.setCursor(3,1) ;
  lcd.print("Controller");
  delay(5000) ;
  lcd.clear() ;
  lcd.setCursor(3,0) ;
  lcd.print("Using LM35");
  delay(5000) ;   
}

void set() //prompt to enter readings
{ 
  lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print("Press A to set");
  lcd.setCursor(0,1) ;
  lcd.print("Temperature:|");
}

void checkA() //Waits for key press to enter readings
{
  key = customKeypad.getKey();  
  if( key != 'A') {
    checkA() ;
  }
}

void getTemp() //to get temp readings
{
  lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print("Enter higher") ;
  lcd.setCursor(0,1) ;
  lcd.print("Temperature:|") ;
  lcd.setCursor(14,1) ;
  lcd.print("*C") ;
  highTemp = "" ;
  while (data_count != 2) {
    key = customKeypad.getKey(); 
    if(key){ 
      highTemp += key;
      data_count++;
    }
  }
  data_count = 0 ; 
  lcd.setCursor(12,1) ; 
  lcd.print(highTemp);
  high = highTemp.toFloat() ;
  delay(1000) ;

  lcd.setCursor(6,0) ;
  lcd.print("lower ") ;
  lcd.setCursor(12,1) ;
  lcd.print("| ") ;
  lowTemp = "" ;
  while (data_count != 2) {
    key = customKeypad.getKey(); 
    if(key){ 
      lowTemp += key;
      data_count++;
    }
  }
  data_count = 0 ;  
  lcd.setCursor(12,1) ;
  lcd.print(lowTemp);
  low = lowTemp.toFloat() ;
  delay(1000) ;
  if(low <= high && low >= 0 && high <= 50 ) //checks if reading entered are in range 
  {
    lcd.clear() ;
    lcd.print("Temp. Range") ;
    lcd.setCursor(0,1) ;
    lcd.print("Set Sucessfully");
    delay(1000) ;
  }
  else {
    lcd.clear() ;
    lcd.print("Range Error!") ;
    lcd.setCursor(0,1) ;
    lcd.print("Set Again");
    delay(1000) ;
  }
}


void setup() 
{

  Serial.begin(9600) ;
  
  pinMode(11, OUTPUT) ;  //for VCC of LM35
  digitalWrite(11, 1) ;

  pinMode(2, OUTPUT) ; //for VCC of relay
  digitalWrite(2, 1) ;
  
  pinMode(12, OUTPUT) ; //Input to relay
  digitalWrite(12, 0) ;

  lcd.init();
  lcd.backlight();
  // Initial message on display
  initial_message() ;    

  set() ;

  checkA() ;

  getTemp() ;

}

void currentTemp() //to display current temperature reading
{
  lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print("Current Temp.") ;
  lcd.setCursor(10,1) ;
  lcd.print("*C") ;
}

void readTemp() //to get sensor data and convert to Celsius 
{  
  temp = analogRead(A0) * 0.48828125 ;
  currentTemp() ;
  lcd.setCursor(5,1) ;
  lcd.print(temp) ;
  delay(1000) ;
}

void pause() //to pause the system
{
  lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print("Long Press C");
  lcd.setCursor(0,1) ;
  lcd.print("To Halt:|");
  delay(1500) ;
  key = customKeypad.getKey(); 
  if(key == 'C') { 
    lcd.clear() ;
    lcd.setCursor(0,0) ;
    lcd.print("Halted, Press D");
    lcd.setCursor(0,1) ;
    lcd.print("To Resume:|");
    key = customKeypad.getKey(); 
    while(key != 'D') {
      key = customKeypad.getKey(); 
    }
  }

}

void reset() //to reset the temperature range
{
  lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print("Long Press B to");
  lcd.setCursor(0,1) ;
  lcd.print("Reset Temp Range");
  delay(1500) ;
  key = customKeypad.getKey(); 
  if(key == 'B') { 
    getTemp() ;
  }

}

void range() //to display temperature range
{
  lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print("Temp. Range") ;
  lcd.setCursor(0,1) ;
  lcd.print("[") ;
  lcd.print(lowTemp) ;
  lcd.print(",") ;
  lcd.print(highTemp) ;
  lcd.print("]") ;
  delay(2000) ;

  reset() ;

}

void loop() 
{  

  readTemp() ;

  if (temp > 50) {
    lcd.clear() ;
    lcd.setCursor(4,0) ;
    lcd.print("⚠Alert⚠") ;
    lcd.setCursor(0,1) ;
    lcd.print("Temp. Above 50*C") ;
    delay(1000) ;
    digitalWrite(12, 0) ;
    lcd.clear() ;
    lcd.setCursor(0,0) ;
    lcd.print("Heater OFF") ;
    delay(1000) ;
  }

  range() ;
  
  pause() ;

  if(temp < low) {
    digitalWrite(12, 1) ;
    lcd.clear() ;
    lcd.setCursor(0,0) ;
    lcd.print("Temp below reqd.") ;
    lcd.setCursor(0,1) ;
    lcd.print("Heater turned ON") ;
  }

  else if (temp > high) {
    digitalWrite(12, 0) ;
    lcd.clear() ;
    lcd.setCursor(0,0) ;
    lcd.print("Temp above reqd.") ;
    lcd.setCursor(0,1) ;
    lcd.print("Heater OFF") ;
  }

  else {
    digitalWrite(12, 1) ;
    lcd.clear() ;
    lcd.setCursor(0,0) ;
    lcd.print("Temp. in Range") ;
    lcd.setCursor(0,1) ;
    lcd.print("Heater ON") ; 
  }

  delay(10000) ;

}  
