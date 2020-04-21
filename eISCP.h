#ifndef eISCP_h
#define eISCP_h

#include "Arduino.h"
#include "Client.h"
#include "WifiClient.h"
#include "EthernetClient.h"

class eISCP {
  public:
    /** Quick eISCP initialization using port 60128 and WiFiClient
      @param ip_address IP address of your eISCP device
    */
    eISCP(String ip_address);

    /** Initialize eISCP using a custom port and EthernetClient instance
      @param ip_address IP address of your eISCP device
      @param port Port of your eISCP device
      @param client EthernetClient instance
    */
    eISCP(String ip_address, int port, EthernetClient client);

    /** Initialize eISCP using a custom port and WiFiClient instance
      @param ip_address IP address of your eISCP device
      @param port Port of your eISCP device
      @param client WiFiClient instance
    */
    eISCP(String ip_address, int port, WiFiClient client);

  private:
    Client* _client;
    String _ip_address;
    int _port;
};

#endif