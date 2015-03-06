
#pragma once


#include "HttpParsers.h"
#include "Buffer.h"
#include "WsPayloadLength.h"

/**
  * @warning Используется Buffer::Iterator, изменение буфера запрещено на время работы с WsMessage
  */
class WsMessage {
public:
	friend class WsProtocol;

	enum class OpCode: uint8_t {
		Undefined = 0x00,
		Text = 0x01,
		Binary = 0x02,
		Close = 0x08
	};

	OpCode opCode() const;

	Buffer::Iterator begin() const;
	Buffer::Iterator payloadBegin() const;
	size_t payloadLength() const;
	Buffer::Iterator payloadEnd() const;
	Buffer::Iterator end() const;

private:
	uint8_t* payload() const; //< DO NOT DO IT!

private:
	OpCode _opCode = OpCode::Undefined;
	Buffer::Iterator _begin, _end, _payloadBegin, _payloadEnd;
};


class WsProtocol {
public:
	/**
	  * @throw std::runtime_error eсли [begin, end) содержит некорректное сообщение
	  */
	template <typename Iterator>
	void readMessage(const Iterator& begin, const Iterator& end, WsMessage* message) const {
		Iterator it = begin;
		uint8_t finFlag = *it & 0x80;
		auto opCode = static_cast<WsMessage::OpCode>(*it & 0x0f);
		++it;
		uint8_t maskFlag = *it & 0x80;
		uint64_t payloadLength;
		it = ReadWsPayloadLength(it, end, &payloadLength);

		assert(finFlag); //< unused

		if (maskFlag) {
			std::vector<uint8_t> maskingKey(it, it + 4);
			it += 4;
			for(uint64_t i = 0; i < payloadLength; ++i) {
				*(it + i) ^= maskingKey[i % 4];
			}
		}

		message->_opCode = opCode;
		message->_begin = begin;
		message->_payloadBegin = it;
		*(it + (payloadLength - 1)); // загрузим payload (если это NetworkIterator)
		message->_payloadEnd = it + payloadLength;
		message->_end = it + payloadLength;
	}

	// Обрамляют данные в буфере так, чтобы получилось ws сообщение
	void writeMessage(WsMessage::OpCode opCode, Buffer& buffer) const;
};


class WsClientProtocol: public WsProtocol {
public:
	WsClientProtocol(const std::string& url = "/");

	void writeHandshakeRequest(Buffer& buffer) const;
	/**
	  * @throw std::runtime_error если [begin, end) содержит некорректный ответ
	  */
	template <typename Iterator>
	Buffer::Iterator readHandshakeResponse(const Iterator& begin, const Iterator& end) const {
		Iterator it = begin;
		HttpResponseParser<Iterator> parser;
		HttpResponse response;
		if (!boost::spirit::qi::parse(it, end, parser, response)) {
			throw std::runtime_error("Invalid HTTP response");
		};
		return it;
	}

private:
	static const std::string handshakeRequest;
	std::string _url;
};


class WsServerProtocol: public WsProtocol {
public:
	/**
	  * @param [begin, end) - client HTTP request
	  * @throw std::runtime_error если [begin, end) содержит некорректный запрос
	  */
	template <typename Iterator>
	Buffer::Iterator doHandshake(const Iterator& begin, const Iterator& end, Buffer& output) {
		Iterator it = begin;
		HttpRequestParser<Iterator> parser;
		HttpRequest request;
		if (!boost::spirit::qi::parse(it, end, parser, request)) {
			throw std::runtime_error("Invalid HTTP request");
		};
		_headers = std::move(request.headers);

		std::string response = generateResponse();
		output.pushBack(response.begin(), response.end());

		return it;
	}

	const HttpHeaders& handshakeHeaders() const;

private:

	std::string generateResponse();

	static const std::string handshakeResponse;
	HttpHeaders _headers;
};