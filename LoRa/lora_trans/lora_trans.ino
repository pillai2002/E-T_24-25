//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>


//setting up gps speedometer
TinyGPS gps;
SoftwareSerial ss(4, 3);
static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "1Node";

void setup() 
{
  Serial.begin(9600);  // Initialize serial communication
  radio.begin(); // Initialize radio communication

  Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS::library_version());

  radio.openWritingPipe(address[1]); // set the writing address
  // radio.openReadingPipe(1, address[0]) 

  radio.setPALevel(RF24_PA_MIN);

  radio.stopListening(); //Set module as transmitter

  radio.printPrettyDetails(); // Print radio details for debugging

  Serial.println("Transmitter initialized...");

  ss.begin(9600);
}

void loop() 
{


  float speed_data = gps.f_speed_mph();//, TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
  // const char text[] = "hello world";
  // bool report = radio.write(&text, sizeof(text)); // Send message to receiver

  bool report = radio.write(&speed_data, sizeof(float));

  Serial.println(report ? "Transmission successful" : "Transmission failed");

  // Test if ACK was received
  if (radio.isAckPayloadAvailable()) {
    Serial.println("Acknowledgement received");
  } else {
    Serial.println("No acknowledgement");
  }

  smartdelay(1000);
  //delay(1000);
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

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}
