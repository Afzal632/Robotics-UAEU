#define m1 12   // Right Motor pin1
#define m2 11   // Right Motor pin2
#define m3 9    // Left Motor pin1
#define m4 8    // Left Motor pin2
#define e1 10   // Right Motor Enable
#define e2 13   // Left Motor Enable

#define ir1 A5
#define ir2 A4
#define ir3 A3
#define ir4 A2
#define ir5 A1

#define HISTORY_SIZE 5
int s1_history[HISTORY_SIZE] = {1, 1, 1, 1, 1};
int s2_history[HISTORY_SIZE] = {1, 1, 1, 1, 1};
int s3_history[HISTORY_SIZE] = {1, 1, 1, 1, 1};
int s4_history[HISTORY_SIZE] = {1, 1, 1, 1, 1};
int s5_history[HISTORY_SIZE] = {1, 1, 1, 1, 1};

float line_angle = 0.0;
int base_speed = 80;
float correction_factor = 0.0;
#define MAX_SPEED 100
#define MIN_SPEED 30  // Lowered for finer control on gentle turns

void setup() {
  Serial.begin(9600);
  
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  pinMode(e1, OUTPUT);
  pinMode(e2, OUTPUT);
  
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(ir5, INPUT);
}

void loop() {
  static unsigned long last_update = 0;
  const unsigned long update_interval = 10;
  if(millis() - last_update < update_interval) return;
  last_update = millis();

  // Shift historical readings
  for(int i = HISTORY_SIZE-1; i > 0; i--) {
    s1_history[i] = s1_history[i-1];
    s2_history[i] = s2_history[i-1];
    s3_history[i] = s3_history[i-1];
    s4_history[i] = s4_history[i-1];
    s5_history[i] = s5_history[i-1];
  }
  
  // Read current values
  s1_history[0] = digitalRead(ir1);
  s2_history[0] = digitalRead(ir2);
  s3_history[0] = digitalRead(ir3);
  s4_history[0] = digitalRead(ir4);
  s5_history[0] = digitalRead(ir5);
  
  int s1 = s1_history[0], s2 = s2_history[0], s3 = s3_history[0], s4 = s4_history[0], s5 = s5_history[0];
  
  // Quick straight case
  if(s3 == 0 && s1 == 1 && s2 == 1 && s4 == 1 && s5 == 1) {
    setMotorSpeeds(MAX_SPEED, MAX_SPEED, true, true);
    Serial.print("F");
    return;
  }
  
  // Handle special cases (sharp turns and line loss)
  if(handleSpecialCases()) return;
  
  // Calculate line trajectory and angle
  line_angle = calculateLineAngle();
  
  // PID controller for smoother steering
  static float last_error = 0;
  static float integral = 0;
  float Kp = 5.0, Ki = 0.05, Kd = 8.0; // Adjusted for gentle turns
  integral += line_angle;
  // Limit integral to prevent windup
  integral = constrain(integral, -50, 50);
  float derivative = line_angle - last_error;
  correction_factor = Kp * line_angle + Ki * integral + Kd * derivative;
  last_error = line_angle;
  
  // Non-linear correction for gentle turns
  float abs_correction = abs(correction_factor);
  float adjusted_correction = abs_correction * abs_correction / 50.0; // Quadratic scaling for smaller adjustments
  if(correction_factor < 0) adjusted_correction = -adjusted_correction;
  
  // Apply speed corrections with dynamic base speed
  int adjusted_base_speed = base_speed - (abs(line_angle) / 3);
  if(line_angle > 0) {
    setMotorSpeeds(MAX_SPEED, max(MIN_SPEED, adjusted_base_speed - adjusted_correction), true, true);
    Serial.print("R");
  } else if(line_angle < 0) {
    setMotorSpeeds(max(MIN_SPEED, adjusted_base_speed + adjusted_correction), MAX_SPEED, true, true);
    Serial.print("L");
  } else {
    setMotorSpeeds(MAX_SPEED, MAX_SPEED, true, true);
    Serial.print("F");
  }

  // Debug output for tuning
  Serial.print(" A:");
  Serial.print(line_angle);
  Serial.print(" C:");
  Serial.println(adjusted_correction);
}

float calculateLineAngle() {
  int sensor_weights[] = {-2, -1, 0, 1, 2};
  float weighted_sum = 0;
  int active_sensors = 0;
  
  if(s1_history[0] == 0) {
    weighted_sum += sensor_weights[0] * 1.0;
    active_sensors++;
  }
  if(s2_history[0] == 0) {
    weighted_sum += sensor_weights[1] * 1.0;
    active_sensors++;
  }
  if(s3_history[0] == 0) {
    weighted_sum += sensor_weights[2] * 1.0;
    active_sensors++;
  }
  if(s4_history[0] == 0) {
    weighted_sum += sensor_weights[3] * 1.0;
    active_sensors++;
  }
  if(s5_history[0] == 0) {
    weighted_sum += sensor_weights[4] * 1.0;
    active_sensors++;
  }
  
  for(int i = 1; i < HISTORY_SIZE; i++) {
    float time_weight = 0.5 - (i * 0.2);
    if(s1_history[i] == 0) weighted_sum += sensor_weights[0] * time_weight;
    if(s2_history[i] == 0) weighted_sum += sensor_weights[1] * time_weight;
    if(s3_history[i] == 0) weighted_sum += sensor_weights[2] * time_weight;
    if(s4_history[i] == 0) weighted_sum += sensor_weights[3] * time_weight;
    if(s5_history[i] == 0) weighted_sum += sensor_weights[4] * time_weight;
    if(s1_history[i] == 0 || s2_history[i] == 0 || s3_history[i] == 0 || 
       s4_history[i] == 0 || s5_history[i] == 0) {
      active_sensors++;
    }
  }
  
  if(active_sensors > 0) {
    return (weighted_sum / active_sensors) * 20.0; // Reduced scaling for finer control
  }
  return 0.0;
}

bool handleSpecialCases() {
  int s1 = s1_history[0], s2 = s2_history[0], s3 = s3_history[0], s4 = s4_history[0], s5 = s5_history[0];
  
  // Sharp left turn
  if((s3 == 0 && s4 == 0 && s5 == 0) && (s1 == 1)) {
    setMotorSpeeds(70, MAX_SPEED, false, true);
    Serial.println("<");
    return true;
  }
  
  // Sharp right turn
  if((s1 == 0 && s2 == 0 && s3 == 0) && (s5 == 1)) {
    setMotorSpeeds(MAX_SPEED, 70, true, false);
    Serial.println(">");
    return true;
  }
  
  // Line lost - use history for recovery
  if(s1 == 1 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 1) {
    int left_sum = 0, right_sum = 0;
    for(int i = 0; i < HISTORY_SIZE; i++) {
      left_sum += (s1_history[i] == 0 || s2_history[i] == 0) ? 1 : 0;
      right_sum += (s4_history[i] == 0 || s5_history[i] == 0) ? 1 : 0;
    }
    
    if(left_sum > right_sum) {
      setMotorSpeeds(60, MAX_SPEED, true, true);
      Serial.println("?L");
    } else if(right_sum > left_sum) {
      setMotorSpeeds(MAX_SPEED, 60, true, true);
      Serial.println("?R");
    } else {
      static byte search_dir = 0;
      if(search_dir++ % 2 == 0) {
        setMotorSpeeds(80, 60, true, true);
      } else {
        setMotorSpeeds(60, 80, true, true);
      }
      Serial.println("?");
    }
    return true;
  }
  
  return false;
}

void setMotorSpeeds(int right_speed, int left_speed, bool right_forward, bool left_forward) {
  analogWrite(e1, right_speed);
  analogWrite(e2, left_speed);
  digitalWrite(m1, right_forward ? LOW : HIGH);
  digitalWrite(m2, right_forward ? HIGH : LOW);
  digitalWrite(m3, left_forward ? LOW : HIGH);
  digitalWrite(m4, left_forward ? HIGH : LOW);
}