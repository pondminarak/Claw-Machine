#include <Adafruit_PWMServoDriver.h>
#include <ESP32Servo.h>

#include <ElegantOTA.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

//WiFi want to connect
#define WIFI_SSID  "PondKuB_WIFI"
#define WIFI_PASS  "0831764572"

WiFiClient wifiClient;
WebServer webServer(80);
PubSubClient mqttClient(wifiClient);

#define APP_MQTT_SERVER  "192.168.1.104"
#define APP_MQTT_PORT    1883

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Servo Servo1;
Servo Servo2;

#define ButtonLeft 26
#define ButtonRight 25
#define ButtonUp 17
#define ButtonDown 16
#define ButtonGrab 27
#define ButtonRelease 14
#define SERVO_MIN_PULSE 150
#define SERVO_MAX_PULSE 600
#define X_AXIS 36
#define Y_AXIS 39
#define ServoClaw1 12
#define ServoClaw2 13
#define SERVOMIN 200
#define SERVOMAX 600
bool ServoCheck = false;

void setupWiFi() {
  Serial.println();
  Serial.println("Connecting to " + String(WIFI_SSID) + " ...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Failed to connect to the Wi-Fi network, restarting...");
      delay(2000);
      ESP.restart();
  }

  Serial.println("Wi-Fi connected, browse to http://" + WiFi.localIP().toString());
}

void mqttCallback(const char *topic, byte *message, unsigned int length) {
  String topicStr(topic);
  String messageStr((uint8_t *)message, length);
  String topicTokens[5];

  Serial.println("MQTT message received on topic: " + topicStr + ", message: " + messageStr);

  for (int i = 0; i < 5; i++) {
      int slashPos = topicStr.indexOf("/");
      topicTokens[i] = topicStr.substring(0, slashPos);
      topicStr = topicStr.substring(slashPos + 1);
      // Serial.println("Slash: " + String(slashPos) + ", Token " + String(i) + ": " + topicTokens[i]);
  }

  if (topicTokens[0] == "esp32" && topicTokens[1] == "servo" && messageStr == "unlock") {
      ServoCheck = true;
      Serial.println("Message: " + messageStr);
  }
  else if(topicTokens[0] == "esp32" && topicTokens[1] == "servo" && messageStr == "lock"){
      ServoCheck = false;
      Serial.println("Message: " + messageStr);
  }
  else {
      Serial.println("Invalid MQTT message received on topic: " + String(topic));
  }
}

void setupMqttClient() {
    mqttClient.setServer(APP_MQTT_SERVER, APP_MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    Serial.println("Connecting to MQTT broker " + String(APP_MQTT_SERVER) + ":" + String(APP_MQTT_PORT));

    while(!mqttClient.connected()) {
        if (mqttClient.connect("ESP32-servo")) {
            // device/<MAC_ADDRESS>/led/<LED_NUMBER>/command
            String ServoTopic = "esp32/servo";

            Serial.println("Connected to MQTT broker");
            Serial.println("Subscribing to topic: " + ServoTopic);

            mqttClient.subscribe(ServoTopic.c_str());
        }
        else {
            Serial.println("Failed to connect to MQTT broker, retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void setup() {
  Serial.begin(115200);
  pinMode(ButtonGrab, INPUT_PULLUP);
  pinMode(ButtonLeft, INPUT_PULLUP);
  pinMode(ButtonRight, INPUT_PULLUP);
  pinMode(ButtonUp, INPUT_PULLUP);
  pinMode(ButtonDown, INPUT_PULLUP);
  pinMode(ButtonRelease, INPUT_PULLUP);
  // pinMode(X_AXIS, INPUT);
  // pinMode(Y_AXIS, INPUT);
  Servo1.attach(ServoClaw1);
  Servo2.attach(ServoClaw2);
  pwm.begin();
  pwm.setPWMFreq(60);

  setupWiFi();
  setupMqttClient();
  webServer.begin();
  ElegantOTA.begin(&webServer);
  String esp32IPAddress = WiFi.localIP().toString();
  //print IP in Serial Moniter
  Serial.println("\nWiFi connected, IP: " + esp32IPAddress);
}

void loop() {
  webServer.handleClient();
  mqttClient.loop();
  ElegantOTA.loop();
  // int X = analogRead(X_AXIS);
  // int Y = analogRead(Y_AXIS);
  
  // X = map(X, 0, 4095, 0, 100);
  // Y = map(Y, 0, 4095, 0, 100);

  // Serial.print("X: ");
  // Serial.print(X);
  // Serial.print(" Y: ");
  // Serial.println(Y);

  // delay(50);
  if (digitalRead(ButtonLeft) == HIGH && digitalRead(ButtonRight) == HIGH && digitalRead(ButtonUp) == HIGH && digitalRead(ButtonDown) == HIGH && ServoCheck){
    pwm.setPWM(0, 0, 0);
    pwm.setPWM(1, 0, 0);
    pwm.setPWM(8, 0, 0);
    pwm.setPWM(9, 0, 0);
  }
  if (digitalRead(ButtonLeft) == LOW && ServoCheck){
    Serial.println("LEFT");
    pwm.setPWM(0, 0, 200);
    pwm.setPWM(1, 0, 200);
    delay(10);
  }

  if (digitalRead(ButtonRight) == LOW && ServoCheck){
    Serial.println("RIGHT");
    pwm.setPWM(0, 0, 500);
    pwm.setPWM(1, 0, 500);
    delay(10);
  }

  if (digitalRead(ButtonUp) == LOW && ServoCheck){
    Serial.println("UP");
    pwm.setPWM(8, 0, 200);
    pwm.setPWM(9, 0, 200);
    delay(10);
  }

  if (digitalRead(ButtonDown) == LOW && ServoCheck){
    Serial.println("DOWN");
    pwm.setPWM(8, 0, 500);
    pwm.setPWM(9, 0, 500);
    delay(10);
  }
  // if (X > 55 && X < 70 && Y > 55 && Y < 70) {
  //   pwm.setPWM(0, 0, 0);
  //   pwm.setPWM(1, 0, 0);
  //   pwm.setPWM(8, 0, 0);
  //   pwm.setPWM(9, 0, 0);
  // } else if (X < 10) {
  //   pwm.setPWM(2, 0, 200);
  //   pwm.setPWM(3, 0, 200);
  //   delay(10);
  // } else if (X > 90) {
  //   pwm.setPWM(2, 0, 500);
  //   pwm.setPWM(3, 0, 500);
  //   delay(10);
  // } else if (Y < 10) {
  //   pwm.setPWM(8, 0, 200);
  //   pwm.setPWM(9, 0, 200);
  //   delay(10);
  // } else if (Y > 90) {
  //   pwm.setPWM(8, 0, 500);
  //   pwm.setPWM(9, 0, 500);
  //   delay(10);
  // }
  if (digitalRead(ButtonGrab) == LOW && ServoCheck) {
    Serial.println("Grabbing");
    rotateServo();
    delay(3000);
  }
  if (digitalRead(ButtonRelease) == LOW && ServoCheck){
    Serial.println("Releasing");
    for (int angle = 0; angle <= 115; angle++) {
      Servo1.write(angle);
      Servo2.write(115-angle);
      delay(15);
    }
    //ALTERNATE METHOD
    // for (int angle = 0; angle <= 115; angle++) {
    //   int pulseWidth = map(angle,0, 180, SERVOMIN,SERVOMAX);
    //   pwm.setPWM(12, 0, pulseWidth);
    //   pwm.setPWM(13, 0, 115-pulseWidth);
    //   delay(15);
    // }
  }
}

void rotateServo() {
  pwm.setPWM(15, 0, 200);
  delay(4000);  // Allow time for the servo to reach the target position
  pwm.setPWM(15, 0, 0);
  for (int angle = 115; angle >= 0; angle--) {
    Servo1.write(angle);
    Servo2.write(115-angle);
    delay(15);
  }
  //ALTERNATE METHOD
  // for (int angle = 115; angle >= 0; angle--) {
  //   int pulseWidth = map(angle,0, 180, SERVOMIN,SERVOMAX);
  //   pwm.setPWM(12, 0, pulseWidth);
  //   pwm.setPWM(13, 0, 115-pulseWidth);
  //   delay(15);
  // }
  delay(1000);
  pwm.setPWM(15, 0, 600);
  delay(1000);
  pwm.setPWM(15, 0, 0);

}

