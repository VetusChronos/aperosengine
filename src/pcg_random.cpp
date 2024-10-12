// SPDX-License-Identifier: LGPL-2.1-or-later

#include "pcg_random.h"

PcgRandom::PcgRandom(uint64_t state, uint64_t seq) {
    seed(state, seq);
}

void PcgRandom::seed(uint64_t state, uint64_t seq) {
    m_state = 0U;
    m_inc = (seq << 1u) | 1u;
    next();
    m_state += state;
    next();
}

uint32_t PcgRandom::next() {
    uint64_t oldstate = m_state;
    m_state = oldstate * 6364136223846793005ULL + m_inc;

    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint32_t PcgRandom::range(uint32_t bound) {
    if (bound == 0)
        return next();

    uint32_t threshold = -bound % bound;
    uint32_t r;
    while ((r = next()) < threshold)
        ;

    return r % bound;
}

int32_t PcgRandom::range(int32_t min, int32_t max) {
    if (max < min)
        throw std::invalid_argument("Invalid range (max < min)");

    uint32_t bound = static_cast<int64_t>(max) - static_cast<int64_t>(min) + 1;
    return range(bound) + min;
}

void PcgRandom::bytes(void* out, size_t len) {
    uint8_t* outb = static_cast<uint8_t*>(out);
    int bytes_left = 0;
    uint32_t r;

    while (len--) {
        if (bytes_left == 0) {
            bytes_left = sizeof(uint32_t);
            r = next();
        }

        *outb = r & 0xFF;
        outb++;
        bytes_left--;
        r >>= CHAR_BIT;
    }
}

int32_t PcgRandom::randNormalDist(int32_t min, int32_t max, int num_trials) {
    int32_t accum = 0;
    for (int i = 0; i != num_trials; i++)
        accum += range(min, max);
    return static_cast<int32_t>(std::round(static_cast<float>(accum) / num_trials));
}

void PcgRandom::getState(uint64_t state[2]) const {
    state[0] = m_state;
    state[1] = m_inc;
}

void PcgRandom::setState(const uint64_t state[2]) {
    m_state = state[0];
    m_inc = state[1];
}
