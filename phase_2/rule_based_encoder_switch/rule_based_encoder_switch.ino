// Motor pin definitions
#define m1 12   // Right Motor pin1
#define m2 11   // Right Motor pin2
#define m3 9    // Left Motor pin1
#define m4 8    // Left Motor pin2
#define e1 10   // Right Motor Enable
#define e2 13   // Left Motor Enable

// 5 Channel IR Sensor Connections
#define ir1 A5  // Left Most Sensor
#define ir2 A4  // Left Sensor
#define ir3 A3  // Middle Sensor
#define ir4 A2  // Right Sensor
#define ir5 A1  // Right Most Sensor

// Encoder pin and constants
#define ENCODER_PIN 7
const float HOLES_PER_REV = 20.0;        // 20 holes per revolution
const float WHEEL_DIAMETER = 6.3;        // Wheel diameter in cm
const float WHEEL_CIRCUM = 3.14159 * WHEEL_DIAMETER; // Circumference in cm
const float TARGET_DISTANCE = 583.0;     // Target distance in cm

volatile float holes = 0;  // Volatile for interrupt
float distance = 0;
bool lineFollowing = true; // Flag to control mode

void setup() {
  Serial.begin(9600);

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

  // Encoder pin
  pinMode(ENCODER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), count, FALLING);
}

void loop() {
  // Calculate distance traveled
  distance = (holes / HOLES_PER_REV) * WHEEL_CIRCUM;

  // Check if target distance is reached
  if (distance >= TARGET_DISTANCE && lineFollowing) {
    Serial.println("Reached 583 cm. Switching to straight forward mode.");
    lineFollowing = false; // Switch to straight forward mode
  }

  if (lineFollowing) {
    // IR sensor readings
    int s1 = digitalRead(ir1);  // Left Most Sensor
    int s2 = digitalRead(ir2);  // Left Sensor
    int s3 = digitalRead(ir3);  // Middle Sensor
    int s4 = digitalRead(ir4);  // Right Sensor
    int s5 = digitalRead(ir5);  // Right Most Sensor

    // Set default motor speeds
    analogWrite(e1, 100);  // Right motor speed
    analogWrite(e2, 100);  // Left motor speed

    // Middle sensor detects line
    if ((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Moving Forward");
    }
    // Left sensor detects line
    else if ((s1 == 1) && (s2 == 0) && (s3 == 1) && (s4 == 1) && (s5 == 1)) {
      analogWrite(e1, 90);     // Right motor full speed
      analogWrite(e2, 70);     // Left motor reduced speed
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Turning Right (Left Sensor)");
    }
    // Left most sensor detects line
    else if ((s1 == 0) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Backward
      digitalWrite(m4, LOW);
      Serial.println("Sharp Right Turn");
    }
    // Right sensor detects line
    else if ((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 0) && (s5 == 1)) {
      analogWrite(e1, 70);     // Right motor reduced speed
      analogWrite(e2, 90);     // Left motor full speed
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Turning Left (Right Sensor)");
    }
    // Right most sensor detects line
    else if ((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Backward
      digitalWrite(m2, LOW);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Sharp Left Turn");
    }
    // Middle and right sensors detect line
    else if ((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 1)) {
      digitalWrite(m1, HIGH);  // Stop
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Gentle Left Turn");
    }
    // Middle and left sensors detect line
    else if ((s1 == 1) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Stop
      digitalWrite(m4, HIGH);
      Serial.println("Gentle Right Turn");
    }
    // Middle, left and left most sensors detect line
    else if ((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Stop
      digitalWrite(m4, HIGH);
      Serial.println("Strong Right Turn");
    }
    // Middle, right and right most sensors detect line
    else if ((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Stop
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Strong Left Turn");
    }
    // All sensors detect line
    else if ((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 0) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Stop
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Stop
      digitalWrite(m4, HIGH);
      Serial.println("Stopped - All Sensors Detect Line");
    }
  } else {
    // Straight forward mode
    analogWrite(e1, 0);  // Right motor speed
    analogWrite(e2, 0);  // Left motor speed
    digitalWrite(m1, LOW);   // Forward
    digitalWrite(m2, HIGH);
    digitalWrite(m3, LOW);   // Forward
    digitalWrite(m4, HIGH);
    Serial.println("Moving Straight Forward");
  }
}

void count() {
  holes++;
}