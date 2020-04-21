#include "Arduino.h"
#include "eISCP.h"

eISCP::eISCP(String ip_address) {
	_ip_address = ip_address;
	WiFiClient client;
	_client = &client;
	_port = 60128;
}

eISCP::eISCP(String ip_address, int port, EthernetClient client) {
	_client = &client;
	_ip_address = ip_address;
	_port = port;
}

eISCP::eISCP(String ip_address, int port, WiFiClient client) {
	_client = &client;
	_ip_address = ip_address;
	_port = port;
}
