#include "wrapping_integers.hh"
#include <iostream>

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    n += isn.raw_value();
    return WrappingInt32(n);
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t looplen = (1ul << 32);
    uint64_t seq = 0;
    if (n.raw_value() >= isn.raw_value()) {
        seq += n.raw_value() - isn.raw_value();
    } else {
        seq += looplen - isn.raw_value() + n.raw_value();
    }
    uint64_t loop = checkpoint / looplen;
    seq += loop * looplen;
    // seq 为 checkpoint 所在循环的 "absolute" sequence number
    // 当不一定是和 checkpoint 最接近的，可能是上一个或下一个循环
    if (seq < checkpoint) { // 比较当前和下一个循环与 checkpoint 的距离
        uint64_t dis1 = checkpoint - seq;
        uint64_t dis2 = looplen - dis1;
        if (dis2 < dis1) {
            seq += looplen;
        }
    } else if (loop > 0) { // 比较当前和上一个循环与 checkpoint 的距离
        uint64_t dis1 = seq - checkpoint;
        uint64_t dis2 = looplen - dis1;
        if (dis2 < dis1) {
            seq -= looplen;
        }
    }
    return seq;
}
