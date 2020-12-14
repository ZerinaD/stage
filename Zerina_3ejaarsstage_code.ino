#include <MKRWAN.h>
#include <DallasTemperature.h>
#include <OneWire.h>

int laatstverzondentijd = 0;

LoRaModem modem;

#include "arduino_secrets.h"
//personal information of the module
String appEui = "70B3D57ED0037CA1";
String appKey = "4F923CF9361D07BFB6FD8C4E2CC2342E";


// This code is based on example codes from the Dallas Temperature and LoraSendAndReceive library 

//batterij
const int batterijpin = A1; //pin om batteri status af te lezen
//const int ledpin= 3;
//instellingen voor de sensor
// Temperatuur sensor DS18B20 op pin 2 aangesloten
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void) {
  pinMode(batterijpin, INPUT);
  //pinMode(ledpin, OUTPUT);
  
  //start serial port
//  Serial.begin (115200);
//  while(!Serial);
  
  // frequency Europe
   if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
   };

//  Serial.print("Your module version is: ");
//  Serial.println(modem.version());
//  Serial.print("Your device EUI is: ");
//  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
     if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
     while (1) {}
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independently by this setting the modem will
  // not allow to send more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
  
  //Start up the Dallas Temperature library
  sensors.begin();
 
}

float Batterij(float batterijtje){
    float batterij= analogRead(batterijpin); //10 bits waarde aflezen.
    float batterijstatus = ((batterij/1024)*3.3)/0.5; 
    //2^10=1024, maar dat is van 0 tot 1023. ADC pin kan max 3.3V aflezen. En de spanningsdeler zorgt ervoor dat de spanning gehalveert wordt.
    float percentagebatterij = (batterijstatus/4.5)*100.00;
    Serial.print(percentagebatterij);
    Serial.println();
    delay(1000);
    return percentagebatterij;
}

float Temperatuur(float gettemperatuur){
    //temperatuur ophalen
    sensors.setResolution(10);
    sensors.requestTemperatures();
    float gettemperature = sensors.getTempCByIndex(0);
    Serial.print(gettemperature);
    Serial.println();
    delay(1000);
    return gettemperature;
}

void loop(void) {
    float b;
    float t;
    String percentage = String(Batterij(b));
    //Serial.print(percentage);
    //Serial.println();
    String celsius= String (Temperatuur(t)); 
    String msg = celsius + percentage;
    //Serial.print(msg);
    
    if(millis() > laatstverzondentijd + 60000)
    {
      modem.beginPacket();
      modem.print(msg);
      Serial.println();
      int err;
      err = modem.endPacket(true); 
      if (err > 0) {
      Serial.println("Message sent correctly!");
      laatstverzondentijd = millis();
      } else {
      Serial.println("Error sending message :(");
      Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
      Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
      laatstverzondentijd = millis();
      }
      
      if (!modem.available()) {
      Serial.println("No downlink message received at this time.");
      return;
    }
    }
    }
 
  
