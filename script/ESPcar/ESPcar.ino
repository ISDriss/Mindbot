#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// WiFi
const char* ssid     = "wifi";
const char* password = "password";

// Serveur web
WebServer server(80);

// Définition des pins moteur
int motor1Pin1 = 9; 
int motor1Pin2 = 8; 

int motor2Pin1 = 6; 
int motor2Pin2 = 5; 

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
  moveMotors(HIGH, LOW, LOW, HIGH);
  server.send(200, "text/plain", "Moving Forward");
}

void handleReverse() {
  Serial.println("Moving Reverse");
  moveMotors(LOW, HIGH, HIGH, LOW);
  server.send(200, "text/plain", "Moving Reverse");
}

void handleLeft() {
  Serial.println("Turning Left");
  moveMotors(LOW, HIGH, LOW, HIGH);
  server.send(200, "text/plain", "Turning Left");
}

void handleRight() {
  Serial.println("Turning Right");
  moveMotors(HIGH, LOW, HIGH, LOW);
  server.send(200, "text/plain", "Turning Right");
}

void handleStop() {
  Serial.println("Stopping");
  moveMotors(LOW, LOW, LOW, LOW);
  server.send(200, "text/plain", "Stopping");
}

void setup() {
  Serial.begin(115200);

  // Initialisation des moteurs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // Activation permanente des moteurs
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);

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

  // Démarrer le serveur web
  server.begin();
  Serial.println("Serveur Web démarré !");
}

void loop() {
  server.handleClient();
}
