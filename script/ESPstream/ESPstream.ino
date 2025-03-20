#include "esp_camera.h"
#include <WiFi.h>
#include <WebSocketsServer.h>

// ✅ Définition des broches de la caméra OV2640
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ✅ Définition des paramètres WiFi
const char* ssid     = "wifi";
const char* password = "password";

// ✅ Serveurs Web et WebSocket
WebSocketsServer webSocket(81);
WiFiServer server(80);

uint8_t cam_num;
bool connected = false;

// ✅ Page HTML pour afficher le flux vidéo
String index_html = R"rawliteral(
<html>
<head>
<title> WebSockets Client</title>
<script src='http://code.jquery.com/jquery-1.9.1.min.js'></script>
<style>
  body { text-align: center; }
  img { width: 100%; max-width: 800px; cursor: pointer; }
  .fullscreen { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: black; display: flex; align-items: center; justify-content: center; }
  .fullscreen img { width: auto; height: 100%; }
</style>
</head>
<body>
<img id='live' src='' onclick='toggleFullScreen(this)'>
<script>
function toggleFullScreen(img) {
    if (!document.fullscreenElement) {
        img.requestFullscreen();
    } else {
        document.exitFullscreen();
    }
}

jQuery(function($){
if (!('WebSocket' in window)) {
    alert('Your browser does not support web sockets');
}else{
    setup();
}
function setup(){
    var host = 'ws://server_ip:81';
    var socket = new WebSocket(host);
    socket.binaryType = 'arraybuffer';
    if(socket){
        socket.onmessage = function(msg){
            var bytes = new Uint8Array(msg.data);
            var binary = '';
            var len = bytes.byteLength;
            for (var i = 0; i < len; i++) {
                binary += String.fromCharCode(bytes[i]);
            }
            var img = document.getElementById('live');
            img.src = 'data:image/jpg;base64,' + window.btoa(binary);
        };
    }
}
});
</script>
</body>
</html>
)rawliteral";

// ✅ Fonction de configuration de la caméra
void configCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 9;
    config.fb_count = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
    }
}

// ✅ Fonction pour initialiser la connexion WiFi
void connectWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connexion au WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connecté !");
    Serial.println("Adresse IP : " + WiFi.localIP().toString());
    
    // Mise à jour de l'IP du serveur dans le code HTML
    index_html.replace("server_ip", WiFi.localIP().toString());
}

// ✅ Fonction de gestion des WebSockets
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Déconnecté !\n", num);
            break;
        case WStype_CONNECTED:
            cam_num = num;
            connected = true;
            Serial.printf("[%u] Connecté\n", num);
            break;
        default:
            break;
    }
}

// ✅ Fonction de capture et d'envoi d'image
void liveCam(uint8_t num) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Erreur de capture de l'image");
        return;
    }
    webSocket.sendBIN(num, fb->buf, fb->len);
    esp_camera_fb_return(fb);
}

// ✅ Fonction de réponse HTTP pour envoyer la page web
void http_resp() {
    WiFiClient client = server.available();
    if (client.connected() && client.available()) {
        client.flush();
        client.print(index_html);
        client.stop();
    }
}

// ✅ Configuration initiale
void setup() {
    Serial.begin(115200);

    // Connexion WiFi
    connectWiFi();

    // Démarrage du serveur Web et WebSocket
    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    // Configuration de la caméra
    configCamera();
}

// ✅ Boucle principale
void loop() {
    http_resp();
    webSocket.loop();
    if (connected) {
        liveCam(cam_num);
    }
}