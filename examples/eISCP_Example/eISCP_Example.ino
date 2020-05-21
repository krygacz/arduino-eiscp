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
// or eISCP device(device_ip, device_port, &client, true) to enable batch processing
// If batch processing is enabled, outgoing messages are put in a queue and sent on device.handle()

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

  // First, check if we're connected to the device
  // This function will try to reconnect before returning false
  if(device.connected())
    Serial.println("Device is connected!");
  else 
    Serial.println("Device is not connected");

  // For more commands check out https://github.com/miracle2k/onkyo-eiscp/blob/master/eiscp-commands.yaml
  String command = "MVLUP";
  
  // We can send messages by just passing a string to the send() method:
  device.send(command);

  // Or create an eISCP_Message instance first...
  eISCP_Message* my_message = new eISCP_Message(command);
  device.send(my_message);
  
  // ...so that we can later check the status of our message.
  // This is especially useful if batch processing is enabled
  switch(my_message->status){
    case eISCP_MESSAGE_PENDING:
      Serial.println("Message is waiting to be sent");
      break;
    case eISCP_MESSAGE_SENT:
      Serial.println("Message sent");
      break;
    case eISCP_MESSAGE_NOT_SENT:
      Serial.println("There was an error sending the message");
      break;
  }

  // Let's set a callback so that we can receive messages back from our device
  // Here you can pass any method that takes eISCP_Message as argument, like my_function
  device.set_callback(my_function);
}

void loop() {
  // Process eISCP tasks
  // The library won't work without this line!
  device.handle();
}

void my_function(eISCP_Message* message){
  switch(message->status){
    case eISCP_MESSAGE_RECEIVED:
      // Do something with the received message
      Serial.println("Message received: " + message->content);
      break;
    case eISCP_MESSAGE_INVALID:
      // The message is probably corrupted
      Serial.println("Message corrupted: " + message->content);
      break;
  }
}