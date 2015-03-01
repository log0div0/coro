
#include "FixedSizedMemoryPool.h"

FixedSizedMemoryPool::FixedSizedMemoryPool(size_t poolSize, size_t bufferSize)
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

FixedSizedMemoryPool::~FixedSizedMemoryPool() {
	for (size_t i = 0; i < _poolSize; ++i) {
		delete _buffers.pop();
	}
}

std::unique_ptr<Buffer, std::function<void(Buffer*)>&> FixedSizedMemoryPool::makeUnique() {
	return { makeBuffer(), _deleter };
}

std::shared_ptr<Buffer> FixedSizedMemoryPool::makeShared() {
	return { makeBuffer(), _deleter };
}

size_t FixedSizedMemoryPool::size() const {
	return _poolSize;
}

Buffer* FixedSizedMemoryPool::makeBuffer() {
	return _buffers.pop();
}
