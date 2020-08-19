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
    _output(capacity), _capacity(capacity), _partially(), _n_unassembled_bytes(0), _eof(false) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (index >= _capacity + _output.bytes_written()) {
        return;
    }

    // std::cout << data << std::endl;
    // [startWrittenIndex, endWrittenIndex] is the data range to be accpetd.
    size_t startWrittenIndex = std::max(_output.bytes_written(), index);
    size_t endWrittenIndex = std::min(_output.bytes_written() + _capacity, index + data.size());
    if (endWrittenIndex ==  index + data.size() && eof) {
        _eof = eof;
    }

    size_t nAccepted = 0;
    if (endWrittenIndex > startWrittenIndex) {
        nAccepted = endWrittenIndex - startWrittenIndex;
        _n_unassembled_bytes += nAccepted;
        // data 放入 _partially 中.
        auto iter = _partially.find(startWrittenIndex);
        bool isMerge = false;
        if (iter == _partially.end()) {
            auto ret = _partially.insert({startWrittenIndex, data.substr(startWrittenIndex - index, nAccepted)});
            iter = ret.first;
            isMerge = true;
        } else if (iter->second.size() < nAccepted) {
            _n_unassembled_bytes -= iter->second.size();
            iter->second = data.substr(startWrittenIndex - index, nAccepted);
            isMerge = true;
        }

        if (isMerge) {
            if (iter != _partially.begin()) {
                auto prev = iter;
                prev--;
                size_t prevEnd = prev->first + prev->second.size();
                size_t iterEnd = iter->first + iter->second.size();
                if (prevEnd >= iterEnd ) { // iter 的 data 整个被 prev 的 data 包含了
                    _n_unassembled_bytes -= iter->second.size();
                    _partially.erase(iter);
                    iter = _partially.end(); // iter 被整个删除，直接指定为 end, 无需再做任何处理
                } else if (prevEnd >= iter->first) { // iter 的 data 部分和 prev 的 data 重叠, 把 iter 与 prev 没有重叠的部分放入 prev 的data 中。
                    size_t i = prevEnd - iter->first;
                    _n_unassembled_bytes -= i;
                    prev->second.append(iter->second.substr(i));
                    _partially.erase(iter);
                    iter = prev;
                }
            }

            if (iter != _partially.end()) {
                size_t iterEnd = iter->first + iter->second.size();
                auto next = iter;
                next++;
                while (next != _partially.end()) {
                    if (iterEnd < next->first) { // 完全没有重叠。
                        break;
                    }
                    size_t nextEnd = next->first + next->second.size();
                    if (iterEnd >= nextEnd) { // next 被包含再 iter 中
                        _n_unassembled_bytes -= next->second.size();
                        next = _partially.erase(next);
                    } else { // next 部分与 iter 重叠
                        size_t i = iterEnd - next->first;
                        _n_unassembled_bytes -= i;
                        iter->second.append(next->second.substr(i));
                        _partially.erase(next);
                        break;
                    }
                }
            }
        }

        // 把连续的数据写入 StreamReassembler.
        iter = _partially.begin();
        size_t start = _output.bytes_written();
        if (iter != _partially.end() && start == iter->first) {
            _n_unassembled_bytes -= iter->second.size();
            _output.write(iter->second);
            iter = _partially.erase(iter);
        }
    }

    if (empty() && _eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    return _n_unassembled_bytes;
}

bool StreamReassembler::empty() const {
    return _partially.empty();
}
