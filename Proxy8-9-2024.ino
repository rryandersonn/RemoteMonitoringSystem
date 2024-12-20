//Ryan Anderson
//Created with Inspiriation from Rui Santos 
//https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/
//GlobalFoundries FANSTAT - Fan Analytics and Notification System for Telemetry and Assessment

//Uploading Directions 
//Go to -> Tools -> Board -> esp32 -> XIAO_ESP32C3; ensure XIAO_ESP32C3 stays
//Go to -> Files -> Preferences -> Additional Boards manager URLs: -> PASTE THIS URL -> https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json

//Proxy Code

//Libraries
#include <esp_now.h>
#include <WiFi.h>

//testing code
int counterzero = 0;
int counter1k = 0;

//*****************************EDIT HERE**************************//
// REPLACE WITH YOUR PROXY MAC Address
// 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
// Replace FF with each mac address value
uint8_t broadcastAddress[] = {0x48, 0x31, 0xB7, 0x3F, 0x93, 0x50};
// Aisle Location for this Proxy:
// Aisle Location for next Proxy:
//*****************************EDIT HERE**************************//

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    char ffu[127]; //specify which ffu
    int rpm; //rpm data
    int fan; //specify which fan
} struct_message;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// Callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  //**Recieve Data**//
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  //char ffuid[] = String(myData.ffu)
  char const* FFU = incomingReadings.ffu;
  int RPM = incomingReadings.rpm;
  int FAN = incomingReadings.fan;
  //**End of Recieve Data**//

  //**Data to Next Proxy**//
  //  Define data structure
  strcpy(myData.ffu, FFU);
  myData.fan = int(FAN);
  myData.rpm = int(RPM);

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  // Check conditions of sender
  if (result == ESP_OK) {
    Serial.println("ToolID: " + String(FFU));
    Serial.println("Fan " + String(FAN) + " from pin D" + String(FAN - 1) + ": Sent Successfully");

  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
  //**Exit Data Send**//

  //print values
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println(String(incomingReadings.ffu) + ": ");
  Serial.print("Fan #" + String(FAN) + ": ");
  Serial.println(String(RPM) + "RPM");
  
  //tester code
  Serial.println("***Test line*** RPM = Zero: " + String(counterzero) + "Times");
  Serial.println("***Test line*** RPM > 1000: " + String(counter1k) + "Times");
  Serial.println();
  if (RPM < 1){
    counterzero++;
  }
  if (RPM < 1000 && RPM > 0){
    counter1k++;
  }

}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatus: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Data Delivered Successfully to Proxy" : "Data Delivery Failed to send to Proxy");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Connected to ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return; }

    // Once ESPNow is successfully initalized, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  // Once ESPNow is successfully connected, data will be sent through Serial
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}


void loop() {

}