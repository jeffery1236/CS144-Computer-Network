#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>
// #include <iostream>
#include <algorithm>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _rto{retx_timeout} {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() {
    /*
    In this function, we check our window and read from input ByteStream. Send as many bytes as possible (TCPSegments, as long as there are new bytes in the the ByteStream and space available in the window.
    
    Make each TCPSegment as large as possible, up to TCPConfig::MAX PAYLOAD SIZE

    Use TCPSegment::length_in_sequence_space() to count total number of seqno occupied by a segment
    - Syn and Fin occupy 1 seqno each

    If window_size of receiver == 0, treat it like window_size is 1 -> send a single byte
    */
    if (!_syn_sent) {
        // Send syn segment
        TCPSegment segment;
        segment.header().syn = true;
        _send_segment(segment);
    }

    // size_t window_left_edge = _next_seqno;
    // size_t window_right_edge = 
    uint64_t window_size = _receiver_window_size ? _receiver_window_size : 1;

    while (!_stream.buffer_empty() && _bytes_in_flight < window_size) {

    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    /*
    Determine new left (ackno) and right (ackno + window_size) bounds of Sender's window

    Sender can drop any outstanding segments before new left bound (they have already been fully acknowledged.)

    Reset _rto and _consecutive_retransmissions
    if there are any outstanding data, restart the retransmission timer to expire after _rto milliseconds
    else stop the retransmission timer
    
    Update _receiver_window_size, _bytes_in_flight, _next_seqno
    */
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if (!_timer_running) {
        return;
    }
    _time_elapsed += ms_since_last_tick;

    if (_time_elapsed >= _rto) {
        // Retransmit the oldest outstanding segment
        _segments_out.push(_segments_outstanding.front());

        if (_receiver_window_size || _segments_outstanding.front().header().syn) {
            // exponential backoff
            _consecutive_retransmissions++;
            _rto *= 2;
        }

        _time_elapsed = 0;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retransmissions; }

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    seg.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(seg);
}

// See test code send_window.cc line 113 why the commented code is wrong.
bool TCPSender::_ack_valid(uint64_t abs_ackno) {
}

void TCPSender::_send_segment(TCPSegment &seg) {
    seg.header().seqno = wrap(_next_seqno, _isn);

    if (seg.header().syn) {
        _syn_sent = true;
    }
    if (seg.header().fin) {
        _fin_sent = true;
    }

    _segments_outstanding.push(seg);
    segments_out.push(seg);

    _bytes_in_flight += seg.length_in_sequence_space();

    if (!_timer_running) {
        _timer_running = true;
    }
    return;
}