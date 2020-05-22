#ifndef eISCP_h
#define eISCP_h

#include "Arduino.h"
#include "Client.h"

#define eISCP_REQUEST_TIMEOUT 500

// Header structure based on https://github.com/miracle2k/onkyo-eiscp/blob/master/eiscp/core.py
typedef struct __attribute__ ((packed)) {
  char magic[4] = {'I', 'S', 'C', 'P'};
  uint32_t size_header = __builtin_bswap32(16);
  uint32_t size_body = __builtin_bswap32(0);
  signed char version = 0x01;
  char reserved[3] = {'\x00','\x00','\x00'};
} eISCP_Header;

class eISCP_Message {
  public:
    eISCP_Message(String _content = "");
    String encode();
    void decode(char* cmsg);
    String content;
    bool valid;
};

class eISCP {
  public:
    eISCP(const char ip_address[], int port, Client* client);
    bool send(String command);
    bool connected();
    void set_callback(void (*_callback)(eISCP_Message));
    void handle();

  private:
    void get_packet();
    bool send_packet(eISCP_Message* message);
    void (*callback)(eISCP_Message);
    Client* client;
    const char* ip_address;
    int port;
};

#endif