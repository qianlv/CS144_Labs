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
    _output(capacity), _capacity(capacity), _partially(capacity, '0'), _filled(capacity, false), _eof(false) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t naccpted = std::min(data.size(), _capacity - index);
    std::copy(data.begin(), data.begin() + naccpted, _partially.begin() + index);
    std::fill_n(_filled.begin() + index, naccpted, true);
    size_t start = _output.bytes_written();
    size_t end = start;
    while (end < _capacity && _filled[end]) {
        end += 1;
    }

    _output.write(std::string(_partially.begin() + start, _partially.begin() + end));

    if (naccpted == data.size() && eof) {
        _eof = eof;
    }

    if (empty() && _eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t count = 0;
    size_t start = _output.bytes_written();
    for (size_t i = start; i < _capacity; ++i) {
        count += _filled[i];
    }
    return count;
}

bool StreamReassembler::empty() const {
    return unassembled_bytes() == 0;
}
