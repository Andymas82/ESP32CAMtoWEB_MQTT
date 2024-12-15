#include <esp_camera.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> // MQTT
#include <ESP32Servo.h> // servo

//  Wi-Fi
const char* ssid = "YOUR WiFI SSID";
const char* password = "YOUR WiFI PASSWORD";

// MQTT connection
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "YOUR TOPIC";

// Web host
const char* host = "YOUR WEB HOST"; // http
const int httpPort = 80;

// Camera init
bool cameraOn = true;
framesize_t currentFrameSize = FRAMESIZE_QVGA;

// MQTT init
WiFiClient espClient;
PubSubClient client(espClient);

// Servo init
Servo servo;
const int servoPin = 13;
int currentAngle = 50;
int targetAngle = 50;
const int stepSize = 5; // step 

// Image to HTTP
void sendImageHTTP(camera_fb_t *fb) {
    WiFiClient client;

    if (!client.connect(host, httpPort)) {
        return;
    }

    String boundary = "----ESP32CamBoundary";
    String bodyStart = "--" + boundary + "\r\n";
    bodyStart += "Content-Disposition: form-data; name=\"file\"; filename=\"image.jpg\"\r\n";
    bodyStart += "Content-Type: image/jpeg\r\n\r\n";
    String bodyEnd = "\r\n--" + boundary + "--\r\n";

    int contentLength = bodyStart.length() + fb->len + bodyEnd.length();

    client.println("POST /upload.php?i=1 HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Content-Type: multipart/form-data; boundary=" + boundary);
    client.println("Content-Length: " + String(contentLength));
    client.println("Cookie: __test=value");
    client.println();
    client.print(bodyStart);
    client.write(fb->buf, fb->len);
    client.print(bodyEnd);

    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }
    String response = client.readString();
}

// Number check
bool isNumeric(String str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        if (!isDigit(str[i])) {
            return false;
        }
    }
    return true;
}

// MQTT read & logic
void handleCommand(String cmd) {
    if (cmd == "start") {
        cameraOn = true;
    } else if (cmd == "stop") {
        cameraOn = false;
    } else if (cmd == "increase_resolution") {
        if (currentFrameSize < FRAMESIZE_UXGA) {
            currentFrameSize = static_cast<framesize_t>(currentFrameSize + 1);
            sensor_t *s = esp_camera_sensor_get();
            s->set_framesize(s, currentFrameSize);
        }
    } else if (cmd == "decrease_resolution") {
        if (currentFrameSize > FRAMESIZE_96X96) {
            currentFrameSize = static_cast<framesize_t>(currentFrameSize - 1);
            sensor_t *s = esp_camera_sensor_get();
            s->set_framesize(s, currentFrameSize);
        }
    } else {
        if (isNumeric(cmd)) {
            int angle = cmd.toInt();
            if (angle < 0) angle = 0;
            if (angle > 180) angle = 180;
            targetAngle = angle;
        }
    }
}

// Callback function, MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    if (String(topic) == mqtt_topic) {
        handleCommand(message);
    }
}

// Connection to MQTT broker
void reconnectMQTT() {
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        if (client.connect(client_id.c_str())) {
            client.subscribe(mqtt_topic);
        } else {
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 5;
    config.pin_d1 = 18;
    config.pin_d2 = 19;
    config.pin_d3 = 21;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;
    config.pin_xclk = 0;
    config.pin_pclk = 22;
    config.pin_vsync = 25;
    config.pin_href = 23;
    config.pin_sscb_sda = 26;
    config.pin_sscb_scl = 27;
    config.pin_pwdn = 32;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = currentFrameSize;
    config.jpeg_quality = 10;
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        while (1);
    }

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);
    reconnectMQTT();

    servo.attach(servoPin, 500, 2500);
    servo.write(currentAngle);
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    if (currentAngle < targetAngle) {
        currentAngle = targetAngle;
        //if (currentAngle > targetAngle) currentAngle = targetAngle;
        servo.write(currentAngle);
        delay(10);
    } else if (currentAngle > targetAngle) {
        currentAngle = targetAngle;
     //   if (currentAngle < targetAngle) currentAngle = targetAngle;
        servo.write(currentAngle);
       delay(10);
    }

    if (cameraOn) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (fb) {
            sendImageHTTP(fb);
            esp_camera_fb_return(fb);
        }
        delay(300);
    }
}
