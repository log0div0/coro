
#pragma once

#include "Buffer.h"
#include <mutex>
#include <queue>


/// Do not use this class. Use MallocUnique and MallocShared instead.
class NonBlockingBufferPool {
public:
	NonBlockingBufferPool();
	~NonBlockingBufferPool();

	NonBlockingBufferPool(const NonBlockingBufferPool& other) = delete;
	NonBlockingBufferPool& operator=(const NonBlockingBufferPool& other) = delete;

	BufferUniquePtr mallocUnique();
	BufferSharedPtr mallocShared();

	size_t size() const;

protected:
	Buffer* malloc();

private:
	size_t _size;
	std::function<void(Buffer*)> _deleter;
	std::mutex _mutex;
	std::queue<Buffer*> _buffers;
};


BufferUniquePtr MallocUnique();
BufferSharedPtr MallocShared();