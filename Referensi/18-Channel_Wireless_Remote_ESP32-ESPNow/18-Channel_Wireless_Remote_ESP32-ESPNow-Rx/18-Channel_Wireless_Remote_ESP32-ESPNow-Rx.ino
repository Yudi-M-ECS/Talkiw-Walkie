// Receiver (Rx) Code
// 18-Channel Independent Wireless Remote Switch ESP32 - ESPNow
// Code by RBMK-5000 (c) 2023

#include <esp_now.h>
#include <WiFi.h>

#define TIMEOUT_TIMER 3000  // Resetting Data when no signal in 3 seconds

unsigned long LastDataReceived = 0;

struct PacketData
{
  byte switch1;
  byte switch2;
  byte switch3;
  byte switch4;  
  byte switch5;
  byte switch6;
  byte switch7;
  byte switch8; 
  byte switch9;
  byte switch10;
  byte switch11;
  byte switch12;
  byte switch13;
  byte switch14;
  byte switch15;
  byte switch16;
  byte switch17;
  byte switch18; 
};
PacketData receiverData;     

int   led1 = 12;
int   led2 = 4;
int   led3 = 5;
int   led4 = 13;
int   led5 = 14;
int   led6 = 15;
int   led7 = 16;
int   led8 = 17;
int   led9 = 18;
int   led10 = 19;
int   led11 = 21;
int   led12 = 22;
int   led13 = 23;
int   led14 = 25;
int   led15 = 26;
int   led16 = 27;
int   led17 = 32;
int   led18 = 33;

// Setting Initial/Default Receiver Switch Status = "LOW" Mode 
void SwitchDefaultValues() {
  receiverData.switch1 = LOW;
  receiverData.switch2 = LOW;
  receiverData.switch3 = LOW;
  receiverData.switch4 = LOW; 
  receiverData.switch5 = LOW;
  receiverData.switch6 = LOW;
  receiverData.switch7 = LOW;
  receiverData.switch8 = LOW;   
  receiverData.switch9 = LOW;
  receiverData.switch10 = LOW;
  receiverData.switch11 = LOW;
  receiverData.switch12 = LOW;  
  receiverData.switch13 = LOW;
  receiverData.switch14 = LOW;
  receiverData.switch15 = LOW; 
  receiverData.switch16 = LOW;
  receiverData.switch17 = LOW; 
  receiverData.switch18 = LOW; 
}

void SwitchWriteValues() {
  digitalWrite(led1, receiverData.switch1);
  digitalWrite(led2, receiverData.switch2);
  digitalWrite(led3, receiverData.switch3);
  digitalWrite(led4, receiverData.switch4); 
  digitalWrite(led5, receiverData.switch5);
  digitalWrite(led6, receiverData.switch6);
  digitalWrite(led7, receiverData.switch7);
  digitalWrite(led8, receiverData.switch8);  
  digitalWrite(led9, receiverData.switch9);
  digitalWrite(led10, receiverData.switch10);
  digitalWrite(led11, receiverData.switch11);
  digitalWrite(led12, receiverData.switch12);
  digitalWrite(led13, receiverData.switch13);  
  digitalWrite(led14, receiverData.switch14);
  digitalWrite(led15, receiverData.switch15);
  digitalWrite(led16, receiverData.switch16);
  digitalWrite(led17, receiverData.switch17);
  digitalWrite(led18, receiverData.switch18);   
}

// Callback Function when data received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len == 0)
  {
    return;
  }
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  SwitchWriteValues();  
  LastDataReceived = millis(); 
}


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
// Setting 18-Channel pinMode Output - i.e. using led1 to led18
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);      
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT); 
  pinMode(led9, OUTPUT);
  pinMode(led10, OUTPUT);
  pinMode(led11, OUTPUT);
  pinMode(led12, OUTPUT);
  pinMode(led13, OUTPUT);
  pinMode(led14, OUTPUT); 
  pinMode(led15, OUTPUT);
  pinMode(led16, OUTPUT);
  pinMode(led17, OUTPUT);
  pinMode(led18, OUTPUT);
  
  SwitchDefaultValues();
  SwitchWriteValues();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}
 

void loop() {
  // Checking Connection
  unsigned long Now = millis();
  if (Now - LastDataReceived > TIMEOUT_TIMER) 
  {
    SwitchDefaultValues();
    SwitchWriteValues();  
  }
}
