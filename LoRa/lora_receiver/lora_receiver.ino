//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdio.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "1Node";

void setup()
{
  while (!Serial);
    Serial.begin(9600);

  radio.begin();
  radio.printDetails();
  
  // radio.openWritingPipe(address[0]); 
  radio.openReadingPipe(0, address[1]); //set the reading address

  radio.startListening(); // Set module as receiver

  radio.printPrettyDetails(); // Print radio details for debugging

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
    Serial.print("Signal strength: ");
    Serial.println(radio.testRPD() ? "Strong" : "Weak");
  }
  else
  {
    Serial.println("No data received");
  }

  delay(1000);
}

