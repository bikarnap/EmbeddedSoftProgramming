#include <LiquidCrystal.h>

//                 RS  E   D4  D5  D6  D7
LiquidCrystal lcd (37, 36, 35, 34, 33, 32); // LCD pin wiring setting


bool isLedON; 
void setup() {
  // put your setup code here, to run once:

  pinMode(LED_BUILTIN, OUTPUT); 
  Serial.begin(9600); 
  
  isLedON = false; 
  lcd.begin(20, 2);   // Display size definition: 20 chars 2 rows
  lcd.setCursor(0, 0); 
  lcd.print("LED STATUS: "); 
  lcd.setCursor(12, 0); 
  welcomeMessage();
  instruction(); 
  Serial.println("\Waiting for command...\n"); 
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0) {
    String text = Serial.readStringUntil('\n'); 
    
    if(text == "Led:ON"){
      if(isLedON)
        Serial.println("The LED was already lit ON"); 
      else
        Serial.println("The LED is lit ON");
      digitalWrite(LED_BUILTIN, HIGH); 
      isLedON = true; 
    }
    else if(text == "Led:OFF") {
      if(isLedON)
        Serial.println("The LED is lit OFF"); 
      else 
        Serial.println("The LED was already lit OFF");
      digitalWrite(LED_BUILTIN, LOW); 
      isLedON = false; 
    } else {
      Serial.println("Not a valid command!"); 
      instruction();
    }
    Serial.println("\nWaiting for command...\n"); 
  }
  lcd.print("         "); 
  lcd.setCursor(12, 0); 
  isLedON 
    ? lcd.print("ON ")
    : lcd.print("OFF"); 
  
}

void welcomeMessage() {
  Serial.println("Welcome to the LED program"); 
}

void instruction() {
  Serial.println("Available Commands"); 
  Serial.println("----------------------"); 
  Serial.println("Led:ON  => Lits the LED ON"); 
  Serial.println("Led:OFF  => Lits the LED OFF"); 
}
