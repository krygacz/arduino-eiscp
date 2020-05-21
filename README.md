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

#### Batch processing
If you don't want messages to be transmitted during every device.send(), you can initialize eISCP with the following line:
```c++
eISCP device("192.168.100.8", 60128, &client_wifi, true);
```
Outgoing messages will be added to a queue and processed on device.handle()

### Checking connection
To check connection with your device, use ```device.connected()```


### Sending messages
To send messages use the ```send()``` function:
```c++
device.send("PWR01");
```
If you want to keep track of the status of your message, create an ```eISCP_Message``` instance first:
```c++
eISCP_Message* my_message = new eISCP_Message("PWR01");
device.send(my_message);
```
Then you can check the status later using ```my_message->status```

Here are the ```eISCP_Message``` status codes:

* ```eISCP_MESSAGE_INITIALIZED``` - default value
* ```eISCP_MESSAGE_PENDING``` - message in queue
* ```eISCP_MESSAGE_SENT``` - message sent successfully
* ```eISCP_MESSAGE_RECEIVED``` - message received
* ```eISCP_MESSAGE_INVALID``` - message may be corrupted
* ```eISCP_MESSAGE_NOT_SENT``` - there was an error sending the message

### Receiving messages
To receive messages you need to specify a callback function that will be called every time a new message arrives using ```device.set_callback(function)```. The callback function needs to be of type **void** and take **eISCP_Message pointer** as its only argument (see example)

### Important!
You need to put ```device.handle()``` in the ```loop()``` part of your Arduino sketch, otherwise the library won't work

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
```
## Credits
eISCP packet structure based on [onkyo-eiscp](https://github.com/miracle2k/onkyo-eiscp), huge thanks to @miracle2k!