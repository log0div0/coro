
#pragma once

#include "Buffer.h"
#include "CoroQueue.h"

class FixedSizedMemoryPool {
public:
	FixedSizedMemoryPool(size_t poolSize, size_t bufferSize);
	~FixedSizedMemoryPool();

	FixedSizedMemoryPool(const FixedSizedMemoryPool& other) = delete;
	FixedSizedMemoryPool& operator=(const FixedSizedMemoryPool& other) = delete;

	std::unique_ptr<Buffer, std::function<void(Buffer*)>&> makeUnique();
	std::shared_ptr<Buffer> makeShared();

	size_t size() const;

protected:
	Buffer* makeBuffer();

private:
	size_t _poolSize;
	std::function<void(Buffer*)> _deleter;
	CoroQueue<Buffer*> _buffers;
};