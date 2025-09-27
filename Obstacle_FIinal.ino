//=================== PIN DEFINITIONS ===================//
#define enA 10 //Enable1 L298 Pin enA 
#define in1 9  //Motor1 L298 Pin in1 
#define in2 8  //Motor1 L298 Pin in2 
#define in3 7  //Motor2 L298 Pin in3 
#define in4 6  //Motor2 L298 Pin in4 
#define enB 5  //Enable2 L298 Pin enB 
#define L_S A0 //IR sensor Left
#define R_S A1 //IR sensor Right
#define echo A2    //Ultrasonic Echo pin
#define trigger A3 //Ultrasonic Trigger pin
#define servo A5   //Servo pin

//=================== GLOBAL VARIABLES ===================//
int Set = 15; // Safe distance in cm
int distance_L, distance_F, distance_R; 
int motorSpeed = 120; // Motor speed (0–255). Lower = slower.

//=================== SETUP ===================//
void setup(){ 
  Serial.begin(9600);

  pinMode(R_S, INPUT);   
  pinMode(L_S, INPUT);   
  pinMode(echo, INPUT);  
  pinMode(trigger, OUTPUT);  
  pinMode(enA, OUTPUT);  
  pinMode(in1, OUTPUT);  
  pinMode(in2, OUTPUT);  
  pinMode(in3, OUTPUT);  
  pinMode(in4, OUTPUT);  
  pinMode(enB, OUTPUT);  
  pinMode(servo, OUTPUT);

  // Apply motor speed
  analogWrite(enA, motorSpeed); 
  analogWrite(enB, motorSpeed);

  // Servo initial sweep
  for (int angle = 70; angle <= 140; angle += 5) servoPulse(servo, angle);  
  for (int angle = 140; angle >= 0; angle -= 5)  servoPulse(servo, angle);  
  for (int angle = 0; angle <= 70; angle += 5)   servoPulse(servo, angle);  

  distance_F = Ultrasonic_read();
  delay(500);
}

//=================== MAIN LOOP ===================//
void loop(){  
  distance_F = Ultrasonic_read();
  Serial.print("D F="); Serial.println(distance_F);

  // Both sensors white → move forward if safe
  if ((digitalRead(R_S) == 0) && (digitalRead(L_S) == 0)) {
    if (distance_F > Set) {
      forword();
    } else {
      Check_side();
    }
  }  
  // Right sensor black, Left white → turn right
  else if ((digitalRead(R_S) == 1) && (digitalRead(L_S) == 0)) {
    turnRight();
  }  
  // Left sensor black, Right white → turn left
  else if ((digitalRead(R_S) == 0) && (digitalRead(L_S) == 1)) {
    turnLeft();
  }  

  delay(10);
}

//=================== SERVO FUNCTION ===================//
void servoPulse(int pin, int angle) {
  int pwm = (angle * 11) + 500; // Convert angle to microseconds
  digitalWrite(pin, HIGH);
  delayMicroseconds(pwm);
  digitalWrite(pin, LOW);
  delay(50); // Refresh cycle
}

//=================== ULTRASONIC ===================//
long Ultrasonic_read() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  long time = pulseIn(echo, HIGH);
  return time / 29 / 2; // Convert to cm
}

//=================== SIDE CHECK ===================//
void Check_side() {
  Stop();
  delay(100);

  // Scan right
  for (int angle = 70; angle <= 140; angle += 5) servoPulse(servo, angle);  
  delay(300);
  distance_R = Ultrasonic_read();
  Serial.print("D R="); Serial.println(distance_R);
  delay(100);

  // Scan left
  for (int angle = 140; angle >= 0; angle -= 5) servoPulse(servo, angle);  
  delay(500);
  distance_L = Ultrasonic_read();
  Serial.print("D L="); Serial.println(distance_L);
  delay(100);

  // Return servo to center
  for (int angle = 0; angle <= 70; angle += 5) servoPulse(servo, angle);  
  delay(300);

  compareDistance();
}

//=================== COMPARE DISTANCE ===================//
void compareDistance() {
  if (distance_L > distance_R) {
    turnLeft();  delay(500);
    forword();   delay(600);
    turnRight(); delay(500);
    forword();   delay(600);
    turnRight(); delay(400);
  } else {
    turnRight(); delay(500);
    forword();   delay(600);
    turnLeft();  delay(500);
    forword();   delay(600);  
    turnLeft();  delay(400);
  }
}

//=================== MOTOR FUNCTIONS ===================//
void forword() {
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, LOW);  
  digitalWrite(in2, HIGH); 
  digitalWrite(in3, HIGH); 
  digitalWrite(in4, LOW);  
}

void backword() {
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);  
  digitalWrite(in4, HIGH); 
}

void turnRight() {
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, LOW);  
  digitalWrite(in2, HIGH); 
  digitalWrite(in3, LOW);  
  digitalWrite(in4, HIGH); 
}

void turnLeft() {
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);  
  digitalWrite(in3, HIGH); 
  digitalWrite(in4, LOW);  
}

void Stop() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW); 
  digitalWrite(in2, LOW); 
  digitalWrite(in3, LOW); 
  digitalWrite(in4, LOW); 
}
