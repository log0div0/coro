
#pragma once

#include "Buffer.h"
#include "CoroQueue.h"

class BlockingBufferPool {
public:
	BlockingBufferPool(size_t size);
	~BlockingBufferPool();

	BlockingBufferPool(const BlockingBufferPool& other) = delete;
	BlockingBufferPool& operator=(const BlockingBufferPool& other) = delete;

	BufferUniquePtr makeUnique();
	BufferSharedPtr makeShared();

	size_t size() const;

protected:
	Buffer* makeBuffer();

private:
	size_t _size;
	std::function<void(Buffer*)> _deleter;
	CoroQueue<Buffer*> _buffers;
};