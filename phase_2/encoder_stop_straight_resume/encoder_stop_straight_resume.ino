// Motor pin definitions
#define m1 12   // Right Motor pin1
#define m2 11   // Right Motor pin2
#define m3 9    // Left Motor pin1
#define m4 8    // Left Motor pin2
#define e1 10   // Right Motor Enable
#define e2 13   // Left Motor Enable

// 5 Channel IR Sensor Connections
#define ir1 A5
#define ir2 A4
#define ir3 A3
#define ir4 A2
#define ir5 A1

// Encoder pin and wheel parameters
#define ENCODER_PIN 7
const float HOLES_PER_REV = 20.0;        // 20 holes per full revolution
const float WHEEL_DIAMETER = 6.3;        // in cm
const float WHEEL_CIRCUM = 3.14159 * WHEEL_DIAMETER; // Circumference in cm
volatile float holes = 0;                // Encoder pulse count (volatile for interrupt)

// Distance thresholds (in cm)
const float LINE_FOLLOW_DISTANCE = 50.0; // Distance for initial line following
const float STRAIGHT_DISTANCE = 50.0;    // Distance for going straight

// State machine modes
enum RobotMode { LINE_FOLLOWING, GO_STRAIGHT };
RobotMode currentMode = LINE_FOLLOWING;

// Flag to prevent cycling
bool hasGoneStraight = false;

// Global variables
float distanceTraveled = 0;

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
  distanceTraveled = (holes / HOLES_PER_REV) * WHEEL_CIRCUM;

  // Handle mode-specific behavior
  if (currentMode == LINE_FOLLOWING) {
    // Line-following logic
    int s1 = digitalRead(ir1);  // Left Most Sensor
    int s2 = digitalRead(ir2);  // Left Sensor
    int s3 = digitalRead(ir3);  // Middle Sensor
    int s4 = digitalRead(ir4);  // Right Sensor
    int s5 = digitalRead(ir5);  // Right Most Sensor

    // Set default motor speeds
    analogWrite(e1, 80);  // Right motor speed
    analogWrite(e2, 80);  // Left motor speed

    // Middle sensor detects line
    if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Moving Forward");
    }
    // Left sensor detects line
    else if((s1 == 1) && (s2 == 0) && (s3 == 1) && (s4 == 1) && (s5 == 1)) {
      analogWrite(e1, 80);     // Right motor full speed
      analogWrite(e2, 40);     // Left motor reduced speed
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Turning Right (Left Sensor)");
    }
    // Left most sensor detects line
    else if((s1 == 0) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Backward
      digitalWrite(m4, LOW);
      Serial.println("Sharp Right Turn");
    }
    // Right sensor detects line
    else if((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 0) && (s5 == 1)) {
      analogWrite(e1, 40);     // Right motor reduced speed
      analogWrite(e2, 80);     // Left motor full speed
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Turning Left (Right Sensor)");
    }
    // Right most sensor detects line
    else if((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Backward
      digitalWrite(m2, LOW);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Sharp Left Turn");
    }
    // Middle and right sensors detect line
    else if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 1)) {
      digitalWrite(m1, HIGH);  // Stop
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Gentle Left Turn");
    }
    // Middle and left sensors detect line
    else if((s1 == 1) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Stop
      digitalWrite(m4, HIGH);
      Serial.println("Gentle Right Turn");
    }
    // Middle, left and left most sensors detect line
    else if((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1)) {
      digitalWrite(m1, LOW);   // Forward
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Stop
      digitalWrite(m4, HIGH);
      Serial.println("Strong Right Turn");
    }
    // Middle, right and right most sensors detect line
    else if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Stop
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);   // Forward
      digitalWrite(m4, HIGH);
      Serial.println("Strong Left Turn");
    }
    // All sensors detect line
    else if((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 0) && (s5 == 0)) {
      digitalWrite(m1, HIGH);  // Stop
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);  // Stop
      digitalWrite(m4, HIGH);
      Serial.println("Stopped - All Sensors Detect Line");
    }

    // Check if distance threshold for line following is reached (only if not yet gone straight)
    if (!hasGoneStraight && distanceTraveled >= LINE_FOLLOW_DISTANCE) {
      currentMode = GO_STRAIGHT;
      holes = 0; // Reset encoder count
      Serial.println("Switching to GO_STRAIGHT mode");
    }
  } else if (currentMode == GO_STRAIGHT) {
    // Go straight
    analogWrite(e1, 80);  // Right motor speed
    analogWrite(e2, 80);  // Left motor speed
    digitalWrite(m1, LOW);   // Forward
    digitalWrite(m2, HIGH);
    digitalWrite(m3, LOW);   // Forward
    digitalWrite(m4, HIGH);
    Serial.println("Going Straight");

    // Check if distance threshold for going straight is reached
    if (distanceTraveled >= STRAIGHT_DISTANCE) {
      currentMode = LINE_FOLLOWING;
      hasGoneStraight = true; // Prevent further mode switches
      holes = 0; // Reset encoder count
      Serial.println("Switching to LINE_FOLLOWING mode (permanent)");
    }
  }
}

void count() {
  holes++;
