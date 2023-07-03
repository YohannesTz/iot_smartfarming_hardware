/*
 *  This sketch sends data to server from sensors.
 *  not an arduino code. ESP32 instead in the boards menu
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "your Wifi name here";
const char* password = "your password";

int soilMoisturePin = 36;
int relayControlPin = 19;
int signalArduinoPin = 5;

//Link to backend server check https://github.com/YohannesTz/iot_smart_backend/
String host = "https://your backend code url/sendData.php";
const float calibrationCurve[2] = { 0.3, 2.7 }; // Calibration curve for YL-69 sensor
const long intervalRelay = 2500; //Delay for pump: 2.5sec
const long intervalHTTP = 50000; // Delay for HTTP: 50sec

/*
 * we have one CPU with one clock. so if we need to run simultaneous parallel tasks running in one loop, we need 
 * to calculate each of their diffence by saving the previous milli second time. so if we match the difference 
 * we can run each task accordingly.
 */
unsigned long previousMillisRelay = 0;
unsigned long previousMillisHTTP = 0;
boolean isPumpOn = false;

void setup()
{
    Serial.begin(9600);
    delay(10);

    //Setting pins correctly
    pinMode(soilMoisturePin, INPUT);
    pinMode(relayControlPin, OUTPUT);
    //to send signal to arduino from ESP32 to set the LCD display text.
    pinMode(signalArduinoPin, OUTPUT);

    // We start by connecting to a WiFi network
    Serial.println();
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
    dht.begin();
}


void loop()
{
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int soilMoistureValue = analogRead(soilMoisturePin);

    unsigned long currentMillis = millis(); // Get the current time
    if (currentMillis - previousMillisRelay >= intervalRelay) { // Check if the interval has passed
      previousMillisRelay = currentMillis; // Update the previous time

      if (soilMoistureValue <= 2500) {
        digitalWrite(relayControlPin, HIGH);
        digitalWrite(signalArduinoPin, HIGH);
        Serial.print("Relay Write: ");
        Serial.println(digitalRead(relayControlPin));
        Serial.println("Signal Pin: 1");
        isPumpOn = true;
      } else {
        digitalWrite(relayControlPin, LOW);
        Serial.print("Relay Write: ");
        digitalWrite(signalArduinoPin, LOW);
        Serial.println(digitalRead(relayControlPin));
        Serial.println("Signal Pin: 0");
        isPumpOn = false;
      } 

      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" °C, Humidity: ");
      Serial.print(humidity);
      Serial.print(" %, Soil Moisture: ");
      Serial.println(soilMoistureValue);
    } 
    if (currentMillis - previousMillisHTTP >= intervalHTTP) {
      previousMillisHTTP = currentMillis; // Update the previous time
      
      Serial.print("connecting to ");
      Serial.println(host);
  
      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      const int httpPort = 80;
  
      HTTPClient http;
      http.begin(host.c_str());
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String httpRequestData;
      // Turns out casting stuff is not the best Idea in c/Arduino. hence, this shitty code.
      if (isPumpOn) {
        httpRequestData = "hardwareId=abc&temp=" + String(temperature) + "&humidity=" + String(humidity) + "&pumpstatus=true";
      } else {
        httpRequestData = "hardwareId=abc&temp=" + String(temperature) + "&humidity=" + String(humidity) + "&pumpstatus=false";
      }

      Serial.println(httpRequestData);

      //recieve the HttpResponse code
      int hRc = http.POST(httpRequestData);
      
      if (hRc > 0) {
        Serial.print("Http Response code: ");
        Serial.println(hRc);
        String payload = http.getString();
        Serial.println(payload);
      } else {
        Serial.println("Something was wrong!");
        Serial.print("HRC: ");
        Serial.print(hRc); 
      }
  
      http.end();
      Serial.println("closing connection");
    }
}
