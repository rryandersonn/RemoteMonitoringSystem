//Ryan Anderson
//Created with Inspiriation from Rui Santos 
//https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/
//GlobalFoundries FANSTAT - Fan Analytics and Notification System for Telemetry and Assessment

//Code that needs to be edited will be marked with //****EDIT HERE****//
//User will need to change the tool ID to identify where the data is coming from
//User will need to change the amount of fans to match how many are in the FFU
//User will need to change the MAC address of the Proxy this collector will connect to 
//User will need to change the name of the Tool ID

//Uploading Directions 
//Go to -> Tools -> Board -> esp32 -> XIAO_ESP32C3; ensure XIAO_ESP32C3 stays
//Go to -> Files -> Preferences -> Additional Boards manager URLs: -> PASTE THIS URL -> https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json


//Collector Code

//Libraries
#include <esp_now.h>
#include <WiFi.h>

//Include variables
volatile unsigned int count = 0;
volatile unsigned int checkfan = 1;
volatile unsigned int dpin = 2;
volatile unsigned int lastdpin = 1;
unsigned long next = 2500;
unsigned int temp = 0;
unsigned int rotations = 0;

//*****************************EDIT HERE**************************//
// change the tool ID here
char toolID[] = "TESTA";
//*****************************EDIT HERE**************************//
// change the number of fans to match how many are in the FFU
int fanamount = 1;
//*****************************EDIT HERE**************************//
// REPLACE WITH YOUR PROXY MAC Address
// 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
// Replace FF with each mac address value
uint8_t broadcastAddress[] = {0x48, 0x31, 0xB7, 0x3F, 0xA8, 0xE4};
//*****************************EDIT HERE**************************//

// Structure must match the proxy structure
typedef struct struct_message {
  char ffu[127];
  int rpm;
  int fan;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// add to count
void magnet_detect() 
{
  count++;
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatus: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Data Delivered Successfully to Proxy" : "Data Delivery Failed to send to Proxy");
}
 
void setup() {
  // Initalize Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

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

}
 
void switchPin(){
         // create inturrupt
  attachInterrupt(digitalPinToInterrupt(dpin), magnet_detect, RISING);//inturrupt current fan pin

}

void nothing(){
}

void loop() {

  // Set values to send
    // notify changed value
    if (millis()>next) { //check if new time is > old time
    switchPin();
    temp = count;
    rotations = temp * 24;
    Serial.print("RPM: ");
    Serial.println(rotations);
    next += 2500; //count every 2.5 seconds
    if (checkfan >= fanamount) {
     checkfan = 1;
     count -=temp;
     }
    else {
    checkfan++;
    count -=temp;
    }
    dpin = checkfan + 1;
    lastdpin = dpin - 1;


  strcpy(myData.ffu, toolID);
  myData.fan = int(checkfan);
  myData.rpm = int(rotations);

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  // Check conditions of sender
  if (result == ESP_OK) {
    Serial.println("ToolID: " + String(toolID));
    Serial.println("Fan " + String(checkfan) + " from pin D" + String(checkfan - 1) + ": Sent Successfully");
    attachInterrupt(digitalPinToInterrupt(dpin), nothing, RISING);//kill inturrupt
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);

}

}