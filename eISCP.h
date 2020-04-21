#ifndef eISCP_h
#define eISCP_h

#include "Arduino.h"
#include "Client.h"
#include "WifiClient.h"
#include "EthernetClient.h"

#define eISCP_SUCCESS 0
#define eISCP_ERR_CONNECTION_FAILED 1
#define eISCP_ERR_CONNECTION_TIMEOUT 2

#define eISCP_REQUEST_TIMEOUT 5000

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

    /** Send eISCP command
      @param command Command to send to your eISCP device
      @return eISCP_SUCCESS if successful, else eISCP_ERR_CONNECTION_FAILED or eISCP_ERR_CONNECTION_TIMEOUT
    */
    int send(String command);
    
  private:
    Client* _client;
    String _ip_address;
    int _port;
};

#endif