#include "Arduino.h"
#include "eISCP.h"

void eISCP_Message::decode(char* cmsg) {
	// Strip terminating characters
	String msg = String(strtok(cmsg, "\x1A\0\r\n"));
	// Check if we have a valid eISCP message
	if(msg.substring(0, 2) == "!1")
		valid = true;
	else 
		valid = false;
	content = msg.substring(2);
}

String eISCP_Message::encode() {
	// Add prefix and a terminating character
	return "!1" + content + '\r';
}


eISCP_Message::eISCP_Message(String _content){
	content = _content;
	valid = false;
}


eISCP::eISCP(const char _ip_address[], int _port, Client* _client) {
	ip_address = _ip_address;
	port = _port;
	client = _client;
	callback = nullptr;
}


void eISCP::set_callback(void (*_callback)(eISCP_Message)) {
	callback = _callback;
}


bool eISCP::connected() {
	if(!client->connected() && !client->available())
		if (!client->connect(ip_address, port))
			return false;
	return true;
}


void eISCP::handle() {
	if(!connected()) return;
	// Read all incoming messages
	while(client->available() >= 16){
		yield();
		get_packet();
	}
}


bool eISCP::send(String command) {
	eISCP_Message message(command);
	return send_packet(&message);
}


bool eISCP::send_packet(eISCP_Message* message) {
	if(!connected()){
		return false;
	}
	eISCP_Header header;
	header.size_body = __builtin_bswap32(message->encode().length());
	const byte* p = (const byte*) &header;
	for (unsigned int i = 0; i < sizeof header; i++)
		client->write(*p++);
	client->print(message->encode());
	return true;
}


void eISCP::get_packet() {
	if(!connected()) return;
	eISCP_Header header;
	if(client->readBytes((byte*) &header, 16) == 16){
		// Swap endianness
		header.size_header = __builtin_bswap32(header.size_header);
		header.size_body = __builtin_bswap32(header.size_body);
		// Check if header is valid
		if(strncmp(header.magic, "ISCP", 4) != 0 || header.size_header != 16)
			return;
	} else return;
	char* buffer = new char[header.size_body];
	unsigned long timeout = millis(), index = 0;

	// Read message to buffer byte by byte
	while(index < header.size_body){
		yield();
		if (millis() - timeout > eISCP_REQUEST_TIMEOUT)
			break;
		byte b = client->read();
		if(b) buffer[index++] = b;
	}
	
	eISCP_Message message;
	message.decode(buffer);
	delete[] buffer;

	// Check if the whole message was read
	if(index != header.size_body)
		message.valid = false;

	// Call the callback function if present
	if(callback != nullptr) 
		callback(message);
}