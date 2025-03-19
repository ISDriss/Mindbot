#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// WiFi
const char* ssid     = "wifi";
const char* password = "password";

// Serveur web
WebServer server(80);

// Définition des pins moteur
int enable1Pin = 10;  
int motor1Pin1 = 9; 
int motor1Pin2 = 8; 

int enable2Pin = 7;  
int motor2Pin1 = 6; 
int motor2Pin2 = 5; 

// Configuration PWM
const int freq = 1000;
const ledc_timer_bit_t resolution = LEDC_TIMER_8_BIT;
int dutyCycle = 150;

String valueString = String(0);

// Fonction pour gérer la page principale
void handleRoot() {
  Serial.println("Client connected to Web Interface");
  const char html[] PROGMEM = R"rawliteral(
    <h1>ESP32-CAM Car Control</h1>
    <button onclick="fetch('/forward')">FORWARD</button>
    <button onclick="fetch('/reverse')">REVERSE</button>
    <button onclick="fetch('/left')">LEFT</button>
    <button onclick="fetch('/right')">RIGHT</button>
    <button onclick="fetch('/stop')">STOP</button>
    <br>
    Motor Speed: <input type="range" min="50" max="255" value="150" oninput="fetch('/speed?value=' + this.value)">
  )rawliteral";
  server.send(200, "text/html", html);
}

// Fonction de mouvement
void moveMotors(int m1_1, int m1_2, int m2_1, int m2_2) {
  digitalWrite(motor1Pin1, m1_1);
  digitalWrite(motor1Pin2, m1_2);
  digitalWrite(motor2Pin1, m2_1);
  digitalWrite(motor2Pin2, m2_2);
}

// Commandes HTTP
void handleForward() {
  Serial.println("Moving Forward");
  moveMotors(HIGH, LOW, HIGH, LOW);
  server.send(200, "text/plain", "Moving Forward");
}

void handleReverse() {
  Serial.println("Moving Reverse");
  moveMotors(LOW, HIGH, LOW, HIGH);
  server.send(200, "text/plain", "Moving Reverse");
}

void handleLeft() {
  Serial.println("Turning Left");
  moveMotors(LOW, HIGH, HIGH, LOW);
  server.send(200, "text/plain", "Turning Left");
}

void handleRight() {
  Serial.println("Turning Right");
  moveMotors(HIGH, LOW, LOW, HIGH);
  server.send(200, "text/plain", "Turning Right");
}

void handleStop() {
  Serial.println("Stopping");
  moveMotors(LOW, LOW, LOW, LOW);
  server.send(200, "text/plain", "Stopping");
}

void handleSpeed() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    int value = valueString.toInt();
    if (value == 0) {
      ledcWrite(enable1Pin, 0);
      ledcWrite(enable2Pin, 0);
      moveMotors(LOW, LOW, LOW, LOW);
    } else { 
      dutyCycle = map(value, 25, 100, 200, 255);
      ledcWrite(enable1Pin, dutyCycle);
      ledcWrite(enable2Pin, dutyCycle);
      Serial.println("Motor speed set to " + String(value));
    }
  }
  server.send(200);
}

void setup() {
  Serial.begin(115200);

  // Initialisation des moteurs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // Initialisation du PWM with 0 duty cycle
  ledcAttach(enable1Pin, freq, resolution);  // Attach motor pin to PWM channel 0 with specified frequency and resolution
  ledcAttach(enable2Pin, freq, resolution);  // Attach motor pin to PWM channel 1 with specified frequency and resolution
  ledcWrite(enable1Pin, 0);
  ledcWrite(enable2Pin, 0);

  // Connexion WiFi
  Serial.print("Connexion au WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecté. IP: " + WiFi.localIP().toString());

  // Routes HTTP
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/reverse", handleReverse);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/speed", handleSpeed);

  // Démarrer le serveur web
  server.begin();
}

void loop() {
  server.handleClient();
}
