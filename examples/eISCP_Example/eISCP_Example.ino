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

  // For more commands check out https://github.com/miracle2k/onkyo-eiscp/blob/master/eiscp-commands.yaml
  String command = "MVLUP";
  
  // We can send messages by just passing a string to the send() method:
  device.send(command);

  // Or create an eISCP_Message instance first...
  eISCP_Message* my_message = new eISCP_Message;
  my_message->content = command;
  device.send(my_message);
  
  // ...so that we can later check the status of our message:
  if(my_message->status == eISCP_MESSAGE_PENDING)
    Serial.println("Command pending");

  // Let's set a callback so that we can receive messages back from our device
  // Here you can pass any method that takes eISCP_Message as argument, like my_function
  device.set_callback(my_function);
}

void loop() {
  // Process eISCP tasks
  // The library won't work without this line!
  device.handle();
}

void my_function(eISCP_Message message){
  if(message.status == eISCP_MESSAGE_INVALID){
    // The message is probably corrupted
    return;
  }
  // Do something with the received message
  Serial.println("Message received: " + message.content);
}