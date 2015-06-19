
#pragma once

#include <iterator>

template <typename Buffer, typename T>
class BufferIterator {
public:
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef std::forward_iterator_tag iterator_category;

	reference operator*() const {
	}

	pointer operator->() const {
	}

	BufferIterator& operator++() {
	}

	BufferIterator operator++(int) {
		BufferIterator copy(*this);
		++*this;
		return copy;
	}

	BufferIterator& operator+=(difference_type distance) {
	}

	BufferIterator operator+(difference_type distance) const {
	}

	void operator=(const BufferIterator& other) {
	}

	bool operator!=(const BufferIterator& other) const {
	}

	bool operator==(const BufferIterator& other) const {
	}

	template <typename From>
	difference_type operator-(const From& from) const {
	}

public: //< Для конверсии Iterator -> ConstIterator
	Buffer* _buffer;
	pointer _it;
};


class Buffer {
public:
	typedef BufferIterator<Buffer, uint8_t> iterator;
	typedef BufferIterator<const Buffer, const uint8_t> const_iterator;
	friend iterator;
	friend const_iterator;
};


class BufferIteratorRange {
public:
	BufferIteratorRange();
	BufferIteratorRange(const Buffer::ConstIterator& begin, const Buffer::ConstIterator& end);

	Buffer::ConstIterator begin() const;
	Buffer::ConstIterator end() const;
	size_t length() const;

	operator std::vector<uint8_t>() const;

	bool operator==(const std::vector<uint8_t>& other) const;
	bool operator!=(const std::vector<uint8_t>& other) const;

private:
	Buffer::ConstIterator _begin, _end;
};


typedef std::unique_ptr<Buffer, std::function<void(Buffer*)>&> BufferUniquePtr;

BufferUniquePtr MallocBuffer();
BufferUniquePtr MallocBuffer(size_t size);
BufferUniquePtr MallocBuffer(const std::initializer_list<uint8_t>& data);
