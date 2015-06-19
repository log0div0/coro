
#pragma once

template <typename Stream, typename Buffer, typename BufferIterator = typename Buffer::iterator>
class StreamIterator {
public:
	typedef typename BufferIterator::difference_type difference_type;
	typedef typename BufferIterator::value_type value_type;
	typedef typename BufferIterator::pointer pointer;
	typedef typename BufferIterator::reference reference;
	typedef std::forward_iterator_tag iterator_category;

	StreamIterator()
		: _stream(nullptr),
		  _buffer(nullptr)
	{

	}

	StreamIterator(Stream& handle, Buffer& buffer)
		: _stream(&handle),
		  _buffer(&buffer),
		  _it(_buffer->begin())
	{
	}

	operator BufferIterator() const {
		return _it;
	}

	reference operator*() const {
		return _it.operator*();
	}

	pointer operator->() const {
		return _it.operator->();
	}

	StreamIterator& operator++() {
		moveForward(1);
		return *this;
	}

	StreamIterator operator++(int) {
		StreamIterator copy(*this);
		++*this;
		return copy;
	}

	StreamIterator& operator+=(difference_type distance) {
		moveForward(distance);
		return *this;
	}

	StreamIterator operator+(difference_type distance) const {
		return StreamIterator(*this) += distance;
	}

	void operator=(const StreamIterator& other) {
		_stream = other._stream;
		_buffer = other._buffer;
		_it = other._it;
	}

	bool operator!=(const StreamIterator& other) const {
		return _it != other._it;
	}

	bool operator==(const StreamIterator& other) const {
		return _it == other._it;
	}

	difference_type operator-(const StreamIterator& other) const {
		return other._it - _it;
	}

private:
	void moveForward(size_t size) {
		while (static_cast<size_t>(_buffer->end() - _it) > size) {
			_stream->readSomeToBuffer(_buffer);
		}
		_it += size;
	}

	Stream* _stream;
	Buffer* _buffer;
	BufferIterator _it;
};

