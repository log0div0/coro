
#pragma once

#include "Buffer.h"
#include <mutex>
#include <queue>


/// Do not use this class. Use MakeBufferUnique and MakeBufferShared instead.
class NonBlockingBufferPool {
public:
	NonBlockingBufferPool(size_t bufferSize);
	~NonBlockingBufferPool();

	NonBlockingBufferPool(const NonBlockingBufferPool& other) = delete;
	NonBlockingBufferPool& operator=(const NonBlockingBufferPool& other) = delete;

	BufferUniquePtr makeUnique();
	BufferSharedPtr makeShared();

	size_t size() const;

protected:
	Buffer* makeBuffer();

private:
	size_t _poolSize, _bufferSize;
	std::function<void(Buffer*)> _deleter;
	std::mutex _mutex;
	std::queue<Buffer*> _buffers;
};


template <size_t BufferSize>
NonBlockingBufferPool& GlobalBufferPool() {
	static NonBlockingBufferPool pool(BufferSize);
	return pool;
}

template <size_t BufferSize>
BufferUniquePtr MakeBufferUnique() {
	return GlobalBufferPool<BufferSize>().makeUnique();
}

template <size_t BufferSize>
BufferSharedPtr MakeBufferShared() {
	return GlobalBufferPool<BufferSize>().makeShared();
}