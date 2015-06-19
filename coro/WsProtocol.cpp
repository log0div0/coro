
#include "coro/WsProtocol.h"
#include "coro/Format.h"
#ifndef _MSC_VER
#include <openssl/sha.h>
#include "coro/base64.h"
#endif


WsMessage::OpCode WsMessage::opCode() const {
	return _opCode;
}


Buffer::Iterator WsMessage::begin() const {
	return _begin;
}


Buffer::Iterator WsMessage::payloadBegin() const {
	return _payloadBegin;
}


Buffer::Iterator WsMessage::payloadEnd() const {
	return _payloadEnd;
}


Buffer::Iterator WsMessage::end() const {
	return _end;
}


size_t WsMessage::payloadLength() const {
	return _payloadEnd - _payloadBegin;
}


const std::string WsProtocol::handshakeRequest = "GET %1% HTTP/1.1\r\n\
Connection: Upgrade\r\n\
Upgrade: websocket\r\n\
\r\n";


void WsProtocol::writeHandshakeRequest(const std::string& path, Buffer& buffer) const {
	std::string request = Format(handshakeRequest, path);
	buffer.pushBack(request.begin(), request.end());
}


const std::string WsProtocol::handshakeResponse = "HTTP/1.1 101 Switching Protocols\r\n\
Connection: Upgrade\r\n\
Upgrade: websocket\r\n\
Sec-WebSocket-Accept: %1%\r\n\
\r\n";


const HttpHeaders& WsProtocol::handshakeHeaders() const {
	return _headers;
}

std::string WsProtocol::generateResponse() {
#ifndef _MSC_VER
	if (_headers.count("Sec-WebSocket-Key")) {
		std::string acceptKey;
		acceptKey += _headers["Sec-WebSocket-Key"];
		acceptKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; //RFC6544_MAGIC_KEY
		std::vector<uint8_t> acceptKeyHash(20, 0);
		SHA1(reinterpret_cast<const uint8_t*>(acceptKey.data()), acceptKey.size(),
			acceptKeyHash.data());
		std::string acceptKeyBase64 = base64::encode(acceptKeyHash);
		return Format(handshakeResponse, acceptKeyBase64);
	}
	else
	{
		return Format(handshakeResponse, "-");
	}
#else
	return Format(handshakeResponse, "-");
#endif
}
