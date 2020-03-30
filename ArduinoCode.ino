#include <SoftwareSerial.h>
#include <TimerOne.h>

volatile int i=0; 
volatile boolean zero_cross=0;
int AC_pin = 3;
int dim = 0;
int freqStep = 75;    // mb 65

#define DEBUG true

SoftwareSerial esp8266(9,10); // rt to tx and vice versa
int outVal = 0;
void setup()
{ 
  Serial.begin(9600);
  esp8266.begin(9600);
   
  sendData("AT+RST\r\n",2000,DEBUG);
  sendData("AT+CWMODE=2\r\n",1000,DEBUG);
  sendData("AT+CIFSR\r\n",1000,DEBUG);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); 

  pinMode(AC_pin, OUTPUT);                         
  attachInterrupt(0, zero_cross_detect, RISING);
  Timer1.initialize(freqStep);         
  Timer1.attachInterrupt(dim_check, freqStep);
}
 
void loop()
{
  if(esp8266.available())
  {
    if(esp8266.find("+IPD,"))
    {
     delay(1000);
     int connectionId = esp8266.read()-48; 
     esp8266.find("pin=");
    
     int pinValue = 0;
     
     int firstNumber = (esp8266.read()-48); 
     Serial.print("First value is: ");
     Serial.println(firstNumber);
     int secondNumber = (esp8266.read()-48); 
     Serial.print("Second value is: ");
     Serial.println(secondNumber);
     int thirdNumber = (esp8266.read()-48);
     Serial.print("Third value is: ");
     Serial.println(thirdNumber);

     if (check(firstNumber)) {
      pinValue += firstNumber;
      if (check(secondNumber)) {
        pinValue *=10;
        pinValue += secondNumber;
        if (check(thirdNumber)) {
          pinValue *= 10;
          pinValue += thirdNumber;
        }
      }
     }
     
     Serial.print("Result is: ");
     Serial.println(pinValue);
     
     analogWrite(11, pinValue); 
     dim = map(pinValue, 0, 255, 128, 0);
     
     
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId;
     closeCommand+="\r\n";
     
     sendData(closeCommand,1000,DEBUG);
    }
  }
}
 

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); 
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        char c = esp8266.read(); 
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}

bool check(int number) {
  return number >=0 && number <=9;
}

void zero_cross_detect() {    
  zero_cross = true;               
  i=0;
  digitalWrite(AC_pin, LOW);      
}                                 
 
void dim_check() {                   
  if(zero_cross == true) {              
    if(i>=dim) {                     
      digitalWrite(AC_pin, HIGH); 
      i=0;        
      zero_cross = false;
    } 
    else {
      i++;          
    }                                
  }                                  
}
