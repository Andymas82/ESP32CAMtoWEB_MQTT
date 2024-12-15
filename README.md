# ESP32-CAM MQTT Control Project

This repository provides a complete solution for controlling an ESP32-CAM using MQTT and a simple web interface. 
The project includes HTML for the web interface, a PHP script for handling image uploads, and the Arduino code to run on the ESP32-CAM.

---

## Features

### ESP32-CAM
- Captures and uploads images to a server using HTTP.
- Allows control of the camera through MQTT:
  - Start/stop capturing images.
  - Adjust resolution.
- Receives servo angle commands (0–180 degrees) from MQTT and adjusts a connected servo accordingly.

### Web Interface
- Control the ESP32-CAM via MQTT using a user-friendly dashboard.
- Slider for adjusting the servo angle in real time.
- Live image view with automatic refresh.

### PHP Script
- Handles image uploads from the ESP32-CAM.
- Saves the uploaded images to the server.

---

## Folder Structure
```
.
├── index.html                        # Web interface for controlling ESP32-CAM
├── upload.php                        # PHP script for handling image uploads
├── esp32cam_HTTP_MQTT_Servo.ino      # Arduino code for the ESP32-CAM
```

---

## Getting Started

### Prerequisites
1. ESP32-CAM board.
2. MQTT broker (e.g., Mosquitto).
3. Web server with PHP enabled (e.g., Apache or Nginx).

---

### Hardware Setup
1. **ESP32-CAM**: Connect a servo to GPIO 13 (default in the code).
   - Signal: GPIO 13
   - VCC and GND: Connect to 5V and GND on the ESP32-CAM.
2. Ensure the ESP32-CAM is powered and connected to a Wi-Fi network.

---

### Software Setup

#### 1. **ESP32-CAM Firmware**
- Install the [ESP32 board package](https://github.com/espressif/arduino-esp32) in the Arduino IDE.
- Upload `esp32cam.ino` to your ESP32-CAM.
  - Configure your Wi-Fi credentials in the `setup()` function.
  - Set your MQTT broker and server details.

#### 2. **Web Interface**
- Place `index.html` in the root of your web server.
- Update the MQTT broker URL in the `<script>` section of `index.html`.

#### 3. **PHP Script**
- Place `upload.php` on your web server in a directory named `uploads`.
- Ensure the `uploads` directory is writable by the server:
  ```bash
  chmod 777 uploads
  ```

---

### Running the Project

1. **Start the ESP32-CAM**:
   - Power on the ESP32-CAM and check its connection to the Wi-Fi network.
   - The ESP32 will begin sending images to the server and listening for MQTT messages.

2. **Open the Web Interface**:
   - Navigate to `http://<your-web-server>/index.html`.
   - Use the buttons to start/stop the camera and adjust the resolution.
   - Use the slider to control the servo angle.

3. **Monitor MQTT Messages**:
   - The ESP32-CAM will subscribe to the MQTT topic and respond to commands like:
     - `"start"`: Start the camera.
     - `"stop"`: Stop the camera.
     - Servo angle (0–180): Adjust the servo.

---

### Examples

#### MQTT Commands
- Send `"start"` to start the camera.
- Send `"stop"` to stop the camera.
- Send `"increase_resolution"` or `"decrease_resolution"` to adjust the resolution.
- Send a number between `0` and `180` to set the servo angle.

#### Web Interface
- Adjust the servo angle using the slider.
- View live images from the camera with auto-refresh.

---

## Troubleshooting
- **ESP32 not connecting to Wi-Fi**:
  - Ensure the correct Wi-Fi SSID and password are configured.
- **Servo not moving**:
  - Verify the servo connection and ensure GPIO 13 is correctly configured.
- **No images uploading**:
  - Check the `upload.php` script and ensure the `uploads` directory has write permissions.

---

Happy coding! 🚀
