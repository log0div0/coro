
#include "DynamicMemoryPool.h"

DynamicMemoryPool::DynamicMemoryPool(size_t bufferSize)
	: _poolSize(0), _bufferSize(bufferSize)
{
	_deleter = [&](Buffer* buffer) {
		buffer->clear();
		std::lock_guard<std::mutex> lock(_mutex);
		_buffers.push(buffer);
	};
}

DynamicMemoryPool::~DynamicMemoryPool() {
	assert(_poolSize == _buffers.size());
	for (size_t i = 0; i < _poolSize; ++i) {
		delete _buffers.front();
		_buffers.pop();
	}
}

std::unique_ptr<Buffer, std::function<void(Buffer*)>&> DynamicMemoryPool::makeUnique() {
	return { makeBuffer(), _deleter };
}

std::shared_ptr<Buffer> DynamicMemoryPool::makeShared() {
	return { makeBuffer(), _deleter };
}

size_t DynamicMemoryPool::size() const {
	return _poolSize;
}

Buffer* DynamicMemoryPool::makeBuffer() {
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
