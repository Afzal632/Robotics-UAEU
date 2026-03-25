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

// PID variables
float Kp = 25;
float Ki = 0;
float Kd = 15;
float lastError = 0;
float integral = 0;

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
}

void loop() {
  // IR sensor readings
  double s1 = digitalRead(ir1);  // Left Most
  double s2 = digitalRead(ir2);  // Left
  double s3 = digitalRead(ir3);  // Center
  double s4 = digitalRead(ir4);  // Right
  double s5 = digitalRead(ir5);  // Right Most
  double sum_sensor = s1 + s2 + s3 + s4 + s5;

  double sensor_line = 0;
  if (sum_sensor != 0) {
    sensor_line = (s1*0 + s2*1 + s3*2 + s4*3 + s5*4) / sum_sensor;
  }

  Serial.print("Sensor Sum: ");
  Serial.println(sum_sensor);
  Serial.print("Line Position: ");
  Serial.println(sensor_line);

  // Fallback if no line is detected
  if (sum_sensor == 0) {
    digitalWrite(m1, HIGH); digitalWrite(m2, HIGH);  // Stop right motor
    digitalWrite(m3, HIGH); digitalWrite(m4, HIGH);  // Stop left motor
    analogWrite(e1, 0);
    analogWrite(e2, 0);
    Serial.println("Stopped - No Line Detected");
    return;
  }

  // PID Control Logic
  float error = 2.0 - sensor_line; // Ideal line position is at center (index 2)
  integral += error;
  float derivative = error - lastError;
  float correction = -(Kp * error + Ki * integral + Kd * derivative);
  lastError = error;

  int baseSpeed = 70;
  int rightSpeed = baseSpeed + correction;
  int leftSpeed = baseSpeed - correction;

  // Clamp values to 0-255
  rightSpeed = constrain(rightSpeed, 0, 255);
  leftSpeed = constrain(leftSpeed, 0, 255);

  // Apply motor speeds
  analogWrite(e1, rightSpeed);
  analogWrite(e2, leftSpeed);

  // Always move forward
  digitalWrite(m1, LOW);  // Right motor forward
  digitalWrite(m2, HIGH);
  digitalWrite(m3, LOW);  // Left motor forward
  digitalWrite(m4, HIGH);

  Serial.print("Correction: "); Serial.println(correction);
  Serial.print("Left Speed: "); Serial.print(leftSpeed);
  Serial.print(" | Right Speed: "); Serial.println(rightSpeed);

  delay(10); // Optional smoothing
}
