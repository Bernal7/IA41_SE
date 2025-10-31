const int buttonPin = 2;
const int buttonPin2 = 3;
const int ledPin = 13;

int ledState = HIGH;
int buttonState;
int buttonState2;
int lastButtonState = LOW;
int lastButtonState2 = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long lastDebounceTime2 = 0;
unsigned long debounceDelay2 = 50;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, ledState);
}

void loop() {
  int reading = digitalRead(buttonPin);
  int reading2 = digitalRead(buttonPin2);
  Serial.println(reading);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    if (reading != buttonState) {
      buttonState = reading;
      
            switch (buttonState2) {
        case 0: {
          if (buttonState == HIGH) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          break;
        }
        case 1: {
          if (buttonState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          break;
        }
      }
    }
  }
  
  if (reading2 != lastButtonState2) {
    lastDebounceTime2 = millis();
  }
  
  if ((millis() - lastDebounceTime2) > debounceDelay2) {

    if (reading2 != buttonState2) {
      buttonState2 = reading2;
      
            switch (buttonState2) {
        case 0: {
          if (buttonState == HIGH) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          break;
        }
        case 1: {
          if (buttonState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          break;
        }
      }
    }
  }
  
  digitalWrite(ledPin, ledState);

  lastButtonState = reading;
  lastButtonState2 = reading2;
}
