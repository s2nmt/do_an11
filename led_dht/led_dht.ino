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

#define FIREBASE_HOST "do-an-1-smart-iot-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyB9DcErDtkwtwGogeY3WDyJ27mAK3Uh8x8"

void configModeCallback (WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

FirebaseData firebaseData;
//WIFIClient client;
String path = "users/oP0SU44Wa4TQFnWoaomeIOGTrwm2/homes/474221e0-f454-11ed-ac68-d58cd55e9941/rooms/4ce35330-f454-11ed-ac68-d58cd55e9941/devices";
FirebaseJson json;
const int ledpin = D2;// khai báo chân
const int quat = D1;
//const int button = D1;//khai báo chân
//const int control = D4;
const int reply = D5;
unsigned long t1 = 0;
bool a;
//int q;
bool door;
//int ts;
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
  pinMode(quat, OUTPUT); //button ở chế độ input
  //pinMode(control, OUTPUT); //button ở chế độ input
  pinMode(reply, INPUT);
 // pinMode(read , INPUT);
  digitalWrite(ledpin, LOW);
  digitalWrite(quat, LOW);
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
  if (Firebase.getBool(firebaseData, path + "/a76d1ed0-f454-11ed-bd0e-21ddb9ceac77/state" )) a = firebaseData.boolData();
// Serial.println(c);
 if (a == true)
  {
    digitalWrite(ledpin, LOW);
    Serial.println("den sang");
  }
  else{
    digitalWrite(ledpin, HIGH);   
    Serial.println("den tat");

  }
  int c = digitalRead(reply);
  if (c == HIGH){ 
      Firebase.setBool(firebaseData,path + "/a88f30a0-f454-11ed-bd0e-21ddb9ceac77/state" ,true);
      Serial.println("Mo cua");
  }
  else{
      Firebase.setBool(firebaseData,path + "/a88f30a0-f454-11ed-bd0e-21ddb9ceac77/state" ,false);
      Serial.println("DONg cua");
  }
  

  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int t = dht.readTemperature();
  
//   Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor !");
    delay(500);
    return;
  }
  Serial.println(t);
  Firebase.setInt(firebaseData,path +"/ash12-may-lanh/temperature" ,t);
  Firebase.setInt(firebaseData,path +"/ash12-may-lanh/humidity" ,h);

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
