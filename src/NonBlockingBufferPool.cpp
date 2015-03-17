
#include "NonBlockingBufferPool.h"

NonBlockingBufferPool::NonBlockingBufferPool()
	: _size(0)
{
	_deleter = [&](Buffer* buffer) {
		buffer->clear();
		std::lock_guard<std::mutex> lock(_mutex);
		_buffers.push(buffer);
	};
}

NonBlockingBufferPool::~NonBlockingBufferPool() {
	assert(_size == _buffers.size());
	for (size_t i = 0; i < _size; ++i) {
		delete _buffers.front();
		_buffers.pop();
	}
}

BufferUniquePtr NonBlockingBufferPool::mallocUnique() {
	return { malloc(), _deleter };
}

BufferSharedPtr NonBlockingBufferPool::mallocShared() {
	return { malloc(), _deleter };
}

size_t NonBlockingBufferPool::size() const {
	return _size;
}

Buffer* NonBlockingBufferPool::malloc() {
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_buffers.size()) {
			Buffer* buffer = _buffers.front();
			_buffers.pop();
			return buffer;
		} else {
			++_size;
		}
	}
	return new Buffer();
}


static NonBlockingBufferPool pool;


BufferUniquePtr MallocUnique() {
	return pool.mallocUnique();
}


BufferSharedPtr MallocShared() {
	return pool.mallocShared();
}