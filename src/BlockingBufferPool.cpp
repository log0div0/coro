
#include "BlockingBufferPool.h"

BlockingBufferPool::BlockingBufferPool(size_t size)
	: _size(size)
{
	_deleter = [&](Buffer* buffer) {
		buffer->clear();
		_buffers.push(buffer);
	};
	for (size_t i = 0; i < _size; ++i) {
		_buffers.push(new Buffer());
	}
}

BlockingBufferPool::~BlockingBufferPool() {
	for (size_t i = 0; i < _size; ++i) {
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
	return _size;
}

Buffer* BlockingBufferPool::makeBuffer() {
	return _buffers.pop();
}
