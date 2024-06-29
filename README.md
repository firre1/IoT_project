# Temperature and Morse Code Project

## Author
Firoz Akbari (fa223me)

## Project Overview
This project is about a temperature sensor used through an ESP32 that measures temperature and displaying different text messages in Morse code through three LEDs. The data that is being transmitted between the Adafruit IO and the ESP32 is controlled and monitored. The hardware setup includes three LED diodes connected to GPIO pins on the ESP32 and a temperature sensor with its output pin connected to an analog pin on the ESP32.

### Time Estimate
This project will take approximately 2 hours to complete.

## Objective
The objective of this project is to create a simple IoT device that measures temperature and displays the data in the dashboard and shows different messages through Morse code using LEDs. This project purpose is to get a better understanding about the basics of IoT. The reason I chose this particular project is that I thought it would be relatively easier. I didn't have an LED screen and I wanted to see how I could send messages so Morse code was an option that fit in very well. The insights I gained from this project are understanding how to interface sensors with microcontrollers, transmitting data over the internet and learning Morse code.

This is what the end result could look like:

![esp32IoT](https://github.com/firre1/IoT_project/assets/51923476/b3cff70f-f81f-42e4-96c5-476fcea0e86f)



## Materials

List of the components used in this project with their costs in SEK.

- **ESP32**: Can be found online somewhere between 100-200 kr. I got mine for free from a friend :). 
- **LED Diodes**: 5 kr each, total 15 kr. Bought from electrokit.
- **Resistors**: 1 kr each, total 3 kr. Bought from electrokit.
- **Temperature Sensor**: 12 kr. Bought from electrokit.
- **Breadboard and Wires**: 69 kr for the breadboard and 49 kr for the wires. Bought from electrokit.
- **Micro-USB-cable**: 39 kr. Bought from electrokit.

### Total Cost
- **Total Cost**: around 300 kr, if you were to buy a esp32.

### Description of Components

The ESP32 microcontroller was chosen for its powerful processing capabilities and the integrated WiFi and Bluetooth. These features make it ideal for IoT projects where connectivity and multitasking are essential. The ESP32 has a large community and extensive documentation which makes it easier to use.

Three LED diodes (5mm red) are used to display Morse code messages. LEDs are chosen for their simplicity and ability to provide a visual of the data that is being transmitted. 330 Ohm resistors are used to limit the current flowing through the LEDs preventing any potential damage.

An analog temperature sensor (MCP9700 TO-92) is used to measure temperature. This sensor is easy to use with the ESP32 analog input pins and gives good readings. The breadboard and wires are used to connect all the different components into a single unit.


## Computer Setup

In this project Arduino IDE was used. You will need to install the Arduino IDE and configure it to work with the ESP32.


### Follow these steps:

1. **Install Arduino IDE**:
   - Download the Arduino IDE from the official Arduino website.
   - Follow the installation instructions for your OS (Windows, macOS, etc).

2. **Install the ESP32 Board**:
   - Open the Arduino IDE.
   - Go to `File > Preferences`.
   - In the "Additional Boards Manager URLs" field, add the following URL: `https://dl.espressif.com/dl/package_esp32_index.json`.
   - Click `OK` to save.
   - Go to `Tools > Board > Boards Manager`.
   - In the Boards Manager window, search for "ESP32" and click `Install` on the "esp32 by Espressif Systems".

3. **Install Libraries**:
   - Open the Arduino IDE.
   - Go to `Sketch > Include Library > Manage Libraries`.
   - In the Library Manager window, search for and install the library `Adafruit IO Arduino`.

4. **Connect the ESP32 to Your Computer**:
   - Use a USB cable to connect the ESP32 to your computer.
   - Go to `Tools > Board` and select your ESP32 board model ("ESP32 Dev Module").
   - Go to `Tools > Port` and select the port to which the ESP32 is connected.
   - Open or write your Arduino code in the Arduino IDE.
   - Click the `Upload` button in the Arduino IDE toolbar to compile and upload the code to your ESP32.
   - Wait for the "Done uploading" message.
   - Your ESP32 is now programmed with your code.

## Putting Everything Together
### Circuit Diagram
1. **LEDs**:
   - Connect the cathode of each LED to ground through a 330 Ohm resistor.
   - Connect the anode of the LEDs to GPIO pins G25, G26, and G27 on the ESP32.
2. **Temperature Sensor**:
   - Connect VDD to 5V.
   - Connect GND to ground.
   - Connect Vout to analog pin G34 on the ESP32.

How the circuit looks like:
![circuitESP](https://github.com/firre1/IoT_project/assets/51923476/a25f1c87-373f-4915-96e5-8582f65c980a)

## Platform

Adafruit IO is a cloud based platform that enables remote monitoring and control of your IoT devices. It offers various features for free which made it an very good option for this project. Adafruit IO is designed to be user friendly, it was easy to set up and use. It integrates simply with the Arduino IDE.
  
**Adafruit IO functionalities**:
- View real time data from your sensors on customizable dashboards.
- Log sensor data for analysis.
- Send commands to your devices remotely.
- Perform actions based on data thresholds or different types of conditions.


## The Code

This part of the code initializes the Adafruit IO feeds for temperature data and text messages. In the `setup` function, the ESP32 connects to Adafruit IO and sets up message handlers for incoming text messages. The LED pins are also configured to behave as an output.

   ```cpp
   // Set up the 'temperature' feed
   AdafruitIO_Feed *temperature = io.feed("temperature");

   // Set up the 'text' feed for receiving text messages
   AdafruitIO_Feed *text_feed = io.feed("text");

   void setup() {
     Serial.begin(115200);
     delay(1000);

     pinMode(ledPin1, OUTPUT);
     pinMode(ledPin2, OUTPUT);
     pinMode(ledPin3, OUTPUT);
     startTime = millis();

     // Wait for serial monitor to open
     while (!Serial);

     Serial.print("Connecting to Adafruit IO");

     // Connect to io.adafruit.com
     io.connect();

     // Set up message handlers
     text_feed->onMessage(handleTextMessage);

     // Wait for a connection
     while (io.status() < AIO_CONNECTED) {
       Serial.print(".");
       delay(500);
     }

     // We are connected
     Serial.println();
     Serial.println(io.statusText());
   }
```
The `loop` function is the main part of the code that runs continuously after the `setup` function.
- `io.run()` This function call is important for maintaining the connection to Adafruit IO and processing any incoming data. It should always be at the beginning of the loop function.
- The code reads the temperature sensor value at intervals specified by the interval variable (1000 ms). It calculates the temperature based on the sensor value.
- Every 30 seconds the code calculates the average temperature. Prints it to the serial monitor and sends it to the Adafruit IO temperature feed. After sending the data it resets the sum and count for the next measurement cycle.
```cpp

void loop() {
  io.run();

  unsigned long currentMillis = millis();

  // Temperature measurement
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int sensorValue = analogRead(sensorPin);
    // 500 mV at 0°C and 10 mV/°C
    float temperature = (sensorValue - 500) / 10;
    temperatureSum += temperature;
    count++;
  }

  // Check if 30 seconds have passed
  if (currentMillis - startTime >= 30000) {
    float averageTemperature = temperatureSum / count;
    Serial.print("sending temperature -> ");
    Serial.println(averageTemperature);
    temperature->save(averageTemperature);

    // Reset for next measurement cycle
    temperatureSum = 0;
    count = 0;
    startTime = currentMillis;
  }
}
```

The `handleTextMessage` function is called whenever a text message is received from Adafruit IO. This function processes the incoming message and displays it as Morse code using the LEDs.
- Receiving and Printing the Message: When a text message is received, the function prints it to the serial monitor.
- Before displaying the Morse code, the `playStartSequence` function is called to indicate the start of a new message.
- The function loops through each character in the message. Depending on whether the character is a lowercase letter, uppercase letter, digit or space, it retrieves the corresponding Morse code sequence and flashes the LEDs accordingly using the `flashSequence` function.
```cpp
void handleTextMessage(AdafruitIO_Data *data) {
  Serial.print("received text message <- ");
  Serial.println(data->toString());

  String message = data->toString();
  playStartSequence();
  for (int i = 0; i < message.length(); i++) {
    char ch = message[i];
    if (ch >= 'a' && ch <= 'z') {
      Serial.print(ch);
      Serial.print(": ");
      Serial.println(letters[ch - 'a']);
      flashSequence(letters[ch - 'a']);
    } else if (ch >= 'A' && ch <= 'Z') {
      Serial.print(ch);
      Serial.print(": ");
      Serial.println(letters[ch - 'A']);
      flashSequence(letters[ch - 'A']);
    } else if (ch >= '0' && ch <= '9') {
      Serial.print(ch);
      Serial.print(": ");
      Serial.println(numbers[ch - '0']);
      flashSequence(numbers[ch - '0']);
    } else if (ch == ' ') {
      Serial.println("Space");
      delay(wordSpace); // Space between words
    }
  }
}
```
## Transmitting the Data / Connectivity

The data is transmitted to the internet using the Adafruit IO platform. Temperature data is sent to Adafruit IO every 30 seconds. This interval updates the temperature while balancing network usage and power consumption. The ESP32 sends temperature data to Adafruit IO using WiFi and the MQTT protocol.

1. **WiFi Connection**: The ESP32 connects to a local WiFi network to access the internet.
2. **MQTT Protocol**: The data is sent to Adafruit IO using the MQTT protocol which is a lightweight messaging protocol ideal for IoT devices.
3. **Data Format**: The temperature data is packaged as a float value and sent to a predefined feed on Adafruit IO.

### Design Choices

- **WiFi**: WiFi is selected for its wide availability and the easy integration with the ESP32. While it consumes more power compared to protocols like LoRa it provides sufficient range and bandwidth for this project.
- **MQTT**: The choice of MQTT for data transmission is driven by its lightweight nature and support for IoT use cases. MQTT's publish/subscribe model allows for efficient data handling and is well supported by Adafruit IO.

## Presenting the Data

The presentation of the data is done using the Adafruit IO dashboard. The dashboard is built with widgets that display real time temperature readings and allow you to send messages to the ESP32.

### Dashboard Setup

   - **Gauge Widget**: This widget is used to display the real time temperature data. It shows the current temperature value in an circular gauge format.
   - **Text Block Widget**: This widget allows you to send text messages to the ESP32. The ESP32 receives these messages and displays them as Morse code using the connected LEDs.
   - The data sent to Adafruit IO is stored in the Adafruit IO feed. By default Adafruit IO keeps the data for 30 days on the free plan it can be extended with a paid plan.
   - The temperature data on the dashboard is updated every 30 seconds.

   How the dashboard looks like:
   ![adaDashb](https://github.com/firre1/IoT_project/assets/51923476/7aa95a24-f891-4f49-89b4-3115e3a21142)


## Finalizing the Design

This project was an fun and a educational opportunity to explore the integration of hardware and software for an IoT application. It demonstrated how to use the ESP32 to read temperature data, display messages using Morse code and transmit data to Adafruit IO for remote controlling and monitoring. The integration of the ESP32 with sensors and LEDs worked great. The temperature sensor provided accurate readings and the LEDs effectively displayed Morse code. The Adafruit IO dashboard was easy to set up and use. It gave a clear visualization of temperature data and an efficient way to send messages to the ESP32. By using the WiFi and MQTT protocols the data transmission to Adafruit was very consistent and reliable.

Implementing better power management techniques like deep sleep modes for the ESP32 could reduce power consumption and make the project more suitable for battery powered applications. Adding more sensors to project such as humidity and pressure sensors could expand the projects functionality. Integrating voice commands or smartphone control could also improve the user interaction. 


Overall the project was a success. Achieving its primary goals and offering valuable learning experiences.

![LedOn](https://github.com/firre1/IoT_project/assets/51923476/12f98884-e6de-4cc7-bf05-138a061cab16)

