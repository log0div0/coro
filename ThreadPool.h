
#pragma once

#include <boost/asio.hpp>

class ThreadPool {
public:
	// Запуск главной корутины
	void main(std::function<void()> program);

	// 1) Приостанавливает текущую корутину
	// 2) Запускает programs в отдельных корутинах
	// 3) Ждет заверщения всех запущеных корутин
	// 4) Возобновляет корутину из п.1
	// 5) Вызывает errorHandler, передавая в неё все брошенные исключения
	void parallel(std::initializer_list<std::function<void()>> programs,
		std::function<void(const std::exception&)> errorHandler = std::function<void(const std::exception&)>());

	boost::asio::io_service& ioService();

private:
	boost::asio::io_service _io_service;
};