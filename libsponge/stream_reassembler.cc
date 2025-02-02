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

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // Maintain buffer and check_data_valid_buffer
    if (eof && (index + data.size()) <= _capacity) {
        _eof = true;
    }

    // cerr << "here" << data << ":" << index << ":" << unassembled_base_id << ":" << (_output.bytes_written() - _output.bytes_read()) << ":" << _eof;
    /* if 
    
    */
    size_t num_bytes_in_output = _output.bytes_written() - _output.bytes_read();
    size_t curr_capacity = num_bytes_in_output + unassembled_size;
    
    if (index <= unassembled_base_id) {

        for (size_t start_id = unassembled_base_id - index; start_id < data.size(); start_id++) {
            _output.write(std::string(1, data[start_id]));
            if (check_data_valid_buffer.size()) {
                if (check_data_valid_buffer[0]) {
                    --unassembled_size;
                }
                buffer.pop_front();
                check_data_valid_buffer.pop_front();
            }
            // cerr << "*" << data[start_id] << _eof;
            ++unassembled_base_id;

            num_bytes_in_output = _output.bytes_written() - _output.bytes_read();
            curr_capacity = num_bytes_in_output + unassembled_size;
            if (curr_capacity >= _capacity) {
                break;
            }
        }
        


        // check if anything from buffer can be written
        size_t num_buffer_bytes_written = 0;
        for (size_t i=0; i < buffer.size(); i++) {
            if (i < check_data_valid_buffer.size() && check_data_valid_buffer[i]) {
                _output.write(std::string(1, buffer[i]));
                // cerr << "!" << buffer[i] << _eof;
                ++num_buffer_bytes_written;
                --unassembled_size;
                ++unassembled_base_id;

            } else {
                break;
            }

            
        }

        for (size_t i=0; i < num_buffer_bytes_written; i++) {
            buffer.pop_front();
            check_data_valid_buffer.pop_front();
        }

    } else {
        // write to buffer
        size_t max_buffer_size = _capacity - num_bytes_in_output;
        size_t buffer_offset = index - unassembled_base_id;
        // cerr << "%" << max_buffer_size << ":" << buffer_offset << "%";
        // capacity is implicitly handled size buffer is popped when bytes are written to bytestream (total bytes in bytestream and buffer are maintained constant)
        for (size_t i = buffer_offset; i < max_buffer_size && (i-buffer_offset) < data.size(); i++) {
            if (i >= check_data_valid_buffer.size()) {
                // buffers are too small for the data but system still has capacity
                check_data_valid_buffer.push_back(true);
                buffer.push_back(data[i - buffer_offset]);

            } else if (i < check_data_valid_buffer.size() && !check_data_valid_buffer[i]) {
                buffer[i] = data[i - buffer_offset];
                check_data_valid_buffer[i] = true;
                // cerr << "^" << data[i - buffer_offset] << i + buffer_offset << "^";
                ++unassembled_size;
            }

            num_bytes_in_output = _output.bytes_written() - _output.bytes_read();
            curr_capacity = num_bytes_in_output + unassembled_size;
            if (curr_capacity >= _capacity) {
                break;
            }
        }
    }

    // cerr << "end" << _eof << ":" << unassembled_size << ":" << count_valid_bytes();
    if (_eof && unassembled_size == 0) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled_size; }

bool StreamReassembler::empty() const { return unassembled_size == 0; }

size_t StreamReassembler::ack_index() const { return unassembled_base_id; }