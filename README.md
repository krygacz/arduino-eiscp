# arduino-eiscp
Arduino library for controlling eISCP-based devices (eg. Onkyo and Pioneer receivers)
## Installation 
* Download the latest release from [here](https://github.com/krygacz/arduino-eiscp/releases)
* Open Arduino IDE, click ```Sketch``` -> ```Include Library``` -> ```Add .ZIP Library``` and select the downloaded ZIP file
## Usage
Include the library in your code:
```c++
#include <eISCP.h>
```
### Initialization
First, we need to create an eISCP instance. 
In this example we'll be connecting to a device with IP ```192.168.100.8```
and port ```60128```.

We can initialize eISCP with ```WiFiClient```:
```c++
WiFiClient client_wifi;
eISCP device("192.168.100.8", 60128, &client_wifi);
```
or ```EthernetClient```:
```c++
EthernetClient cllient_eth;
eISCP device("192.168.100.8", 60128, &client_eth);
```

### Sending commands
To send commands use the ```send(String command)``` function:
```c++
device.send("PWR01");
```
Response codes:
* ```eISCP_SUCCESS``` - command sent successfully
* ```eISCP_ERR_CONNECTION_FAILED``` - eISCP request failed
* ```eISCP_ERR_CONNECTION_TIMEOUT``` - eISCP request timeout


## Example
```c++
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
```