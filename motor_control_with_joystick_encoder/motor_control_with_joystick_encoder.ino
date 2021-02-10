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
String direction = "";

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
  digitalWrite(M1_DIR, LOW); 
  digitalWrite(M2_DIR, LOW); 
  analogWrite(M1_SPEED, motor_speed);
  analogWrite(M2_SPEED, motor_speed);
  attachInterrupt(digitalPinToInterrupt(ENCB_M1), ISR_ENCB_M1, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCB_M2), ISR_ENCB_M2, FALLING);
  Serial.begin(9600); 

  lcd.begin(16, 4); 
  lcd.setCursor(0, 0);  
  lcd.print("PULSE#: "); 
  lcd.setCursor(0, 1); 
  lcd.print("X-POS: ");  
  instructions();
}

void loop() {
  // Read joystick x-, and y-poistions
  int x_position = analogRead(JOYSTICK_X); 
  int y_position = analogRead(JOYSTICK_Y);
  
  /* x_position >= 512 --> forward movement
     x_position < 512 --> reverse movement
     otherwise stop */
  if (x_position > 500) { 
    digitalWrite(M1_DIR, LOW); 
    digitalWrite(M2_DIR, LOW);
    motor_speed = (255./500.)*x_position - 255.;
    direction = "Forward";
    printInfo(pulse, motor_speed, direction);
  } else if (x_position < 500) {
    digitalWrite(M1_DIR, HIGH);
    digitalWrite(M2_DIR, HIGH); 
    motor_speed = -255./500. * x_position + 255.;
    direction = "Backward";
    printInfo(pulse, motor_speed, direction); 
  } else {
    motor_speed = 0; 
  }
  analogWrite(M1_SPEED, motor_speed);
  analogWrite(M2_SPEED, motor_speed);

  lcd.setCursor(9, 0); 
  lcd.print("         "); 
  lcd.setCursor(9, 0);
  lcd.print(pulse); 

  lcd.setCursor(9, 1); 
  lcd.print("         "); 
  lcd.setCursor(9, 1); 
  lcd.print(x_position);   
}

// -- End of loop --

// Interrupt-Service Routines (ISRs)
void ISR_ENCB_M1() {
  int dir = digitalRead(ENCA_M1); 
  if(dir == HIGH)
    pulse++; 
  else
    pulse--; 
}

void ISR_ENCB_M2() {
  int dir = digitalRead(ENCA_M2); 
  if(dir == HIGH)
    pulse++; 
  else
    pulse--; 
}

// -- End of ISRs --

void printInfo(int p, int s, String d) {
  Serial.print("PULSE COUNT: "); 
  Serial.print(p);
  Serial.print("  MOTOR SPEED: "); 
  Serial.print(s); 
  Serial.print("  MOTOR DIRECTION: "); 
  Serial.println(d);
}

void instructions() {
  Serial.println("Use the joystick to move the vehicle forware or backward"); 
  Serial.println("\nWaiting for joystick movement..."); 
}
