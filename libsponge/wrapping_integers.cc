#include "wrapping_integers.hh"

#include <iostream>
#include <cmath>
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
    return WrappingInt32(isn.raw_value() + static_cast<uint32_t>(n));
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

    uint64_t num_round_trips = checkpoint >> 32;  // Efficiently divide by 2^32
    uint64_t offset;
    // Compute the 32-bit offset from ISN
    if (n - isn < 0) {
        offset = uint64_t(n - isn + (1L << 32));
    } else {
        offset = uint64_t(n - isn);
    }

    // Compute candidate absolute sequence numbers
    uint64_t current_round_candidate = (num_round_trips << 32) + offset;
    uint64_t next_round_candidate = ((num_round_trips + 1) << 32) + offset;
    uint64_t prev_round_candidate = ((num_round_trips - 1) << 32) + offset;

    uint64_t dist_cur = current_round_candidate > checkpoint ? current_round_candidate - checkpoint : checkpoint - current_round_candidate;
    uint64_t dist_prev = prev_round_candidate > checkpoint ? prev_round_candidate - checkpoint : checkpoint - prev_round_candidate;
    uint64_t dist_next = next_round_candidate > checkpoint ? next_round_candidate - checkpoint : checkpoint - next_round_candidate;

    // Return the absolute sequence number closest to checkpoint
    if (dist_cur <= dist_next && dist_cur <= dist_prev) {
        return current_round_candidate;
    } else if (dist_next <= dist_prev) {
        return next_round_candidate;
    } else {
        return prev_round_candidate;
    }
}
