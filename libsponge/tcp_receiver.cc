#include "tcp_receiver.hh"
#include "wrapping_integers.hh"
#include <algorithm>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // Need to track abs seqno for stream index (i.e. id in ByteStream)
    if (seg.header().syn) {
        _synReceived = true;
        _finReceived = false;
        _isn = seg.header().seqno; // set isn
    }
    if (!_synReceived) {
        return;
    }
    if (seg.header().fin) {
        // unsure if I should write with_eof to _reassembler
        _finReceived = true;
        _synReceived = false;
    }

    // Write payload to StreamReassembler
    // get stream_index (abs_seqno - 1)
    uint64_t abs_seqno = unwrap(seg.header().seqno, _isn, _reassembler.ack_index());
    size_t stream_index = static_cast<size_t>(abs_seqno) - 1;
    _reassembler.push_substring(seg.payload().copy(), stream_index, _finReceived);

    return;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_synReceived) {
        return nullopt;
    }

    return wrap(_reassembler.ack_index(), _isn);
}

size_t TCPReceiver::window_size() const {
    return _capacity - _reassembler.unassembled_bytes() - stream_out().buffer_size();
}
