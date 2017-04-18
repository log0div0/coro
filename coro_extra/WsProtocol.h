
#pragma once


#include "coro_extra/HttpParsers.h"
#include "coro_extra/Buffer.h"
#include "coro_extra/WsPayloadLength.h"

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
	void writeHandshakeRequest(const std::string& path, Buffer& buffer) const;

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

	/**
	  * @throw std::runtime_error если [begin, end) содержит некорректный ответ
	  */
	template <typename Iterator>
	Buffer::Iterator readHandshakeResponse(const Iterator& begin, const Iterator& end) {
		Iterator it = begin;
		HttpResponseParser<Iterator> parser;
		HttpResponse response;
		if (!boost::spirit::qi::parse(it, end, parser, response)) {
			throw std::runtime_error("Invalid HTTP response");
		};
		_headers = std::move(response.headers);
		return it;
	}

	const HttpHeaders& handshakeHeaders() const;

	/**
	  * @throw std::runtime_error eсли [begin, end) содержит некорректное сообщение
	  */
	template <typename Iterator>
	WsMessage readMessage(const Iterator& begin, const Iterator& end) const {
		Iterator it = begin;
		uint8_t finFlag = *it & 0x80;
		auto opCode = static_cast<WsMessage::OpCode>(*it & 0x0f);
		++it;
		uint8_t maskFlag = *it & 0x80;
		uint64_t payloadLength;
		it = ReadWsPayloadLength(it, end, &payloadLength);

		if (!finFlag) {
			throw std::runtime_error("Websocket message segmentation is not implemented");
		}

		if (maskFlag) {
			std::vector<uint8_t> maskingKey(it, it + 4);
			it += 4;
			for(uint64_t i = 0; i < payloadLength; ++i) {
				*(it + i) ^= maskingKey[i % 4];
			}
		}

		WsMessage message;
		message._opCode = opCode;
		message._begin = begin;
		message._payloadBegin = it;
		*(it + (payloadLength - 1)); // загрузим payload (если это IoHandleIterator)
		message._payloadEnd = it + payloadLength;
		message._end = it + payloadLength;
		return message;
	}

	// Обрамляют данные в буфере так, чтобы получилось ws сообщение
	void writeMessage(WsMessage::OpCode opCode, Buffer& buffer) const;

private:
	static const std::string handshakeRequest;
	static const std::string handshakeResponse;

	std::string generateResponse();

	HttpHeaders _headers;
};
