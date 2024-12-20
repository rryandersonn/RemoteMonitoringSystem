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
const char* diagnosis = "Operational";

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    char ffu[127]; //specify which ffu
    int rpm; //rpm data
    int fan; //specify which fan
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  int RPM = myData.rpm;
  int FAN = myData.fan;

  //Diagnosis
  const char* diagnosis = "Operational";
  if (RPM < 1){
    diagnosis = "OFF";
  }
  //if (RPM < 1 && current > .05){
    //diagnosis = "Siezed";
  //}

  Serial.println("FFU: " + String(myData.ffu) + 
                 ", FAN: " + String(FAN) + 
                 ", RPM: " + String(RPM) + 
                 ", CURRENT: " + "Current" + 
                 ", DIAGNOSIS: " + String(diagnosis));
  
  //tester code
  //Serial.println("***Test line*** RPM = Zero: " + String(counterzero) + "Times");
  //Serial.println("***Test line*** RPM > 1000: " + String(counter1k) + "Times");
  //Serial.println(); 
  // if (RPM < 1){
  //   counterzero++;
  // }
  // if (RPM < 1000 && RPM > 0){
  //   counter1k++;
  // }

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

  // Once ESPNow is successfully connected, data will be sent through Serial
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {

}