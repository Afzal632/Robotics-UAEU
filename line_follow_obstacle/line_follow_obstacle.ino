// Pin definitions from your working ultrasonic sensor code
const int trigPin = 3;
const int echoPin = 2;

// Motor pin definitions adapted from the second code
#define m1 12   // Right Motor pin1 (was motor1pin1)
#define m2 11   // Right Motor pin2 (was motor1pin2)
#define m3 9    // Left Motor pin1 (was motor2pin1)
#define m4 8    // Left Motor pin2 (was motor2pin2)
#define e1 10   // Right Motor Enable (was motor1Enable)
#define e2 13   // Left Motor Enable (was motor2Enable)

// 5 Channel IR Sensor Connections
#define ir1 A5
#define ir2 A4
#define ir3 A3
#define ir4 A2
#define ir5 A1

void setup() {
  Serial.begin(9600);
  
  // Ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Motor pins
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  pinMode(e1, OUTPUT);
  pinMode(e2, OUTPUT);
  
  // IR sensor pins
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(ir5, INPUT);
}

void loop() {
  // Ultrasonic sensor reading
  long duration, cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);

  // IR sensor readings
  int s1 = digitalRead(ir1);  // Left Most Sensor
  int s2 = digitalRead(ir2);  // Left Sensor
  int s3 = digitalRead(ir3);  // Middle Sensor
  int s4 = digitalRead(ir4);  // Right Sensor
  int s5 = digitalRead(ir5);  // Right Most Sensor

  // If object is within 10cm, stop motors
  if (cm <= 10) {
    digitalWrite(m1, HIGH);  // Flipped from LOW
    digitalWrite(m2, HIGH);  // Flipped from LOW
    digitalWrite(m3, HIGH);  // Flipped from LOW
    digitalWrite(m4, HIGH);  // Flipped from LOW
    Serial.print("Stopped - Object at ");
    Serial.print(cm);
    Serial.println("cm");
  } 
  else {
    // Line following logic with your motor speeds (100)
    analogWrite(e1, 80);  // Right motor speed
    analogWrite(e2, 80);  // Left motor speed

    // Middle sensor detects line
    if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Flipped from HIGH
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, LOW);   // Flipped from HIGH
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // Left sensor detects line
    else if((s1 == 1) && (s2 == 0) && (s3 == 1) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Flipped from HIGH
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, HIGH);  // Flipped from LOW
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // Left most sensor detects line
    else if((s1 == 0) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Flipped from HIGH
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, HIGH);  // Flipped from LOW
      digitalWrite(m4, LOW);   // Flipped from HIGH
    }
    // Right sensor detects line
    else if((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 0) && (s5 == 1)) {
      digitalWrite(m1, HIGH);  // Flipped from LOW
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, LOW);   // Flipped from HIGH
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // Right most sensor detects line
    else if((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Flipped from LOW
      digitalWrite(m2, LOW);   // Flipped from HIGH
      digitalWrite(m3, LOW);   // Flipped from HIGH
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // Middle and right sensors detect line
    else if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 1)) {
      digitalWrite(m1, HIGH);  // Flipped from LOW
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, LOW);   // Flipped from HIGH
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // Middle and left sensors detect line
    else if((s1 == 1) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Flipped from HIGH
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, HIGH);  // Flipped from LOW
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // Middle, left and left most sensors detect line
    else if((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Flipped from HIGH
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, HIGH);  // Flipped from LOW
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // Middle, right and right most sensors detect line
    else if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Flipped from LOW
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, LOW);   // Flipped from HIGH
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
    // All sensors detect line
    else if((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 0) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Flipped from LOW
      digitalWrite(m2, HIGH);  // Flipped from LOW
      digitalWrite(m3, HIGH);  // Flipped from LOW
      digitalWrite(m4, HIGH);  // Flipped from LOW
    }
  }
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}