#include "coro_extra/FcgiClient.h"

using namespace coro;
using namespace Fcgi;

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

Header::Header(RequestType t, size_t size)
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

static int get_http_status(const std::string &headers)
{
	// Status: 200 OK

	const std::string status_tag = "Status:";
	size_t pos = headers.find(status_tag);
	if (pos == std::string::npos) {
		return -1;
	}
	pos += status_tag.length();
	while (!isdigit(headers[pos])) {
		++pos;
	}
	if (pos >= headers.length()) {
		return -1;
	}
	std::string code;
	while (isdigit(headers[pos])) {
		code += headers[pos];
	++pos;
	}
	if (code.empty()) {
		return -1;
	}

	return std::stoi(code);
}

Response
Client::make_request(const std::string& method, const std::string &URI,
	const std::string &body, TcpSocket& socket, const std::string server, uint16_t port)
{
	Header beginRequestHeader(RequestType::BeginRequest, sizeof(BeginRequestBody));
	socket.write(beginRequestHeader.as_buf());

	BeginRequestBody beginBody;
	memset(&beginBody, 0, sizeof(beginBody));
	beginBody.roleB0 = 1;
	socket.write(beginBody.as_buf());

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
	Header paramHeader(RequestType::Params, param_size);
	socket.write(paramHeader.as_buf());

	for (FastcgiParam& item : param_list)
	{
		socket.write(asio::buffer(item.m_buffer));
	}
	Header zeroParamHeader(RequestType::Params, 0);
	socket.write(zeroParamHeader.as_buf());

	if (method == "POST" || method == "PUT" || method == "DELETE")
	{
		size_t offset = 0;
		while (offset < input_size)
		{
			size_t block_size = std::min((size_t)65535, input_size - offset);
			Header hdr(RequestType::StdIn, block_size);
			socket.write(hdr.as_buf());

			socket.write(asio::const_buffer(&input[offset], block_size));

			offset += 65535;
		}
	}

	Header endHeader(RequestType::StdIn, 0);
	socket.write(asio::const_buffer(&endHeader, sizeof(Header)));

	std::string resp("");
	while (1)
	{
		Header responseHeader(RequestType::UnknownType, 0);
		socket.read(responseHeader.as_mbuf());
		if (responseHeader.type == (uint8_t)RequestType::EndRequest)
		{
			break;
		}

		size_t len = responseHeader.contentLengthB1;
		len = len << 8;
		len += responseHeader.contentLengthB0;
		if (len > 0)
		{
			std::string text_response;
			text_response.resize(len);
			socket.read(asio::mutable_buffer((void*)text_response.data(), len));
			resp += text_response;
		}

		if (responseHeader.paddingLength > 0)
		{
			std::vector<uint8_t> padding;
			padding.resize(responseHeader.paddingLength);
			socket.read(padding.data(), responseHeader.paddingLength);
		}
	}

	const std::string delimiter = "\r\n\r\n";
	ssize_t body_pos = resp.find(delimiter);
	const std::string headers = resp.substr(0, body_pos);

	Response result;
	result.status = get_http_status(headers);
	result.text = resp.substr(body_pos + delimiter.length());
	result.headers = headers;

	return result;
}
