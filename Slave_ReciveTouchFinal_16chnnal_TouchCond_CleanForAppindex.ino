//Libs for espnow e wifi
#include <esp_now.h>
#include <WiFi.h>
#include <Servo.h>

/////////////////////////// PWM Servo
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
uint8_t servonum = 0;
int pulse [9];
////////////////////////////////

int servosNum = 9;
int dataNum = 9;
int inc = 7; // incrment increases for servo pos
int servoMin = 0; // min degree pos
int servoMax = 180; // max degree pos
int servoPos [9] = {}; // servo position for each one it should = to data number coming and = to number of servo
int delayGet = 5; // delay to get the new data

void setup() {

  ////////////////PWM Servo
  Wire.begin(33,32); //sda scl
  ////////////////
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("Mac Address in Station: "); 
  Serial.println(WiFi.macAddress());
  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);

////////////////PWM Servo
    pwm.begin();
    pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
    yield();
////////////////
  for(int i=0; i<dataNum; i++){
    pwm.setPWM(i, 0, SERVOMIN);
  }
}

void InitESPNow() {
  //If the initialization was successful
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  //If there was an error
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}


//Callback function that tells us when data from Master is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  //Copies the sender Mac Address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Prints it on Serial Monitor
  //Serial.print("Received from: "); 
  //Serial.println(macStr);
  //Serial.println("");
    
    for(int i=0; i<dataNum; i++){
      if (data[i] == 1) servoPos [i] += inc;
      if (data[i] == 0) servoPos [i] = servoMin;
      if (servoPos [i] > servoMax) servoPos [i] = servoMax;
     }

  for(int i=0; i<dataNum; i++){
    pulse [i] = map(servoPos [i],0, 180, SERVOMIN,SERVOMAX);
    pwm.setPWM(i, 0, pulse [i]);
  }
  

 delay (delayGet);
}

//We don't do anything on the loop.
//Everytime something comes from Master
//the OnDataRecv function is executed automatically
//because we added it as callback using esp_now_register_recv_cb
void loop() {
}
