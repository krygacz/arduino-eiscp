/**
 * arduino-eiscp
 * Arduino library for controlling eISCP-based devices
 * 
 * visit https://github.com/krygacz/arduino-eiscp for more information
 * 
*/

#include <ESP8266WiFi.h>
#include <eISCP.h>

const char* ssid      = "your-ssid";
const char* password  = "your-password";
const char* device_ip = "your-eISCP-device-ip";
int device_port       = 60128;    // 60128 for Onkyo A/V receivers

WiFiClient client;
eISCP device(device_ip, device_port, &client);

void my_function(eISCP_Message message);

void setup() {
  // Connect to a WiFi network
  Serial.begin(115200);
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Check if we're connected to the device
  if(device.connected())
    Serial.println("Device is connected!");
  else 
    Serial.println("Device is not connected");

  // For more commands check out https://github.com/miracle2k/onkyo-eiscp/blob/master/eiscp-commands.yaml
  String command = "MVLUP";
  
  // We can send messages by just passing a string to the send() method:
  if(device.send(command))
    Serial.println("Message sent!");
  else 
    Serial.println("There was an error sending the message");

  // Let's set a callback so that we can receive messages back from our device
  // Here you can pass any method that takes eISCP_Message as argument, like my_function
  device.set_callback(my_function);
}

void loop() {
  // The following line is necessary to receive messages
  device.handle();
}

void my_function(eISCP_Message message){
  if(message.valid){
    // Do something with the received message
    Serial.println("Message received: " + message.content);
  } else {
    // The message is probably corrupted
    Serial.println("Message corrupted: " + message.content);
  }
}