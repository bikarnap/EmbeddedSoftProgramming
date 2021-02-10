#include <LiquidCrystal.h>

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

int motor_speed = 0; 
volatile int pulse = 0; 
volatile float distance_cm = 0; 

LiquidCrystal lcd (37,36,35,34,33,32);

void setup() {
  pinMode(JOYSTICK_X, INPUT); 
  pinMode(JOYSTICK_Y, INPUT); 
  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M1_SPEED, OUTPUT);
  pinMode(M2_SPEED, OUTPUT);
  pinMode(ENCA_M1, INPUT);
  pinMode(ENCA_M2, INPUT);
  pinMode(ENCB_M1, INPUT);
  pinMode(ENCB_M2, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCB_M1), ISR_ENCB_M1, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCB_M2), ISR_ENCB_M2, FALLING);
  Serial.begin(9600); 
  lcd.begin(20, 4); 
  lcd.setCursor(0, 0);  
  lcd.print("Command:"); 
  lcd.setCursor(0, 1); 
  lcd.print("Dist.:");  
}
String command ="";
String value="";
void loop() {
  lcd.setCursor(8, 0); 
  lcd.print("             "); 
  lcd.setCursor(8, 0);
  if(command != "" && value!="") {
    lcd.print(command);
    lcd.print(":");
    lcd.print(value);
  }
   
  lcd.setCursor(6, 1); 
  lcd.print("                "); 
  lcd.setCursor(6, 1); 
  lcd.print(abs(distance_cm)); 
  lcd.print(" cm");
  if(distance_cm > 0.) 
    lcd.print("(F)");
  if(distance_cm < 0.0)
    lcd.print("(R)");
  if(Serial.available() > 0) {
    String text = Serial.readStringUntil('\n'); 
    int sep_index = text.indexOf(':');
    pulse = 0; 
    distance_cm = 0.0;
    if (sep_index != -1) {
      command = text.substring(0, sep_index); 
      value = text.substring(sep_index + 1, text.length()); 
      Serial.print("COMMAND: "); 
      Serial.print(command); 
      Serial.print("  VALUE: "); 
      Serial.println(value); 
    } else {
      Serial.println("Invalid coammand:value pair"); 
    }
  }
  int d = value.toInt();
  if(d > 0) {
    motor_speed = 100; 
    digitalWrite(M1_DIR, LOW);
    digitalWrite(M2_DIR, LOW);
    analogWrite(M1_SPEED, motor_speed); 
    analogWrite(M2_SPEED, motor_speed); 
    if(distance_cm >= float(d)) {
      motor_speed = 0; 
      digitalWrite(M1_DIR, HIGH);
      digitalWrite(M2_DIR, HIGH);
      analogWrite(M1_SPEED, motor_speed); 
      analogWrite(M2_SPEED, motor_speed); 
     
    } 
  } else if(d < 0) {
    motor_speed = 100; 
    digitalWrite(M1_DIR, HIGH);
    digitalWrite(M2_DIR, HIGH);
    analogWrite(M1_SPEED, motor_speed); 
    analogWrite(M2_SPEED, motor_speed); 
    
    if(distance_cm <= float(d)) {
      motor_speed = 0; 
      digitalWrite(M1_DIR, LOW);
      digitalWrite(M2_DIR, LOW);
      analogWrite(M1_SPEED, motor_speed); 
      analogWrite(M2_SPEED, motor_speed);      
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

  distance_cm = (30./1000.) * pulse; 
}

void ISR_ENCB_M2() {
  int dir = digitalRead(ENCA_M2); 
  if(dir == HIGH)
    pulse++; 
  else
    pulse--;
}

// -- End of ISRs --
