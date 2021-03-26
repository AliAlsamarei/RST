/*
   Touch Sensor Pin Layout
   T0 = GPIO4
   T1 = GPIO0
   T2 = GPIO2
   T3 = GPIO15
   T4 = GPIO13
   T5 = GPIO12
   T6 = GPIO14
   T7 = GPIO27
   T8 = GPIO33
   T9 = GPIO32 
   touchRead(T0)*/
   
//Libs for espnow and wifi
#include <esp_now.h>
#include <WiFi.h>

//Channel used in the connection
#define CHANNEL 1

//int analogPin01 = 34;
//int analogPin02 = 35;

int sensorPoints = 9;
int incomingValues[9] = {0,0,0,0,0,0,0,0,0};   
uint8_t macSlaves[][6] = {0x3C, 0x71, 0xBF, 0x03, 0x32, 0x54}; // send to a spesific esp32 or device by MAC adress

void setup() {
  Serial.begin(115200);
  WiFi.disconnect(true);
  delay(10);

  WiFi.mode(WIFI_STA);
  Serial.print("Mac Address in Station: "); 
  Serial.println(WiFi.macAddress());
  InitESPNow();
  int slavesCount = sizeof(macSlaves)/6/sizeof(uint8_t); 
  for(int i=0; i<slavesCount; i++){
    esp_now_peer_info_t slave;
    slave.channel = CHANNEL;
    slave.encrypt = 0;
    memcpy(slave.peer_addr, macSlaves[i], sizeof(macSlaves[i]));
    esp_now_add_peer(&slave);
  }
  
//Registers the callback that will give us feedback about the sent data
  //The function that will be executed is called OnDataSent
  esp_now_register_send_cb(OnDataSent);
      
  
  
//  Calls the send function
  Serial.println ("start send");
  send();

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

void send(){


  int TVR = 9; // how many sensor input
  int CR = 5; // how many raws to fill
  uint8_t values [TVR] = {0,0,0,0,0,0,0,0,0}; 

  long fillArray [CR][TVR]={
      {0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0}
    };

  
  for (int i=0; i<CR; i++){
      fillArray [i][0] = touchRead(T0);
      fillArray [i][1] = touchRead(T2);  
      fillArray [i][2] = touchRead(T3);
      fillArray [i][3] = touchRead(T4); 
      fillArray [i][4] = touchRead(T5);
      fillArray [i][5] = touchRead(T6); 
      fillArray [i][6] = touchRead(T7); 
      fillArray [i][7] = touchRead(T8); // analogRead(34);  //analogRead(analogPin01);
      fillArray [i][8] = touchRead(T9); 
  }


  
  for (int j=0; j<TVR; j++){
    for (int i=0; i<CR; i++){
      if (fillArray [0][j] < 10 && fillArray [1][j] < 10 && fillArray [2][j] < 10 && fillArray [3][j] < 10 && fillArray [4][j] < 10  
//      && fillArray [5][j] < 5  && fillArray [6][j] < 5 && fillArray [7][j] < 5 && fillArray [8][j] < 5 && fillArray [9][j] < 5 
//      && fillArray [10][j] < 5 && fillArray [11][j] < 5 && fillArray [12][j] < 5 && fillArray [13][j] < 5 && fillArray [14][j] < 5
      ){
        values [j] = 1;
      }
      else {
      values [j] = 0;
      }
    }
  }

/////////////for cheaking the data out//////////////
//   for (int i = 0; i <CR; i++) {
//       for (int j = 0; j < TVR; j++) {
//           Serial.print("  ");Serial.print(fillArray[i][j], DEC);
//       }
//       Serial.println();
//   }
//  Serial.println("=========");
//
//
  for(int i=0; i<TVR; i++){
    Serial.print(values[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("=========");


//  for(int i=0; i<TVR; i++){
//    Serial.print(tchVal[i]);
//    Serial.print(" ");
//  }
//  Serial.println();
//  Serial.println("=========");
////////////////////////////////////////////////


  uint8_t macAddr[] = {0x3C, 0x71, 0xBF, 0x03, 0x32, 0x54};
  esp_err_t result = esp_now_send(macAddr, (uint8_t*) &values, sizeof(values));

  Serial.print("Send Status: ");
  //If it was successful
  if (result == ESP_OK) {
    Serial.println("Success");
  }
  //if it failed
  else {
    Serial.println("Error");
  }

  
}

//Callback function that gives us feedback about the sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  //Copies the receiver Mac Address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Prints it on Serial Monitor
  Serial.print("Sent to: "); 
  Serial.println(macStr);
  //Prints if it was successful or not
  Serial.print("Status: "); 
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  //Sends again
  delay (10);
  send();
}


//We don't do anything on the loop.
//Every time we receive feedback about the last sent data,
//we'll be calling the send function again,
//therefore the data is always being sent

void loop() {
}
