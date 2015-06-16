
#include <boost/test/unit_test.hpp>
#include "coro/WsProtocol.h"


namespace qi = boost::spirit::qi;


const std::string certificate = "-----BEGIN CERTIFICATE-----\r\n\
MIIBxDCCAXECAQIwCgYGKoUDAgIDBQAwdzELMAkGA1UEBhMCUlUxDzANBgNVBAgM\r\n\
Bk1vc2NvdzEPMA0GA1UEBwwGTW9zY293MRYwFAYDVQQKDA1TZWN1cml0eSBDb2Rl\r\n\
MRIwEAYDVQQLDAlDb250aW5lbnQxGjAYBgNVBAMMEXJvb3RfY3Byb18yMDAxX3Yz\r\n\
MB4XDTE0MTAyMTA5MTIyN1oXDTE1MTAyMTA5MTIyN1owYjEnMCUGA1UEAx4eAHQA\r\n\
cwBfAGMAcAByAG8AXwAyADAAMAAxAF8AdgAzMRYwFAYDVQQKEw1TZWN1cml0eSBD\r\n\
b2RlMRIwEAYDVQQLEwlDb250aW5lbnQxCzAJBgNVBAYTAlJVMGMwHAYGKoUDAgIT\r\n\
MBIGByqFAwICJAAGByqFAwICHgEDQwAEQNxehzo1K5mn37UDueeVwmWD6NBAYjQH\r\n\
7ouJym02CDHNksRQesaf5Tx8pPHmgEzx16oW/WC9U8KlqB8PIn5UB8MwCgYGKoUD\r\n\
AgIDBQADQQDnjbrWeyjOPdjgASrpusMkLGyCSyJOfZREj0tDl4nDcqFA+5pRjEDJ\r\n\
apUUsj8y2zmp2Iu32HeGI7M5XG5c4zeV\r\n\
-----END CERTIFICATE-----";


const std::string basicClientHandshake = "GET /some/path HTTP/1.1\r\n\
Connection: Upgrade\r\n\
Upgrade: websocket\r\n\
\r\n";


const std::string clientHandshake = "GET /some/path HTTP/1.1\r\n\
Connection: Upgrade\r\n\
Upgrade: websocket\r\n\
Sec-WebSocket-Key: /SkdYOM3GiJMn/2FJ4QKbw==\r\n\
X-Client-EndPoint: 192.168.0.1:1234\r\n\
X-SSL-Client-Cert: " + certificate + "\r\n\
\r\n";

const std::string serverHandshake = "HTTP/1.1 101 Switching Protocols\r\n\
Connection: Upgrade\r\n\
Upgrade: websocket\r\n\
Sec-WebSocket-Accept: 4JYIQyX14UVSlE0hKjBlVa51jiE=\r\n\
\r\n";

const std::vector<uint8_t> textMessage { 0x81, 0x04, 'a', 'b', 'c', 'd'};
const std::vector<uint8_t> binaryMessage { 0x82, 0x04, 0x01, 0x02, 0x03, 0x04 };
const std::vector<uint8_t> maskedMessage {
	0x81, 0x8b, 0xc8, 0x22, 0x2b, 0x7c, 0xa0, 0x47,
	0x47, 0x10, 0xa7, 0x02, 0x5c, 0x13, 0xba, 0x4e,
	0x4f
};


const std::string realRequestHeaders = "Host: localhost:44442\r\n\
Connection: Upgrade\r\n\
Pragma: no-cache\r\n\
Cache-Control: no-cache\r\n\
Upgrade: websocket\r\n\
Origin: null\r\n\
Sec-WebSocket-Version: 13\r\n\
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.111 Safari/537.36\r\n\
Accept-Encoding: gzip, deflate, sdch\r\n\
Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\r\n\
Sec-WebSocket-Key: /SkdYOM3GiJMn/2FJ4QKbw==\r\n\
Sec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\r\n";


BOOST_AUTO_TEST_SUITE(SuiteWsProtocol)


BOOST_AUTO_TEST_CASE(TestHttpHeadersParser) {
	Buffer buffer("AAAA: 1111\r\nBBBB: 2222\r\n");
	HttpHeadersParser<Buffer::Iterator> parser;
	HttpHeaders headers;

	BOOST_REQUIRE(qi::parse(buffer.begin(), buffer.end(), parser, headers));
	BOOST_REQUIRE(headers.size() == 2);
	BOOST_REQUIRE(headers.at("AAAA") == "1111");
}


BOOST_AUTO_TEST_CASE(TestHttpHeadersParser2) {
	Buffer buffer(realRequestHeaders);
	HttpHeadersParser<Buffer::Iterator> parser;
	HttpHeaders headers;

	BOOST_REQUIRE(qi::parse(buffer.begin(), buffer.end(), parser, headers));

	BOOST_REQUIRE(headers.size() == 12);
	BOOST_REQUIRE(headers.at("Upgrade") == "websocket");
}


BOOST_AUTO_TEST_CASE(TestHttpRequestParser) {
	Buffer buffer(clientHandshake);
	HttpRequestParser<Buffer::Iterator> parser;
	HttpRequest request;

	BOOST_REQUIRE(qi::parse(buffer.begin(), buffer.end(), parser, request));
	BOOST_REQUIRE(request.headers.size() == 5);
	BOOST_REQUIRE(request.headers["X-SSL-Client-Cert"] == certificate);


	Buffer buffer2("aaaa");
	BOOST_REQUIRE(!qi::parse(buffer2.begin(), buffer2.end(), parser, request));
}


BOOST_AUTO_TEST_CASE(TestHttpResponseParser) {
	Buffer buffer(serverHandshake);
	HttpResponseParser<Buffer::Iterator> parser;
	HttpResponse response;

	BOOST_REQUIRE(qi::parse(buffer.begin(), buffer.end(), parser, response));
	BOOST_REQUIRE(response.code == 101);
	BOOST_REQUIRE(response.headers.size() == 3);


	Buffer buffer2("aaaa");
	BOOST_REQUIRE(!qi::parse(buffer2.begin(), buffer2.end(), parser, response));
}


BOOST_AUTO_TEST_CASE(TestWriteText) {
	WsProtocol protocol;

	Buffer buffer;
	buffer.pushBack(textMessage.begin() + 2, textMessage.end());
	protocol.writeMessage(WsMessage::OpCode::Text, buffer);
	BOOST_REQUIRE(buffer == Buffer(textMessage));
}


BOOST_AUTO_TEST_CASE(TestWriteBinary) {
	WsProtocol protocol;

	Buffer buffer;
	buffer.pushBack(binaryMessage.begin() + 2, binaryMessage.end());
	protocol.writeMessage(WsMessage::OpCode::Binary, buffer);
	BOOST_REQUIRE(buffer == Buffer(binaryMessage));
}


BOOST_AUTO_TEST_CASE(TestReadMaskedMessage) {
	std::string expected = "hello world";
	WsProtocol protocol;
	Buffer buffer;
	buffer.pushBack(maskedMessage.begin(), maskedMessage.end());
	WsMessage message = protocol.readMessage(buffer.begin(), buffer.end());
	BOOST_REQUIRE(message.payloadLength() == expected.size());
	BOOST_REQUIRE(std::equal(expected.begin(), expected.end(), message.payloadBegin()));
}


#ifndef _MSC_VER
BOOST_AUTO_TEST_CASE(TestServerSuccessfulConnection) {
	WsProtocol protocol;

	Buffer inputBuffer, outputBuffer;
	inputBuffer.pushBack(clientHandshake.begin(), clientHandshake.end());
	inputBuffer.pushBack(textMessage.begin(), textMessage.end());
	inputBuffer.pushBack(binaryMessage.begin(), binaryMessage.end());

	BOOST_REQUIRE_NO_THROW(inputBuffer.popFront(
		protocol.doHandshake(inputBuffer.begin(), inputBuffer.end(), outputBuffer)));

	BOOST_REQUIRE(inputBuffer.usefulDataSize() == textMessage.size() + binaryMessage.size());
	BOOST_REQUIRE(outputBuffer.usefulDataSize() == serverHandshake.size());
	BOOST_REQUIRE(std::equal(outputBuffer.begin(), outputBuffer.end(), serverHandshake.begin()));

	auto& headers = protocol.handshakeHeaders();
	BOOST_REQUIRE(headers.size() == 5);
	BOOST_REQUIRE(headers.at("Connection") == "Upgrade");

	WsMessage message;
	BOOST_REQUIRE_NO_THROW(message = protocol.readMessage(inputBuffer.begin(), inputBuffer.end()));
	BOOST_REQUIRE(inputBuffer.usefulDataSize() == textMessage.size() + binaryMessage.size());

	BOOST_REQUIRE(message.opCode() == WsMessage::OpCode::Text);
	BOOST_REQUIRE(message.payloadLength() == 4);
	std::equal(message.payloadBegin(), message.payloadEnd(), textMessage.begin() + 1);

	inputBuffer.popFront(message.end());
	BOOST_REQUIRE(inputBuffer.usefulDataSize() == binaryMessage.size());

	BOOST_REQUIRE_NO_THROW(message = protocol.readMessage(inputBuffer.begin(), inputBuffer.end()));
	BOOST_REQUIRE(inputBuffer.usefulDataSize() == binaryMessage.size());

	BOOST_REQUIRE(message.opCode() == WsMessage::OpCode::Binary);
	BOOST_REQUIRE(message.payloadLength() == 4);
	BOOST_REQUIRE(std::equal(message.payloadBegin(), message.payloadEnd(), binaryMessage.begin() + 2));

	inputBuffer.popFront(message.end());
	BOOST_REQUIRE(inputBuffer.usefulDataSize() == 0);
}
#endif


BOOST_AUTO_TEST_CASE(TestClientSuccessfulConnection) {
	WsProtocol protocol;

	Buffer inputBuffer, outputBuffer;

	protocol.writeHandshakeRequest("/some/path", outputBuffer);

	BOOST_REQUIRE(outputBuffer.usefulDataSize() == basicClientHandshake.size());
	BOOST_REQUIRE(std::equal(outputBuffer.begin(), outputBuffer.end(),
		basicClientHandshake.begin()));

	inputBuffer.pushBack(serverHandshake.begin(), serverHandshake.end());
	BOOST_REQUIRE_NO_THROW(inputBuffer.popFront(
		protocol.readHandshakeResponse(inputBuffer.begin(), inputBuffer.end())));

	BOOST_REQUIRE(inputBuffer.usefulDataSize() == 0);
}


BOOST_AUTO_TEST_CASE(TestServerInvalidHTTPRequest) {
	WsProtocol protocol;
	Buffer inputBuffer("aaaa"), outputBuffer;
	BOOST_REQUIRE_THROW(protocol.doHandshake(inputBuffer.begin(), inputBuffer.end(), outputBuffer),
		std::runtime_error);
}


BOOST_AUTO_TEST_SUITE_END()
