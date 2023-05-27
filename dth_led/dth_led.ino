#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h" //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>
#include "FirebaseESP8266.h"
#include "DHT.h"

#define DHTTYPE DHT11 // type of the temperature sensor
const int DHTPin = D3; //--> The pin used for the DHT11 sensor is Pin D1 = GPIO5
DHT dht(DHTPin, DHTTYPE); //--> Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor);

#define FIREBASE_HOST "htniot-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "y2a9wUQiGvkj1bw3oJ2cGPPrxVhgm0HfNjqz4iIr"

void configModeCallback (WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

FirebaseData firebaseData;
//WIFIClient client;
String path = "PHONG_KHACH";
FirebaseJson json;
const int ledpin = D2;// khai báo chân
const int button = D1;//khai báo chân
//const int control = D4;
const int reply = D5;
unsigned long t1 = 0;
int a;
int door;
int ts;
void setup()
{
  Serial.begin(115200);
  //Khai bao wifiManager
  WiFiManager wifiManager;
  //Setup callback de khoi dong AP voi SSID "ESP+chipID"
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    //Neu ket noi that bai thi reset
    ESP.reset();
    delay(1000);
  }
  // Thanh cong thi bao ra man hinh
  Serial.println("connected...");

  dht.begin();  //--> Start reading DHT11 sensors
  delay(500);
  pinMode(ledpin, OUTPUT); //ledpin ở chế độ output
  pinMode(button, INPUT); //button ở chế độ input
  //pinMode(control, OUTPUT); //button ở chế độ input
  pinMode(reply, INPUT);
 // pinMode(read , INPUT);
  digitalWrite(ledpin, LOW);
//  digitalWrite(control, LOW);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
}
void loop()
{
  if (Firebase.getInt(firebaseData, path + "/DEN" )) a = firebaseData.intData();
// Serial.println(c);
 if (a == 1)
  {
    digitalWrite(ledpin, HIGH);
    Serial.println("den sang");
  }
  else{
    digitalWrite(ledpin, LOW);   
    Serial.println("den tat");

  }
//  if (Firebase.getInt(firebaseData,"/DOOR" )) door = firebaseData.intData();
//// Serial.println(c);
// if (door == 1)
//  {
//    digitalWrite(control, HIGH);
//    Serial.println("Mo cua");
//    delay(100);
//  }
//  else{
//    digitalWrite(control, LOW);   
//    Serial.println("Dong cua");
//    delay(100);
//  }
  
  int c = digitalRead(reply);
  if (c == HIGH){ 
    if(ts == 0){
      Firebase.setInt(firebaseData,"/DOOR" ,1);
      ts = 1;
    }
  }
  else{
    if(ts == 1){
      Firebase.setInt(firebaseData,"/DOOR" ,0);
      ts = 0;
    }
  }
  
  int b = digitalRead(button);//đọc trạng thái button
  if (b == LOW) // nếu nút được nhấn
  {
      while (digitalRead(button) == LOW){ // chờ nút nhấn buông 
        delay(100);
   } 
   a = !a;//đảo trạng thái led
   Firebase.setInt(firebaseData, path + "/DEN" ,a);
   digitalWrite(ledpin, a); // ghi trạng thái của led ra chân ledpin
  }

  

  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
//   Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor !");
    delay(500);
    return;
  }
  Serial.println(t);
  Firebase.setInt(firebaseData,"/NHIET_DO" ,t);
  Firebase.setInt(firebaseData,"/DO_AM" ,h);

}

//#include <ESP8266WiFi.h>
//#include <WiFiManager.h>
//
//void setup() {
//  Serial.begin(115200);
//  delay(1000);
//  
//  // Khởi tạo đối tượng WiFiManager
//  WiFiManager wifiManager;
//  
//  // Xóa thông tin kết nối Wi-Fi cũ
//  wifiManager.resetSettings();
//  
//  Serial.println("Wi-Fi settings cleared!");
//  
//  // Tắt nguồn hoặc thực hiện các hành động khác (nếu cần)
//}
//
//void loop() {
//  // Không cần thực hiện gì trong hàm loop
//}
