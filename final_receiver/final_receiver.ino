//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdio.h>

RF24 radio(9, 8);  // CE, CSN   // create an RF24 object
const byte address[6] = "1Node"; // address through which two modules communicate.

struct Telemetry {
  float    speed_mph;
  uint32_t primary_rpm;
  uint32_t secondary_rpm;
};


void setup()
{
  while (!Serial);
    Serial.begin(9600);

  radio.begin();
  radio.printDetails();
  // Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  // radio.openReadingPipe(0, address[1]); //set the reading address
  radio.openReadingPipe(0, address);
  radio.startListening(); // Set module as receiver
  // radio.printPrettyDetails(); // Print radio details for debugging
  Serial.println("Receiver initialized and listening...");
}

void loop()
{
  //Read the data if available in buffer
  if (radio.available())
  {
    //char text[32] = {0};
    //radio.read(&text, sizeof(text));
    float speed_data = 0.0;
    Serial.print("Received: ");
    radio.read(&speed_data, sizeof(float));
    Serial.println(speed_data);

    // Test signal strength
    Serial.print("Sig: ");
    Serial.println(radio.testRPD() ? "Strong" : "Weak");
  }
  else
  {
    Serial.println("No data received");
  }

  delay(1000);
}


