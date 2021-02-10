#include <LiquidCrystal.h>
#include <Wire.h>

#define M1_DIR 7          // pin for the direction of motor 1
#define M2_DIR 8          // pin for the direction of motor 2
#define M1_SPEED 9        // pin for the speed of motor 1
#define M2_SPEED 10       // pin for the speed of motor 2
#define JOYSTICK_X A9     // pin for the horizontal position of the joystick
#define JOYSTICK_Y A10    // pin for the vertical position of the joystick
#define ENCA_M1 23        // pin for sensing direction of motor 1
#define ENCB_M1 2         // pin for sensing pulse of motor 1
#define ENCA_M2 24        // pin for sensing direction of motor 2
#define ENCB_M2 3         // pin for sensing pulse of motor 2
#define CMPS14 0x60       // compass register
#define CMPS 1            // pin 
#define SDA 20
#define SCL 21

int motor_speed = 0; 
volatile int pulse = 0; 
volatile float distance_cm = 0; 
long previousAngle = 0; 
long currentAngle = 0; 

LiquidCrystal lcd (37,36,35,34,33,32);

void setup() {
  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M1_SPEED, OUTPUT);
  pinMode(M2_SPEED, OUTPUT);
  
  pinMode(ENCA_M1, INPUT);
  pinMode(ENCA_M2, INPUT);
  pinMode(ENCB_M1, INPUT);
  pinMode(ENCB_M2, INPUT);

  pinMode(SDA, INPUT); 
  pinMode(SCL, INPUT);
  
  digitalWrite(M1_DIR, LOW); 
  digitalWrite(M2_DIR, LOW); 
  analogWrite(M1_SPEED, motor_speed);
  analogWrite(M2_SPEED, motor_speed);
  
  attachInterrupt(digitalPinToInterrupt(ENCB_M1), ISR_ENCB_M1, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCB_M2), ISR_ENCB_M2, FALLING);
  
  Serial.begin(9600); 
  Wire.begin(); 

  lcd.begin(20, 4); 
  lcd.setCursor(0, 0);  
  lcd.print("                    ");
  lcd.setCursor(0, 0); 
  lcd.print("DEGREE DIRECTION");   
  lcd.setCursor(2, 1); 
}

String command;
String value;
long finalAngle;

void loop() {
  readDirection(currentAngle); 
  // Serial.println(currentAngle); 
  // const int angle = currentAngle; 
  // int finalAngle; 

  if(Serial.available() > 0) {
    readDirection(currentAngle); 
    //  Serial.println(currentAngle); 
    const long angle = currentAngle; 
    long finalAngle; 
    String text = Serial.readStringUntil('\n'); 
    int sep_index = text.indexOf(':');
    pulse = 0; 
    distance_cm = 0.0;
    finalAngle = 0; 
    command = "";
    value = ""; 
    if (sep_index != -1) {
      command = text.substring(0, sep_index); 
      value = text.substring(sep_index + 1, text.length()); 
      finalAngle = previousAngle + (long)value.toInt();
      Serial.print("COMMAND: "); 
      Serial.print(command); 
      Serial.print("  VALUE: "); 
      Serial.println(value); 
    } else {
      Serial.println("Invalid coammand:value pair"); 
    }

    finalAngle = (long)value.toInt() + angle; 
    long rem = finalAngle % 360; 
    if(rem == 1)
      finalAngle = 0; 
    
    long temp; 
    readDirection(temp);
    if(value.toInt() > 0) {
      while(temp < finalAngle) {
        digitalWrite(M1_DIR, LOW); 
        analogWrite(M1_SPEED, 100); 
        readDirection(temp);
        if(temp >= finalAngle) {
          digitalWrite(M1_DIR, HIGH); 
          analogWrite(M1_SPEED, 0);
          break;
        }
      } 
    }

    if(value.toInt() < 0) {
      while(temp > finalAngle) {
        digitalWrite(M2_DIR, LOW); 
        analogWrite(M2_SPEED, 100); 
        readDirection(temp);
        if(temp <= finalAngle) {
          digitalWrite(M2_DIR, HIGH); 
          analogWrite(M2_SPEED, 0);
          break;
        }
      } 
    }     
  }
}

// -- End of loop --

// Interrupt-Service Routines (ISRs)
void ISR_ENCB_M1() {
  int dir = digitalRead(ENCA_M1); 
  if(dir == HIGH)
    pulse++; 
  else
    pulse--; 
  distance_cm = (30./1000.) * (float)pulse; 
}

void ISR_ENCB_M2() {
  int dir = digitalRead(ENCA_M2); 
  if(dir == HIGH)
    pulse++; 
  else
    pulse--; 
}

// -- End of ISRs --

// Functions
void readDirection(long &deg) {
  Wire.beginTransmission(CMPS14); 
  Wire.write(CMPS); 
  Wire.endTransmission(false); 
  Wire.requestFrom(CMPS14, CMPS, true); 

  if(Wire.available() >= 1) {
    long raw_value = Wire.read();  
    deg = ((long)raw_value * 360) / 255; 
    char* dir;     

    if(deg == 0 || deg == 360)
      dir = "N"; 
    else if(deg == 90)  
      dir = "W"; 
    else if(deg == 180)
      dir = "S"; 
    else if(deg == 270)
      dir = "E"; 
    else if(deg > 0 && deg < 90)
      dir = "NW"; 
    else if(deg > 90 && deg < 180)
      dir = "SW"; 
    else if(deg > 180 && deg < 270)
      dir = "SE"; 
    else if(deg > 270 && deg < 360)
      dir = "NE"; 
      
    lcd.setCursor(2, 1); 
    lcd.print("    "); 
    lcd.setCursor(2, 1); 
    lcd.print(deg); 
    lcd.setCursor(11, 1); 
    lcd.print("   "); 
    lcd.setCursor(11, 1); 
    lcd.print(dir);
    
  }
}

// Get the quadrant of xy-plane based on degree
// 0-90 Quadrant 1
// 91-180 Quadrant 2
// 181-270 Quadrant 3
// 271 - 360 Quadrant 4
int getQuadrant(long deg) {
  long degree = deg % 3; 
  int ret; 
  if(degree >= 0 && degree <=90)
    ret = 1; 
  if(degree > 90 && degree <= 180)
    ret = 2; 
  if(degree > 180 && degree <= 270)
    ret = 3; 
  if(degree > 270)
    ret = 4; 
  return ret; 
}

/*
 previousAngle = currentAngle;
   lcd.setCursor(9, 0); 
  lcd.print("         "); 
  lcd.setCursor(9, 0);
  lcd.print(pulse); 

  lcd.setCursor(8, 1); 
  lcd.print("         "); 
  lcd.setCursor(8, 1); 
  lcd.print(distance_cm); 
  if(Serial.available() > 0) {
    String text = Serial.readStringUntil('\n'); 
    int sep_index = text.indexOf(':');
    pulse = 0; 
    distance_cm = 0.0;
    if (sep_index != -1) {
      command = text.substring(0, sep_index); 
      value = text.substring(sep_index + 1, text.length()); 
      finalAngle = previousAngle + (long)value.toInt();
      Serial.print("COMMAND: "); 
      Serial.print(command); 
      Serial.print("  VALUE: "); 
      Serial.println(value); 
    } else {
      Serial.println("Invalid coammand:value pair"); 
    }
  }
  int d = value.toInt();  
  while(currentAngle < finalAngle) {
    digitalWrite(M1_DIR, LOW); 
    analogWrite(M1_SPEED, 100); 
  }
  digitalWrite(M1_DIR, HIGH); 
  analogWrite(M1_SPEED, 0);  
 */
