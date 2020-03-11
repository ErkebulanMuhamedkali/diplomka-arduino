
int buttonPin = 0;

void setup() {
  
}


void loop() {
  for(int i=0; i<255; i++){
  analogWrite(3, i);
  delay(10);
 
  }
}
