#include "Arduino.h"
#include "eISCP.h"

void eISCP_Message::decode(char* cmsg) {
	// Strip terminating characters
	String msg = String(strtok(cmsg, "\x1A\0\r\n"));
	// Check if we have a valid eISCP message
	if(msg.substring(0, 2) != "!1"){
		status = eISCP_MESSAGE_INVALID;
		return;
	}
	content = msg.substring(2);
}

String eISCP_Message::encode() {
	// Add prefix and a terminating character
	return "!1" + content + '\r';
}


eISCP::eISCP(const char ip_address[], int port, Client* client) {
	_ip_address = ip_address;
	_port = port;
	_client = client;
	_client->connect(_ip_address, _port);
}


bool eISCP::connected() {
	if(!_client->connected() && !_client->available())
		if (!_client->connect(_ip_address, _port))
			return false;
	return true;
}


void eISCP::setCallback(void (*callback)(eISCP_Message)) {
	_callback = callback;
}


void eISCP::handle() {
	if(!connected()) return;
	// Read all incoming messages
	while(_client->available() >= 16){
		int status = get_packet();
		// If header or body is corrupted 
		// we need to flush the buffer
		if(status >= 10)
			while(_client->read()) yield();
		yield();
	}
	// Send queued message
	if(_nextMessage->status == eISCP_MESSAGE_PENDING)
		send_packet(_nextMessage);
}


void eISCP::send(String command) {
	eISCP_Message message;
	message.content = command;
	message.status = eISCP_MESSAGE_PENDING;
	_nextMessage = &message;
}


void eISCP::send(eISCP_Message& message) {
	message.status = eISCP_MESSAGE_PENDING;
	_nextMessage = &message;
}


void eISCP::send_packet(eISCP_Message* message) {
	String body = message->encode();
	// Header structure based on https://github.com/miracle2k/onkyo-eiscp/blob/master/eiscp/core.py
	// Integers need to be swapped to Big Endian
	struct __attribute__ ((packed)) {
		char t1[4] = {'I', 'S', 'C', 'P'};
		uint32_t i1 = __builtin_bswap32(16);
		uint32_t i2 = __builtin_bswap32(body.length());
		signed char b1 = 0x01;
		char t2[3] = {'\x00','\x00','\x00'};
	} header;
	
	// Send header byte by byte
	const byte* p = (const byte*) &header;
	for (unsigned int i = 0; i < sizeof header; i++)
		_client->write(*p++);

	_client->print(body);
	message->status = eISCP_MESSAGE_SENT;
}


int eISCP::get_packet() {
	struct __attribute__ ((packed)) {
		char t1[4];
		uint32_t i1;
		uint32_t i2;
		signed char b1;
		char t2[3];
	} response;

	// Unpack header to struct
	if(_client->readBytes((byte*) &response, 16) == 16){
		// Swap endianness
		response.i1 = __builtin_bswap32(response.i1);
		response.i2 = __builtin_bswap32(response.i2);
		// Check if header is valid
		if(strncmp(response.t1, "ISCP", 4) != 0 || response.i1 != 16)
			return eISCP_ERR_CORRUPTED_HEADER;
	} else return eISCP_ERR_CORRUPTED_HEADER;
	static const unsigned int msg_size = response.i2;
	char buffer[msg_size];
	unsigned long timeout = millis();
	unsigned int index = 0;

	// Read message to buffer byte by byte
	while(index < msg_size){
		if (millis() - timeout > eISCP_REQUEST_TIMEOUT)
			break;
		byte b = _client->read();
		if(b) buffer[index++] = b;
		yield();
	}
	
	eISCP_Message message;
	message.status = eISCP_MESSAGE_RECEIVED;
	message.decode(buffer);

	// Call the callback function if present
	if(_callback != nullptr) 
		_callback(message);

	// Check if the whole message was read
	if(index != msg_size) 
		return eISCP_ERR_CORRUPTED_MESSAGE;
	return eISCP_SUCCESS;
}