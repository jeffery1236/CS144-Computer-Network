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
    // uint64_t num_round_trips = checkpoint >> 32; // (can do some bit operations to make this efficient)

    // if (isn.raw_value() >=n.raw_value()) {
    //     uint64_t remainder = uint64_t(isn - n + (1L << 32));
    //     return  ((num_round_trips - 1) << 32) + remainder;
    // } else {
    //     uint64_t remainder = uint64_t(n - isn);
    //     return (num_round_trips << 32) + remainder;
    // }

    uint64_t num_round_trips = checkpoint >> 32;  // Efficiently divide by 2^32
    uint64_t remainder;
    // Compute the remainder as the 32-bit offset from ISN
    if (n - isn < 0) {
        remainder = uint64_t(n - isn + (1L << 32));
    } else {
        remainder = uint64_t(n - isn);
    }

    // Compute candidate absolute sequence numbers
    uint64_t current_round_trip_abs_seq_no = (num_round_trips << 32) + remainder;
    uint64_t next_round_trip_abs_seq_no = ((num_round_trips + 1) << 32) + remainder;
    uint64_t prev_round_trip_abs_seq_no = ((num_round_trips - 1) << 32) + remainder;

    // Compute absolute differences
    int64_t diff_current = static_cast<int64_t>(current_round_trip_abs_seq_no) - static_cast<int64_t>(checkpoint);
    int64_t diff_next = static_cast<int64_t>(next_round_trip_abs_seq_no) - static_cast<int64_t>(checkpoint);
    int64_t diff_prev = static_cast<int64_t>(prev_round_trip_abs_seq_no) - static_cast<int64_t>(checkpoint);

    // Return the absolute sequence number closest to checkpoint
    if (std::llabs(diff_current) <= std::llabs(diff_next) && std::llabs(diff_current) <= std::llabs(diff_prev)) {
        return current_round_trip_abs_seq_no;
    } else if (std::llabs(diff_next) <= std::llabs(diff_prev)) {
        return next_round_trip_abs_seq_no;
    } else {
        return prev_round_trip_abs_seq_no;
    }
}
