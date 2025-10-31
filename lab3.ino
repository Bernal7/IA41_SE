void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int pullDetection = digitalRead(3);
  int buttonState = digitalRead(2);
  Serial.print("Pull ?: ");
  Serial.print(pullDetection);
  Serial.print(" Boton Estado? :");
  Serial.print(pullDetection);
  Serial.print("\n");
