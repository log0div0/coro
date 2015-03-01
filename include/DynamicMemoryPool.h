
#pragma once

#include "Buffer.h"
#include <mutex>
#include <queue>

class DynamicMemoryPool {
public:
	template <size_t BufferSize>
	static DynamicMemoryPool& global() {
		static DynamicMemoryPool pool(BufferSize);
		return pool;
	}

	DynamicMemoryPool(size_t bufferSize);
	~DynamicMemoryPool();

	DynamicMemoryPool(const DynamicMemoryPool& other) = delete;
	DynamicMemoryPool& operator=(const DynamicMemoryPool& other) = delete;

	std::unique_ptr<Buffer, std::function<void(Buffer*)>&> makeUnique();
	std::shared_ptr<Buffer> makeShared();

	size_t size() const;

protected:
	Buffer* makeBuffer();

private:
	size_t _poolSize, _bufferSize;
	std::function<void(Buffer*)> _deleter;
	std::mutex _mutex;
	std::queue<Buffer*> _buffers;
};