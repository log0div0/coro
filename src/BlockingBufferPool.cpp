
#include "BlockingBufferPool.h"

BlockingBufferPool::BlockingBufferPool(size_t poolSize, size_t bufferSize)
	: _poolSize(poolSize)
{
	_deleter = [&](Buffer* buffer) {
		buffer->clear();
		_buffers.push(buffer);
	};
	for (size_t i = 0; i < _poolSize; ++i) {
		_buffers.push(new Buffer(bufferSize));
	}
}

BlockingBufferPool::~BlockingBufferPool() {
	for (size_t i = 0; i < _poolSize; ++i) {
		delete _buffers.pop();
	}
}

BufferUniquePtr BlockingBufferPool::makeUnique() {
	return { makeBuffer(), _deleter };
}

BufferSharedPtr BlockingBufferPool::makeShared() {
	return { makeBuffer(), _deleter };
}

size_t BlockingBufferPool::size() const {
	return _poolSize;
}

Buffer* BlockingBufferPool::makeBuffer() {
	return _buffers.pop();
}
