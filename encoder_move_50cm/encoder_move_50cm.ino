int motor1pin1 = 12;
int motor1pin2 = 11;
const int motor2Enable = 13;
const int motor1Enable = 10;
float holes = 0;
float distance =0 ;
int motor2pin1 = 9;
int motor2pin2 = 8;
const float HOLES_PER_REV   = 20.0;        // 20 holes per full revolution
const float WHEEL_DIAMETER  = 6.3;         // in cm (example)
const float WHEEL_CIRCUM    = 3.14159 * WHEEL_DIAMETER; // Circumference in cm
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(7 , INPUT);
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(7),count,FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  //control speed
  analogWrite(13, 100);  // Motor1 speed
  analogWrite(10, 100);  // Motor2 speed
  float distanceTraveled = (holes / HOLES_PER_REV) * WHEEL_CIRCUM;

  if(distanceTraveled <= 50){
   digitalWrite(motor1pin1, LOW);
   digitalWrite(motor1pin2,HIGH );

   digitalWrite(motor2pin1, LOW);
   digitalWrite(motor2pin2, HIGH);}
  else {
   digitalWrite(motor1pin1, LOW);
   digitalWrite(motor1pin2, LOW);

   digitalWrite(motor2pin1, LOW);
   digitalWrite(motor2pin2, LOW);
   delay(5000);
   holes =0; }
}
void count(){
  holes++;
}
