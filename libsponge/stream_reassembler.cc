#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : unass_base(0)
    , unass_size(0)
    , _eof(0)
    , buffer(capacity, '\0')
    , bitmap(capacity, false)
    , _output(capacity)
    , _capacity(capacity) {}

//! \details This functions calls just after pushing a substring into the
//! _output stream. It aims to check if there exists any contiguous substrings
//! recorded earlier can be push into the stream.
void StreamReassembler::check_contiguous() {
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
}

size_t StreamReassembler::unassembled_bytes() const { return unass_size; }

bool StreamReassembler::empty() const { return unass_size == 0; }

size_t StreamReassembler::ack_index() const { return unass_base; }