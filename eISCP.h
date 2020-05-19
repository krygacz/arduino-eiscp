#ifndef eISCP_h
#define eISCP_h

#include "Arduino.h"
#include "Client.h"

#define eISCP_SUCCESS 0
#define eISCP_ERR_CONNECTION_FAILED 1
#define eISCP_ERR_CONNECTION_TIMEOUT 2
#define eISCP_ERR_CORRUPTED_HEADER 10
#define eISCP_ERR_CORRUPTED_MESSAGE 11

#define eISCP_MESSAGE_INITIALIZED 0
#define eISCP_MESSAGE_PENDING 1
#define eISCP_MESSAGE_SENT 2
#define eISCP_MESSAGE_RECEIVED 3
#define eISCP_MESSAGE_INVALID -1

#define eISCP_REQUEST_TIMEOUT 500

#define eISCP_MESSAGE_BUFFER_SIZE 50

class eISCP_Message {
  public:
    String encode();
    void decode(char* cmsg);
    String content;
    int status;
};

class eISCP {
  public:
    eISCP(const char ip_address[], int port, Client* client);
    void send(String command);
    void send(eISCP_Message* message);
    bool connected();
    void set_callback(void (*callback)(eISCP_Message));
    void handle();

  private:
    int get_packet();
    void send_packet(eISCP_Message* message);
    void enqueue(eISCP_Message* message);
    eISCP_Message** _buffer;
    unsigned int _buffer_index = 0;
    void (*_callback)(eISCP_Message);
    Client* _client;
    const char* _ip_address;
    int _port;
};

#endif