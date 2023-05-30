TaskHandle_t Task1;
TaskHandle_t Task2;

#include  <WiFi.h>
#include <Arduino_JSON.h>
//#include <FirebaseESP32.h>
#include <string.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>                 // Khai báo thư viện Keypad

#define MODEM_RX 16
#define MODEM_TX 17
#define mySerial Serial2 // use for ESP32

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
//#define WIFI_SSID "Sieu Vip Pro"
//#define WIFI_PASSWORD "55557777"

//#define FIREBASE_HOST "htniot-default-rtdb.firebaseio.com/"
//#define FIREBASE_AUTH "y2a9wUQiGvkj1bw3oJ2cGPPrxVhgm0HfNjqz4iIr"

//den tx nau rx
//int relay = 13;
int lcdColumns = 20;
int lcdRows = 4;
const byte ROWS = 4; // Bốn hàng
const byte COLS = 4; // Ba cột
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {26, 25, 33, 32}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {13, 12, 14, 27};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

 
char STR[4] = {'2', '0', '1', '8'}; // Cài đặt mật khẩu tùy ý
char str[4] = {' ', ' ', ' ', ' '};
int i, j, count = 0;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
//FirebaseData firebaseData;
//WIFIClient client;
//String path = "PHONG_KHACH";
//FirebaseJson json;
int a = 0;
const int relay = 2;
const int button = 4;//khai báo chân

unsigned long t1 = 0;
unsigned long t2 = 0;

void setup() {
  Serial.begin(115200);
  pinMode(relay,OUTPUT);
  digitalWrite(relay, 0);
  pinMode(button, INPUT);   

  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
  lcd.init();                     
  lcd.backlight();
  
  xTaskCreatePinnedToCore(Task1code,"Task1",65536,NULL,1,&Task1,0);    
  vTaskDelay(100);                     
  xTaskCreatePinnedToCore(Task2code,"Task2",16384,NULL,10,&Task2,1);   
  vTaskDelay(100);        
}

void Task1code( void * parameter ){
  Serial.print("Task1 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
if (millis() - t2 > 100)
{ 

  int b = digitalRead(button);//đọc trạng thái button
  if (b == LOW) // nếu nút được nhấn
  {
      while (digitalRead(button) == LOW){ // chờ nút nhấn buông 
      delay(100);   
   } 
   a = !a;//đảo trạng thái led
   digitalWrite(relay, a);
   Serial.println(a);
   
   digitalWrite(relay, a); // ghi trạng thái của led ra chân ledpin
  }
  getFingerprintIDez();
  t2 = millis();
  vTaskDelay(100); 
}
vTaskDelay(100);   
  } 
}

void Task2code( void * parameter ){
  Serial.print("Task2 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    
  if (millis() - t1 > 100){   
    vTaskDelay(100); 
    lcd.setCursor(1, 0);
    lcd.print("Nhap Mat Khau"); 
    if (a == 1){
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Mo cua"); 
      digitalWrite(relay, 1);
      vTaskDelay(10000); 
      a = 0;
      digitalWrite(relay, 0);
    }
    
  char key = keypad.getKey(); // Ký tự nhập vào sẽ gán cho biến Key
  if (key) // Nhập mật khẩu
  {
    if (i == 0) {
      str[0] = key;
      
      Serial.print(str[0]);
      //delay(100); // Ký tự hiển thị trên màn hình LCD trong 1s
      vTaskDelay(100); 
      Serial.print("*"); // Ký tự được che bởi dấu *
      lcd.setCursor(0, 1);
      lcd.print("*"); 
    }
    if (i == 1) {
      str[1] = key;
  
      Serial.print(str[1]);
      //delay(100);
      vTaskDelay(100); 
      lcd.setCursor(1, 1);
      lcd.print("*"); 
      Serial.print("*");
    }
    if (i == 2) {
      str[2] = key;

      Serial.print(str[2]);
      //delay(100);
      vTaskDelay(100); 
      Serial.print("*");
      lcd.setCursor(2, 1);
      lcd.print("*"); 
    }
    if (i == 3) {
      str[3] = key;

      Serial.print(str[3]);
      //delay(100);
      vTaskDelay(100); 
      Serial.print("*");
      count = 1;
      lcd.setCursor(3, 1);
      lcd.print("*"); 
    }
    i = i + 1;
  }
 
  if (count == 1) {
    if (str[0] == STR[0] && str[1] == STR[1] && str[2] == STR[2] &&
        str[3] == STR[3]) {
      
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("Correct"); 
      
      Serial.print("Correct!");
      vTaskDelay(3000); 
      //delay(3000);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Mo cua"); 
      
      Serial.print("mo cua");

      Serial.print("      Opened!");
      a = 1;
      i = 0;
      count = 0;

    } else {     
      Serial.println("Incorrect!");
      delay(3000);
      
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("Try Again!"); 
      
      Serial.println("Try Again!");
      //delay(3000);
      vTaskDelay(3000); 

      Serial.println(" Enter Password");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(""); 
      //delay(1000);
      vTaskDelay(1000); 
      i = 0;
      count = 0;
    }
  }
 
  switch (key) {
  case '#':
    digitalWrite(relay,0);
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Closed"); 
      
    Serial.print("     Closed!");
    vTaskDelay(10000); 
    //delay(10000);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password"); 
    //delay(10000);
    vTaskDelay(5000); 
    Serial.print(" Enter Password");
    i = 0;
    break;
  }

  
         t1 = millis(); 
    }
    vTaskDelay(100);   
  }
}

void loop() {
  delay(100);
}
uint8_t getFingerprintIDez() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);    
  a = 1;
  return finger.fingerID;
}
