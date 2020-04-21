#include <ESP8266WiFi.h>
#include <eISCP.h>

const char* ssid      = "your-ssid";
const char* password  = "your-password";
const char* device_ip = "your-eISCP-device-ip";
int device_port       = 60128;    // 60128 for Onkyo A/V receivers

WiFiClient client;
eISCP device(device_ip, device_port, &client);

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
}

void loop() {
  // Command to be sent - eg. 'MVLUP' increments the volume on ONKYO receivers
  // For more commands check out https://github.com/miracle2k/onkyo-eiscp/blob/master/eiscp-commands.yaml
  String command = "MVLUP"; 
  
  if(device.send(command) == eISCP_SUCCESS){
    // Command was sent successfully
    Serial.println("Command sent!");
  } else {
    // There was an error
    Serial.println("Error!");
  }
  
  // Wait 60s
  delay(60000);
}
