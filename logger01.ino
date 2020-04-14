
#include <MsTimer2.h>
#include <Ethernet2.h>
// -------------A/D---------------
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int inputPin = A2;              // A/D port
// -------------Mode--------------
const int led_pin = 13;         // default to pin 13
int eth_mail = 0;                   // mail_off
int mail_1st = 1;
// -----------Ethernet------------
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xB4, 0xDF }; // Your Ethernet Shield MAC Address
IPAddress ip(192, 168, 179, 3);
IPAddress gateway(192, 168, 179, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetClient client;

void flash()
{
  static boolean output = HIGH;  // LED

  total = total - readings[readIndex];            // subtract the last reading:
  readings[readIndex] = analogRead(inputPin);     // read from the sensor:
  total = total + readings[readIndex];            // add the reading to the total:
  readIndex = readIndex + 1;                      // advance to the next position in the array:
  if (readIndex >= numReadings) {                 // if we're at the end of the array...
    readIndex = 0;                                // ...wrap around to the beginning:
  }
  average = total / numReadings;                  // calculate the average:
  //Serial.println(average);                        // send it to the computer as ASCII digits
  delay(1);                                       // delay in between reads for stability
  digitalWrite(led_pin, output); // LED
}

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);                                       // initialize serial communication with computer:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;                              // initialize all the readings to 0:
  }  
  MsTimer2::set(10, flash);                       // 20ms period
  MsTimer2::start();                              // timer start
  
  Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip, gateway, subnet);
  }
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.println ();
}

void loop() {
  if (average < 500){
    if (mail_1st == 1){
      mail_1st = 0;
      if ( client.connect("maker.ifttt.com", 80) ){                       // put your main code here, to run repeatedly:
        Serial.println("connected");
        int bodyLength = 0;                                               // POST request
//String requestHead1 = "curl -X POST -H \"Content-Type: application/json\" -d '{\"value1\":\"1\",\"value2\":\"1\",\"value3\":\"1\"}
//' https://maker.ifttt.com/trigger/ShortCircuitDetected/with/key/bIJ1aLcucq4HE-YNmSI502 HTTP/1.1\r\n"
        String requestHead1 = "POST https://maker.ifttt.com/trigger/ShortCircuitDetected/with/key/bIJ1aLcucq4HE-YNmSI502 HTTP/1.1\r\n"
                               + String("Host: maker.ifttt.com\r\n") +
                              "Content-Type: application/json\r\n";                        
        String requestHead2 = "Content-Length: " + String(bodyLength) + "\r\n" + "\r\n";
        client.print(requestHead1 + requestHead2 );
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        delay( 1000 );
        Serial.println("--- request ---");
        Serial.println(requestHead1 + requestHead2);
        Serial.println("---------------");
        Serial.println("Sent message");
      }else{
        Serial.println("Error: Could not make a TCP connection");
      }
      Serial.println("--- response ---");
      while (client.available()){
        char c = client.read();
        Serial.print(c);
      }
      Serial.println("----------------");
      if (!client.connected()){
        client.stop();
      }
    }
  }else{
    mail_1st = 1;    
  }
}
