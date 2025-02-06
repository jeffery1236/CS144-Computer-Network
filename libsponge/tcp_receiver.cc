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
        // if (seg.header().fin) {
        //     _finReceived = true;
        //     _reassembler.push_substring(seg.payload().copy(), 0, _finReceived);
        //     return;
        // }
    }
    if (!_synReceived) {
        return;
    }
    if (seg.header().fin) {
        _finReceived = true;
    }

    // Write payload to StreamReassembler
    // get stream_index (abs_seqno - 1)
    uint64_t abs_seqno = unwrap(seg.header().seqno, _isn, _reassembler.ack_index());
    // cerr << "here123 " << abs_seqno << endl;
    size_t stream_index = static_cast<size_t>(abs_seqno) - 1;
    _reassembler.push_substring(seg.payload().copy(), stream_index, _finReceived);
    
    cerr << "here1 " << stream_index << endl;
    WrappingInt32 stream_index2 = wrap(_reassembler.ack_index(), _isn);
    cerr << "here2 " << stream_index2 << " " << _isn << endl;
    return;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_synReceived && !_finReceived) {
        return nullopt;
    }

    WrappingInt32 stream_index = wrap(_reassembler.ack_index(), _isn);
    cerr << "here123 " << stream_index << " " << _isn << endl;
    return stream_index + _synReceived + _finReceived;
}

size_t TCPReceiver::window_size() const {
    // return _capacity - _reassembler.unassembled_bytes() - stream_out().buffer_size();
    return _capacity - stream_out().buffer_size();
}
