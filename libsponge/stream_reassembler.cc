#include "stream_reassembler.hh"
#include <iostream>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) :
    _output(capacity), _capacity(capacity), _partially(capacity, '0'),
    _filled(capacity, false), _nunassembled_bytes(0), _eof(false) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (index >= _capacity) {
        return;
    }

    size_t nWrittenIndex = std::max(_output.bytes_written(), index);
    for (size_t i = nWrittenIndex; i < index + data.size() && i < _capacity; ++i) {
        if (!_filled[i]) {
            _filled[i] = true;
            _partially[i] = data[i-index];
            _nunassembled_bytes += 1;
        }
    }

    size_t start = _output.bytes_written();
    size_t end = start;
    while (end < _capacity && _filled[end]) {
        end += 1;
    }

    _nunassembled_bytes -= (end - start);
    _output.write(std::string(_partially.begin() + start, _partially.begin() + end));

    // std::cout << _nunassembled_bytes << std::endl;
    // std::cout << _capacity - index << "|" << data.size() << std::endl;

    if (_capacity - index >= data.size() && eof) {
        _eof = eof;
    }

    if (empty() && _eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    return _nunassembled_bytes;
}

bool StreamReassembler::empty() const {
    return unassembled_bytes() == 0;
}
