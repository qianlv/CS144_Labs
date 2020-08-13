#include "byte_stream.hh"

#include <algorithm>
#include <iterator>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity):
    _buffer(capacity, '\0'),
    _start_index(0),
    _end_index(0),
    _cap(capacity),
    _error(false),
    _nread_bytes(0),
    _nwritten_bytes(0),
    _end_input(false) {
}

size_t ByteStream::write(const string &data) {
    size_t nwritten = std::min(remaining_capacity(), data.size());
    if (nwritten > _cap - _end_index) {
        std::move(_buffer.begin() + _start_index, _buffer.begin() + _end_index, _buffer.begin());
        _end_index = buffer_size();
        _start_index = 0;
    }
    if (nwritten > 0) {
        std::copy_n(data.begin(), nwritten, _buffer.begin() + _end_index);
        _end_index += nwritten;
        _nwritten_bytes += nwritten;
    }
    return nwritten;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t n = std::min(len, buffer_size());
    return std::string{_buffer.begin() + _start_index, _buffer.begin() + _start_index + n};
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    size_t n = std::min(len, buffer_size());
    _start_index += n;
    _nread_bytes += n;
}

void ByteStream::end_input() {
    _end_input = true;
}

bool ByteStream::input_ended() const {
    return _end_input;
}

size_t ByteStream::buffer_size() const {
    return _end_index - _start_index;
}

bool ByteStream::buffer_empty() const {
    return buffer_size() == 0;
}

bool ByteStream::eof() const {
    return input_ended() && buffer_empty();
}

size_t ByteStream::bytes_written() const { return _nwritten_bytes; }

size_t ByteStream::bytes_read() const { return _nread_bytes; }

size_t ByteStream::remaining_capacity() const {
    return _start_index + _cap - _end_index;
}
