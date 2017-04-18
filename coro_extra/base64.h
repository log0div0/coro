
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace base64 {
	/// Закодировать данные в строку
	std::string encode(const void *data, size_t size);
	std::string encode(const std::vector<uint8_t> &data);

	/// Раскодировать данные из base64 (корректно обрабатывая выравнивание и переводы строк)
	/// Возвращает размер данных, не зависимо от того, что передали в качестве data и size
	/// Не заполняет data больше чем на size
	size_t decode(const std::string &str, void *data, size_t size);
	std::vector<uint8_t> decode(const std::string &str);

	// Делит base64 на строки по 64 символа, без вариантов
	std::string split(const std::string &str);

} // namespace base64

// Для совместимости эти прототипы оставлю, но они теперь всегда будут работать правильно
std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

std::string base64_encode_ex(const std::vector<uint8_t> &data);
std::vector<uint8_t> base64_decode_ex(const std::string &base64_data);

