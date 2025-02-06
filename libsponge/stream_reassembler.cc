#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : unassembled_base_id(0)
    , unassembled_size(0)
    , _eof(0)
    , buffer(capacity, '\0')
    , check_data_valid_buffer(capacity, false)
    , _output(capacity)
    , _capacity(capacity) {}

//! \details This functions calls just after pushing a substring into the
//! _output stream. It aims to check if there exists any contiguous substrings
//! recorded earlier can be push into the stream.
// void StreamReassembler::check_contiguous() {
// }

size_t StreamReassembler::count_valid_bytes() {
    size_t num_valid = 0;
    for (size_t i = 0; i < check_data_valid_buffer.size(); i++) {
        if (check_data_valid_buffer[i]) {
            num_valid++;
        }
    }
    return num_valid;
}

void StreamReassembler::check_contiguous() {
    while (!_output.input_ended()) {
        if (buffer.empty() || !check_data_valid_buffer.front()) {
            break;
        }
        cerr << "write " << buffer.front() << endl;
        _output.write(std::string(1, buffer.front()));
        buffer.pop_front();
        check_data_valid_buffer.pop_front();
        
        buffer.push_back('\0');
        check_data_valid_buffer.push_back(false);
        --unassembled_size;
        ++unassembled_base_id;

        if (_eof && unassembled_size == 0) {
            cerr << "end_input1" << endl;
            _output.end_input();
            break;
        }
    }

}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (data.length() == 0 && eof && unassembled_size == 0) {
        _output.end_input();
        cerr << "end_input0" << endl;
        return;
    }
    // Maintain buffer and check_data_valid_buffer
    cerr << "here0 " << eof << " " << index << " " << data.size() << " " << _capacity << " " << _output.buffer_size() << endl;
    if (eof && (index + data.size()) <= _capacity) {
    // if (eof ) {
        cerr << "eof" << endl;
        _eof = true;
    }

    size_t segment_start = max(index, unassembled_base_id);
    size_t segment_end = min(index + data.size(), _capacity);
    cerr << "here1 " << index << " " << unassembled_base_id << " " << _capacity << " " << segment_start << " " << segment_end <<  endl;

    for (size_t i = segment_start; i < segment_end; i++) {
        size_t buffer_offset = i - unassembled_base_id;
        size_t data_offset = i - index;
        if (!check_data_valid_buffer[buffer_offset]) {
            check_data_valid_buffer[buffer_offset] = true;
            buffer[buffer_offset] = data[data_offset];

            unassembled_size++;
        }
    }
    check_contiguous();
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled_size; }

bool StreamReassembler::empty() const { return unassembled_size == 0; }

size_t StreamReassembler::ack_index() const { return unassembled_base_id; }