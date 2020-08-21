#include "tcp_receiver.hh"
#include <iostream>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader& header = seg.header();
    if (_has_syn == header.syn) {
        return false;
    }
    if (_has_fin && header.fin) {
        return false;
    }

    WrappingInt32 seqno = header.seqno;
    if (header.syn) {
        _has_syn = header.syn;
        _isn = header.seqno;
        seqno ++;
    }
    if (header.fin) {
        _has_fin = header.fin;
        _reassembler.stream_out().end_input();
    }
    const Buffer& payload = seg.payload();
    if (payload.size() > 0) {
        uint64_t abs_seqno = unwrap(seqno, _isn, _reassembler.stream_out().bytes_written());
        uint64_t index = abs_seqno-1;
        // std::cout << index + payload.size() << "|"
        //           << stream_out().bytes_written() << "|"
        //           << stream_out().bytes_read() + _capacity
        //           << std::endl;
        if (index + payload.size() <= stream_out().bytes_written() ||
            index >= stream_out().bytes_read() + _capacity) {
            // std::cout << "----" << std::endl;
            return false;
        }
        // std::cout << abs_seqno << "|" << index << std::endl;
        _reassembler.push_substring(payload.copy(), index, _has_fin);
    }
    return true;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_has_syn) {
        return std::nullopt;
    }

    return wrap(stream_out().bytes_written() + _has_syn + (_has_fin && _reassembler.empty()), _isn);
}

size_t TCPReceiver::window_size() const {
    return _capacity - stream_out().buffer_size();
}
