
#pragma once

#include <functional>
#include <exception>

// 1) Приостанавливает текущую корутину
// 2) Запускает routines
// 3) Ждет завершения routines
// 4) Возобновляет корутину из п.1
// 5) Вызывает errorHandler, передавая в неё все непойманные корутинами исключения
void Parallel(const std::initializer_list<std::function<void()>>& routines,
	const std::function<void(const std::exception&)>& errorHandler
		= std::function<void(const std::exception&)>());
