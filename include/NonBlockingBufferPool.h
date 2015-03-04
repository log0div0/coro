
#pragma once

#include "Buffer.h"
#include <mutex>
#include <queue>


/// Do not use this class. Use MakeBufferUnique and MakeBufferShared instead.
class NonBlockingBufferPool {
public:
	NonBlockingBufferPool();
	~NonBlockingBufferPool();

	NonBlockingBufferPool(const NonBlockingBufferPool& other) = delete;
	NonBlockingBufferPool& operator=(const NonBlockingBufferPool& other) = delete;

	BufferUniquePtr makeUnique();
	BufferSharedPtr makeShared();

	size_t size() const;

protected:
	Buffer* makeBuffer();

private:
	size_t _size;
	std::function<void(Buffer*)> _deleter;
	std::mutex _mutex;
	std::queue<Buffer*> _buffers;
};


BufferUniquePtr MakeBufferUnique();
BufferSharedPtr MakeBufferShared();