
#include "NonBlockingBufferPool.h"

NonBlockingBufferPool::NonBlockingBufferPool(size_t bufferSize)
	: _poolSize(0), _bufferSize(bufferSize)
{
	_deleter = [&](Buffer* buffer) {
		buffer->clear();
		std::lock_guard<std::mutex> lock(_mutex);
		_buffers.push(buffer);
	};
}

NonBlockingBufferPool::~NonBlockingBufferPool() {
	assert(_poolSize == _buffers.size());
	for (size_t i = 0; i < _poolSize; ++i) {
		delete _buffers.front();
		_buffers.pop();
	}
}

BufferUniquePtr NonBlockingBufferPool::makeUnique() {
	return { makeBuffer(), _deleter };
}

BufferSharedPtr NonBlockingBufferPool::makeShared() {
	return { makeBuffer(), _deleter };
}

size_t NonBlockingBufferPool::size() const {
	return _poolSize;
}

Buffer* NonBlockingBufferPool::makeBuffer() {
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_buffers.size()) {
			Buffer* buffer = _buffers.front();
			_buffers.pop();
			return buffer;
		} else {
			++_poolSize;
		}
	}
	return new Buffer(_bufferSize);
}
