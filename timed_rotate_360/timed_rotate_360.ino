int motor1pin1 = 12;
int motor1pin2 = 11;
const int motor2Enable = 13;
const int motor1Enable = 10;

int motor2pin1 = 9;
int motor2pin2 = 8;

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
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(13, 100);  // Motor1 speed
  analogWrite(10, 100);  // Motor2 speed

  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);

  delay(1640);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
  delay(5000);

}
