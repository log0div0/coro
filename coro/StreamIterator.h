
#pragma once

#include <algorithm>
#include <boost/iterator/iterator_facade.hpp>

template <typename Stream, typename Buffer>
class StreamIterator
	: public boost::iterator_facade<
		StreamIterator<Stream, Buffer>,
		typename Buffer::value_type,
		boost::random_access_traversal_tag,
		typename Buffer::reference,
		size_t
	  >
{
public:
	enum { MinimumChunkSize = 1024 };

	StreamIterator()
		: _stream(nullptr),
		  _buffer(nullptr),
		  _offset(-1)
	{

	}

	StreamIterator(Stream& handle, Buffer& buffer, size_t offset = 0)
		: _stream(&handle),
		  _buffer(&buffer),
		  _offset(offset)
	{
	}

	operator typename Buffer::iterator () const {
		return _buffer->begin() + _offset;
	}

	operator typename Buffer::const_iterator () const {
		return _buffer->cbegin() + _offset;
	}

private:
	friend class boost::iterator_core_access;

	void increment() {
		advance(1);
	}

	void advance(size_t distance) {
		_offset += distance;
	}

	bool equal(const StreamIterator& other) const {
		return _offset == other._offset;
	}

	typename Buffer::reference dereference() const {
		while (_offset >= _buffer->size()) {
			size_t chunkSize = std::max(static_cast<size_t>(MinimumChunkSize), _offset - _buffer->size());
			Buffer chunk(chunkSize, typename Buffer::value_type());
			chunk.resize(_stream->readSome(boost::asio::buffer(&chunk[0], chunk.size())));
			_buffer->insert(_buffer->end(), chunk.begin(), chunk.end());
		}
		return (*_buffer)[_offset];
	}

	Stream* _stream;
	Buffer* _buffer;
	size_t _offset;
};

