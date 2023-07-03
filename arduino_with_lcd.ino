#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int digitalInputPin = 7;

void setup()
{
  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("IOT SmartFarming");
  lcd.setCursor(0,1);
  lcd.print("Pump Status: OFF");
  pinMode(digitalInputPin, INPUT_PULLUP);
  Serial.begin(9600);
}


void loop() {
  delay(1000);
  Serial.print("Digital Read at Pin 5: ");
  Serial.println(digitalRead(digitalInputPin));
  // read the state coming from ESP32
  if (digitalRead(digitalInputPin) == HIGH) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("IOT SmartFarming");
    lcd.setCursor(0,1);
    lcd.print("Pump Status: ON");
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("IOT SmartFarming");
    lcd.setCursor(0,1);
    lcd.print("Pump Status: OFF");
  }  
}
