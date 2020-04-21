#ifndef eISCP_h
#define eISCP_h

#include "Arduino.h"
#include "Client.h"

#define eISCP_SUCCESS 0
#define eISCP_ERR_CONNECTION_FAILED 1
#define eISCP_ERR_CONNECTION_TIMEOUT 2

#define eISCP_REQUEST_TIMEOUT 5000

class eISCP 
{
  public:
    /** Initialize eISCP
      @param ip_address eiSCP IP Address
      @param port eiSCP port
      @param client Pointer to a WifiClient, EthernetClient or GSMCLient instance
    */
    eISCP(const char ip_address[], int port, Client* client);
    
    /** Send eISCP command
      @param command Command string
      @return eISCP_SUCCESS if successful, else eISCP_ERR_CONNECTION_FAILED or eISCP_ERR_CONNECTION_TIMEOUT
    */
    int send(String command);

  private:
    Client* _client;
    const char* _ip_address;
    int _port;
};

#endif