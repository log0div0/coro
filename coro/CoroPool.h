
#pragma once

#include "coro/Coro.h"
#include <set>
#include <initializer_list>

namespace coro {

/// Класс для иерархического управления корутинами в пределах текущего Strand
class CoroPool {
public:
	/// Блокирует поток выполнения до тех пор, пока не завершатся все дочерние корутины
	CoroPool() = default;
	~CoroPool();

	CoroPool(const CoroPool& other) = delete;
	CoroPool(CoroPool&& other);

	CoroPool& operator=(const CoroPool& other) = delete;
	CoroPool& operator=(CoroPool&& other);

	/// Запустить новую корутину в текущем Strand
	Coro* exec(std::function<void()> routine);
	/// Дождаться завершения всех дочерних корутин
	void waitAll(bool noThrow = false);
	/// Отменить все дочерние корутины
	void cancelAll();

	std::string token() const;
	size_t size() const;

private:
	void onCoroDone(Coro* coro);
	void assertCurrentIsParent();

	Coro* _parentCoro = Coro::current();
	std::set<Coro*> _childCoros;
};

void WaitOne(std::initializer_list<std::function<void()>> routines);

}