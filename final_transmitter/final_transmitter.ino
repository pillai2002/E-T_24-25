// GPS libs
#include <SoftwareSerial.h>
#include <TinyGPS.h>

// LoRa libs
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// GPS  
TinyGPS gps;
SoftwareSerial ss(4, 3); // RX -> D3  TX -> D4
static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);

// LoRa
RF24 radio(9, 8);  // CE, CSN   create an RF24 object
const byte address[6] = "1Node";  // address through which two modules communicate.

struct Telemetry {
  float       speed_mph;     // 4 bytes
  uint32_t    primary_rpm;   // 4 bytes
  uint32_t    secondary_rpm; // 4 bytes
};                           

// hall sensor
int hallprimary_pin = 2; // Hall sensor for primary cvt at pin 2
int hallsecondary_pin = 5; // Hall sensor for secondary cvt at pin 5
unsigned long primary_rpm = 0;
unsigned long secondary_rpm = 0;
unsigned long duration1, duration2;


void setup() {
  Serial.begin(9600);
  radio.begin(); // Initialize radio communication

  // Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS::library_version());

  // LoRa
  // radio.openWritingPipe(address[1]); // set the writing address // radio.openReadingPipe(1, address[0]) 
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); //Set module as transmitter
  // radio.printPrettyDetails(); // Print radio details for debugging

  // hall sensor
  pinMode(hallprimary_pin, INPUT); //Sets sensor as input
  pinMode(hallsecondary_pin, INPUT); //Sets sensor as input
  // Serial.println("Hall effect sensor test:");

  ss.begin(9600);
}

void loop() {

  // GPS Speedometer 
  // print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
  float speed_data = gps.f_speed_mph();


  // hall sensor
  //starttime = millis();
  duration1 = pulseIn(hallprimary_pin, RISING, 1000000); //times the amount of microseconds the motor is not timing IR, Times out after 100000 uS. Raise the timeout for slower RPM readings. .5 second
  duration2 = pulseIn(hallsecondary_pin, RISING, 1000000);
  //elapsedTime=millis()-starttime;
  if (duration1 > 0) primary_rpm = 60000000.0 / duration1; 
  else primary_rpm = 0;

  if (duration2 > 0) secondary_rpm = 60000000.0 / duration2;
  else secondary_rpm = 0;


 if(primary_rpm>0){
    String outMsg = String("RPM :") + primary_rpm;
    Serial.println(outMsg);
    smartdelay(1000);
  }else{
  smartdelay(1000);
  }

  // LoRa
  Telemetry pkt;
  pkt.speed_mph    = speed_data;
  pkt.primary_rpm  = primary_rpm;
  pkt.secondary_rpm = secondary_rpm;

  bool report = radio.write(&pkt, sizeof(pkt));
  Serial.println(report ? "TX-OK" : "TX-Fail");

  // if (radio.isAckPayloadAvailable()) {          // Test if ACK was received
  //   Serial.println("Yes Ack");
  // } else {
  //   Serial.println("No Ack");
  // }
  
}


static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
