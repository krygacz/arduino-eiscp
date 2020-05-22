# arduino-eiscp
Arduino library for controlling eISCP-based devices (eg. Onkyo and Pioneer receivers)
## Installation 

* Download the latest release from [here](https://github.com/krygacz/arduino-eiscp/releases)
* Open Arduino IDE, click ```Sketch``` -> ```Include Library``` -> ```Add .ZIP Library``` and select the downloaded ZIP file

You can also install the library using [PlatformIO Library Manager](https://platformio.org/lib/show/7370/arduino-eiscp)
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

### Checking connection
To check connection with your device, use ```device.connected()```

### Sending messages
To send messages use the ```send()``` function:
```c++
device.send("PWR01");
```
It will return **true** if the message was sent or **false** if there was a problem
### Receiving messages
To receive messages you need to specify a function that will be called every time a new message arrives:
```c++
device.set_callback(function)
```
The callback function needs to be of type **void** and take **eISCP_Message** as its only argument (see example).

**eISCP_Message** has two properties:
* ```content``` *(String)* - the received message
* ```valid``` *(bool)* - **true** if the message is valid or **false** if there was a problem getting the message (eg. due to a network issue)

#### Important!
You need to put ```device.handle()``` in the ```loop()``` part of your Arduino sketch, otherwise you won't be able to receive messages.

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
```
## Credits
eISCP packet structure based on [onkyo-eiscp](https://github.com/miracle2k/onkyo-eiscp), huge thanks to @miracle2k!