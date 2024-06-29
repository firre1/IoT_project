#include "config.h"


// Variables
//Morse code
int dotDelay = 500;
int dashDelay = dotDelay * 5; // Delay for dash
int symbolSpace = dotDelay; // Space between parts of the same letter
int letterSpace = dotDelay * 6; // Space between letters
int wordSpace = dotDelay * 10; // Space between words
//Temperature 
unsigned long previousMillis = 0;
unsigned long interval = 1000; // interval for temperature measurement
float temperatureSum = 0;
int count = 0;
unsigned long startTime = 0;
//Pins
const int ledPin1 = 25;
const int ledPin2 = 26;
const int ledPin3 = 27; 
const int sensorPin = 34;

// set up the 'button' feed
AdafruitIO_Feed *temperature = io.feed("temperature");

// set up the 'text' feed for receiving text messages
AdafruitIO_Feed *text_feed = io.feed("text");

// Morse code for letters
char* letters[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

// Morse code for numbers
char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-", ".....",
  "-....", "--...", "---..", "----."
};


// function declarations here:
void flashSequence(char* sequence);
void flashDotOrDash(char dotOrDash);
void playStartSequence();
void handleTextMessage(AdafruitIO_Data *data);

void setup() {
  
  Serial.begin(115200);
  delay(1000);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  startTime = millis();

    // wait for serial monitor to open
  while (!Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // set up message handlers
  text_feed->onMessage(handleTextMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
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


// function definitions here:


void playStartSequence() {
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  delay(1000); // Wait for 1 second

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, LOW);
  delay(1000); // Wait for 1 second

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, HIGH);
  delay(1000); // Wait for 1 second
}


void flashSequence(char* sequence) {
  int i = 0;
  while (sequence[i] != '\0') {
    flashDotOrDash(sequence[i]);
    i++;
  }
  delay(letterSpace); // Space between letters
}

void flashDotOrDash(char dotOrDash) {
  if (dotOrDash == '.') {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    delay(dotDelay); 
  } else { // must be a '-'
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    delay(dashDelay); 
  }
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  delay(symbolSpace);
}

// this function is called whenever a 'text' message
// is received from Adafruit IO. it was attached to
// the text feed in the setup() function above.
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