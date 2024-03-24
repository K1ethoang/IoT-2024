#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <LittleFS.h>

// Thông tin WiFi
const char* ssid = "Căn_Tin _UTC2";
const char* password = "123456789";

// Thiết lập web server trên port 80
AsyncWebServer server(80);

// Khai báo chân LED và nút nhấn
const int LED1 = D1;
const int LED2 = D2;
const int BUTTON1 = D5;
const int BUTTON2 = D6;

// Khai báo đối tượng DHT
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Biến trạng thái LED và giá trị nhiệt độ
int ledState1 = HIGH;
int ledState2 = LOW;
float temperature = 0;
float humidity = 0;

// Biến lưu trữ log
String logData = "";

void setup() {
  // Khởi tạo Serial
  Serial.begin(115200);

  // Kết nối WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Khởi tạo LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An error occurred while mounting LittleFS");
    return;
  }

  digitalWrite(LED1, ledState1);
  digitalWrite(LED2, ledState2);

  // Khởi tạo server
  server.begin();

  // Thiết lập chân LED và nút nhấn
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    sendHttpResponse(request);
  });

  // Serve images from LittleFS
  server.on("/bg.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/bg.svg", "image/svg+xml");
  });
  
  server.on("/logo_UTC2.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/logo_UTC2.jpg", "image/jpeg");
  });
  
  server.on("/logo_ddt.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/logo_ddt.jpg", "image/jpeg");
  });

  server.on("/banner.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/banner.png", "image/png");
  });
  
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/style.css", "text/css");
  });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/script.js", "text/javascript");
  });

  // Routes for LED control
//  server.on("/bat1", HTTP_GET, [](AsyncWebServerRequest *request) {
//      bat1();
//  });
//  
//  server.on("/tat1", HTTP_GET, [](AsyncWebServerRequest *request) {
//      tat1();
//  });
//  
//  server.on("/bat2", HTTP_GET, [](AsyncWebServerRequest *request) {
//      bat2();
//  });
//  
//  server.on("/tat2", HTTP_GET, [](AsyncWebServerRequest *request) {
//      tat2();
//  });
  
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"temperature\":" + String(temperature) + ",\"led1\":" + 
    String(ledState1) + ",\"led2\":" + String(ledState2) + 
    ",\"humidity\":" + String(humidity)+
    "}";
    request->send(200, "application/json", json);
  });

    // Khởi tạo DHT11
  dht.begin();
}

int btnState1;
int btnState2;

void loop() {
  // Đọc trạng thái nút nhấn
  btnState1 = digitalRead(BUTTON1);
  btnState2 = digitalRead(BUTTON2);
  
  // Đọc trạng thái nút nhấn và thiết lập trạng thái LED
  if (btnState1 == LOW) {
    ledState1 = LOW;
    digitalWrite(LED1, ledState1);
  }
  
  if (btnState2 == LOW) {
    ledState1 = HIGH;
    digitalWrite(LED1, ledState1);
    ledState2 = LOW;
    digitalWrite(LED2, ledState2);
  }

  // Đọc giá trị từ DHT11
  readDHTValues();

  // Cập nhật log
  updateLog();
  
  delay(300);
}
//
//// Bat led 1
//void bat1() {
//  ledState1 = HIGH;
//  digitalWrite(LED1, ledState1);
//}
//
//// Tat led 1
//void tat1() {
//  ledState1 = LOW;
//  digitalWrite(LED1, ledState1);
//}
//
//// Bat led 2
void bat2() {
  ledState2 = HIGH;
  digitalWrite(LED2, ledState2);
}
//
//// Tat led 2
//void tat2() {
//  ledState2 = LOW;
//  digitalWrite(LED2, ledState2);
//}

void readDHTValues() {
  // Đọc nhiệt độ và độ ẩm từ DHT11
//  int readData = dht.read(DHTPIN);
  float newTemperature = dht.readTemperature();
  float newHumidity = dht.readHumidity();

  // Kiểm tra nếu đọc thành công
  if (!isnan(newTemperature)) {
    if(newTemperature > 33)
    {
      bat2();
    }
    temperature = newTemperature;
  }
  if(!isnan(newHumidity))
  {
    humidity = newHumidity;
  }
} //isnan: not a number

void sendHttpResponse(AsyncWebServerRequest *request) {
  // Read the index.html file from flash
  String html = readFileFromFlash("/index.html");

  // Send the HTTP response
  request->send(200, "text/html", html);
}

String readFileFromFlash(String path) {
  String fileContent = "";
  File file = LittleFS.open(path, "r");
  if (file) {
    while (file.available()) {
      fileContent += (char)file.read();
    }
    file.close();
  } else {
    Serial.println("Error opening file: " + path);
  }
  return fileContent;
}

void updateLog() {
  // Cập nhật log với trạng thái LED và giá trị nhiệt độ
  String newLog = "LED1: " + String(ledState1) + ", LED2: " + String(ledState2) + ", Temperature: " + String(temperature) + "°C" + ", Humidity: " + String(humidity);
  logData = newLog + "\n";
  Serial.println(logData);
}
