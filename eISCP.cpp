#include "Arduino.h"
#include "eISCP.h"

eISCP::eISCP(const char ip_address[], int port, Client* client)
{
	_ip_address = ip_address;
	_port = port;
	_client = client;
}

int eISCP::send(String command)
{
	// Body and header structure based on https://github.com/miracle2k/onkyo-eiscp/blob/master/eiscp/core.py
	// Integers need to be swapped to Big Endian
	static String body = "!1" + command + '\r';
	struct __attribute__ ((packed)) {
		char t1[4] = {'I', 'S', 'C', 'P'};
		uint32_t i1 = __builtin_bswap32(16);
		uint32_t i2 = __builtin_bswap32(body.length());
		signed char b1 = 0x01;
		char t2[3] = {'\x00','\x00','\x00'};
	} header;

	if (!_client->connect(_ip_address, _port))
		return eISCP_ERR_CONNECTION_FAILED;
	
	// Send header byte by byte
	const byte* p = (const byte*) &header;
	for (unsigned int i = 0; i < sizeof header; i++)
		_client->write(*p++);
	
	_client->print(body);

	unsigned long timeout = millis();
	while (_client->available() == 0) {
		if (millis() - timeout > eISCP_REQUEST_TIMEOUT) {
			_client->stop();
			return eISCP_ERR_CONNECTION_TIMEOUT;
		}
	}
	_client->stop();
	return eISCP_SUCCESS;
}