# Motion Detection with Notification System

## Overview

This project uses an ESP32 microcontroller to detect motion using a PIR sensor and notifies a user through Signal app messages. Additionally, it serves a simple web page displaying the motion detection count and the last detection time.

## Components

1. **ESP32**: The main microcontroller.
2. **PIR Sensor**: Used to detect motion.
3. **LED**: Indicates motion detection.
4. **WiFi**: Connects to the internet to fetch the current time and send notifications.
5. **Signal API**: Used to send notifications when motion is detected.
6. **Web Server**: Hosts a simple web page to show the number of detections and the last detection time.

## Libraries

- `WiFi.h`: Handles WiFi connectivity.
- `HTTPClient.h`: Manages HTTP requests.
- `WebServer.h`: Creates a web server on the ESP32.

## Configuration

1. **WiFi Credentials**: Replace `my_ssid` and `my_password` with your WiFi SSID and password.
2. **Signal API Configuration**: Replace `my_signal_key` with your Signal API key and `my_phonenumber` with the recipient's phone number.

## Setup

1. **Connect the PIR sensor**:
    - Red = VCC 5V
    - Brown = GND Ground
    - Green = Data pin connected to GIPO pin 13
2. **Connect the LED**:
    - Positive lead to GPIO 21
    - Negative lead to GND (through a resistor if necessary)
  
![image](https://github.com/user-attachments/assets/f9ac0060-5d5e-4006-b3ab-98d4d99a58c0)

## Code Description

### Global Variables
- `ssid` and `password`: WiFi credentials.
- `timeURL`: API endpoint for getting the current time.
- `apiKey` and `phone_number`: Signal API credentials.
- `url`: Stores the final URL for sending the notification.
- `newestTime`: Stores the last motion detection time.
- `count` and `pastCount`: Counters for the number of motion detections.
- `limit`: Maximum number of messages to be sent within a certain time period.
- `led` and `sensor`: GPIO pins for the LED and PIR sensor.
- `state`: Current state of motion detection.
- `previousMillis` and `interval`: Used for managing the cooldown period for notifications.
- `server`: Web server object.

### Functions

- `handleRoot()`: Handles the root URL by displaying the motion count and last detection time.
- `updatePageContent()`: Updates the web page content when motion is detected.
- `setup()`: Initializes the WiFi, sets up the web server, and configures the PIR sensor and LED.
- `loop()`: Continuously checks for motion and updates the server and notifications accordingly.
- `getTime()`: Fetches the current time from an API.
- `message_to_signal()`: Prepares and sends a message through the Signal API.
- `postData()`: Posts data to the API endpoint.
- `urlencode()`: Encodes the URL to ensure it's properly formatted.

## Operation

1. **Startup**: The ESP32 connects to the specified WiFi network and starts the web server.
2. **Motion Detection**: When motion is detected, the LED turns on, and the current time is fetched. The detection count is updated and displayed on the web page.
3. **Notifications**: If motion is detected and the cooldown period has passed, a message is sent to the specified phone number using the Signal API.
4. **Web Page**: The web server hosts a page displaying the total number of detections and the last detection time, which updates in real-time.

Image showing The updates from the signal bot when the motion is triggered

![image](https://github.com/user-attachments/assets/7d1382fa-d7de-4213-bee7-025833c199f6)

Webserver that is created by the ESP32 to display current count of activations and time

![image](https://github.com/user-attachments/assets/3cc495bc-7917-450c-ab49-e6dbd711c29b)

## Notes

- The detection count is limited to avoid exceeding the API's message limit. The api’s limit of 40 messages per 4 hours.
- The cooldown period between notifications is set to 10 minutes.
- Ensure that the API key and phone number are kept confidential.

## Dependencies

- ESP32 board package in the Arduino IDE
- WiFi, HTTPClient, and WebServer libraries

## License

This project is open-source and available for use as long as credit is given.

