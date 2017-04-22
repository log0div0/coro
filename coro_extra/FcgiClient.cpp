#include "coro_extra/FcgiClient.h"

class FastcgiParam
{
public:
	FastcgiParam(const std::string& name, const std::string& value)
	{
		size_t header_len = 0;
		if (name.length()>127)
		{
			header_len += 4;
		}
		else
		{
			header_len += 1;
		}
		if (value.length()>127)
		{
			header_len += 4;
		}
		else
		{
			header_len += 1;
		}

		m_buffer.resize(name.length()+value.length()+header_len);
		size_t buffer_ptr = 0;
		if (name.length()>127)
		{
			size_t b = name.length();
			m_buffer[buffer_ptr] = (uint8_t)((b >> 24) | 0x80);
			m_buffer[buffer_ptr+1] = (uint8_t)(b >> 16);
			m_buffer[buffer_ptr+2] = (uint8_t)(b >> 8);
			m_buffer[buffer_ptr+3] = (uint8_t)(b);
			buffer_ptr += 4;
		}
		else
		{
			m_buffer[buffer_ptr++] = (uint8_t)name.length();
		}
		if (value.length()>127)
		{
			size_t b = value.length();
			m_buffer[buffer_ptr] = (uint8_t)((b >> 24) | 0x80);
			m_buffer[buffer_ptr+1] = (uint8_t)(b >> 16);
			m_buffer[buffer_ptr+2] = (uint8_t)(b >> 8);
			m_buffer[buffer_ptr+3] = (uint8_t)(b);
			buffer_ptr += 4;
		}
		else
		{
			m_buffer[buffer_ptr++] = (uint8_t)value.length();
		}

		if (name.length()>0)
		{
			memcpy(&m_buffer[buffer_ptr], name.data(), name.length());
			buffer_ptr += name.length();
		}
		if (value.length()>0)
		{
			memcpy(&m_buffer[buffer_ptr], value.data(), value.length());
			buffer_ptr += value.length();
		}
	}

	std::vector<uint8_t> m_buffer;
};

FcgiHeader::FcgiHeader(FcgiRequestType t, size_t size)
{
	version = 1;
	type = (uint8_t)t;
	requestIdB1 = 0;
	requestIdB0 = 1;
	contentLengthB1 = (size >> 8) % 256;
	contentLengthB0 = size % 256;
	paddingLength = 0;
	reserved = 0;
	if (size > 65535)
	{
		throw std::runtime_error("Fcgi client: msg too big");
	}
}

void
FcgiClient::writeRequest(const std::string& method, const std::string &URI,
	const std::string &body, const std::string server, uint16_t port, Buffer& buffer)
{
	FcgiHeader beginRequestHeader(FcgiRequestType::BeginRequest, sizeof(FcgiBeginRequestBody));
	buffer.pushBack(beginRequestHeader.begin(), beginRequestHeader.end());

	FcgiBeginRequestBody beginBody;
	memset(&beginBody, 0, sizeof(beginBody));
	beginBody.roleB0 = 1;
	buffer.pushBack(beginBody.begin(), beginBody.end());

	const uint8_t *input = nullptr;
	const size_t input_size = body.size();
	if (method == "POST" || method == "PUT" || method == "DELETE")
	{
		input = (const uint8_t*)(body.data());
	}

	std::list<FastcgiParam> param_list;
	param_list.push_back(FastcgiParam("REQUEST_METHOD", method));
	param_list.push_back(FastcgiParam("SCRIPT_NAME", URI));
	param_list.push_back(FastcgiParam("REQUEST_URI", URI));
	param_list.push_back(FastcgiParam("SERVER_NAME", server));
	param_list.push_back(FastcgiParam("SERVER_PORT", std::to_string(port)));
	param_list.push_back(FastcgiParam("SERVER_PROTOCOL", "HTTP/1.1"));
	if (method == "POST" || method == "PUT" || method == "DELETE")
	{
		param_list.push_back(FastcgiParam("CONTENT_LENGTH", std::to_string(input_size)));
		param_list.push_back(FastcgiParam("CONTENT_TYPE", "application/x-www-form-urlencoded; charset=UTF-8"));
	}

	size_t param_size = 0;
	for (FastcgiParam& item : param_list)
	{
		param_size += item.m_buffer.size();
	}
	FcgiHeader paramHeader(FcgiRequestType::Params, param_size);
	buffer.pushBack(paramHeader.begin(), paramHeader.end());

	for (FastcgiParam& item : param_list)
	{
		buffer.pushBack(item.m_buffer.begin(), item.m_buffer.end());
	}
	FcgiHeader zeroParamHeader(FcgiRequestType::Params, 0);
	buffer.pushBack(zeroParamHeader.begin(), zeroParamHeader.end());

	if (method == "POST" || method == "PUT" || method == "DELETE")
	{
		size_t offset = 0;
		while (offset < input_size)
		{
			size_t block_size = std::min((size_t)65535, input_size - offset);
			FcgiHeader hdr(FcgiRequestType::StdIn, block_size);
			buffer.pushBack(hdr.begin(), hdr.end());

			buffer.pushBack(&input[offset], &input[offset+block_size]);

			offset += 65535;
		}
	}

	FcgiHeader endHeader(FcgiRequestType::StdIn, 0);
	buffer.pushBack(endHeader.begin(), endHeader.end());
}
