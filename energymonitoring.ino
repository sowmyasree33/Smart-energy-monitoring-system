//energy montoring system
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID "IOTlab"
#define WLAN_PASS "iotlab@p202"

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME  "Ankitha"
#define AIO_KEY       "aio_xHQQ21XjnHEKhL2HE6mRdhFbCsRb"
// use 8883 for SSL
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish photocell=Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/power");
const int sensorIn = A0;
int mVperAmp= 100; //185mV for 5A, 100mV for 10A, 66mV for 20A
double Vpp= 0;
double Vp = 0;
double Vrms = 0;
double Irms= 0; 
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
void setup() {
Serial.begin(9600);
WiFi.begin(WLAN_SSID, WLAN_PASS);
while (WiFi.status()!=WL_CONNECTED)
{
Serial.println("Connecting...");
delay(1000);
}
Serial.println("Connected");
}
void loop() {
MQTT_connect();
Vpp = getVPP();
Vp = Vpp/2.0;
Vrms=Vp*0.707;
Irms=((Vrms * 1000)/mVperAmp)-0.09;
Serial.print(Irms);
Serial.println(" Amps");
if (! photocell.publish(Irms))
{
Serial.println("Failed");
}
else
{
Serial.println("OK!");
}
delay (2000);
}
double getVPP ()
{
double result;
int readValue;
int maxValue = 0;
int minValue = 1024;
uint32_t start_time = millis();
while ((millis()-start_time) < 1000) //sample for 1
{
   readValue = analogRead(sensorIn);
   // see if you have a new maxValue
   if (readValue> maxValue)
   {
     maxValue=readValue;
    }
   if(readValue < minValue)
   {
    minValue = readValue;
   }
}
result=((maxValue-minValue)*5.0)/1024.0;
return result;
}
