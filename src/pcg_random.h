// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef PCG_RANDOM_H
#define PCG_RANDOM_H

#include <cstdint>
#include <stdexcept>
#include <climits>
#include <cmath>
#include <array>

class PcgRandom {
public:
    static constexpr int32_t RANDOM_MIN = std::numeric_limits<int32_t>::min();
    static constexpr int32_t RANDOM_MAX = std::numeric_limits<int32_t>::max();
    static constexpr uint32_t RANDOM_RANGE = std::numeric_limits<uint32_t>::max();

    explicit PcgRandom(uint64_t state = 0x853c49e6748fea9bULL, uint64_t seq = 0xda3e39cb94b95bdbULL);
    void seed(uint64_t state, uint64_t seq = 0xda3e39cb94b95bdbULL);
    [[nodiscard]] uint32_t next();
    [[nodiscard]] uint32_t range(uint32_t bound);
    [[nodiscard]] int32_t range(int32_t min, int32_t max);
    void bytes(void* out, size_t len);
    [[nodiscard]] int32_t randNormalDist(int32_t min, int32_t max, int num_trials = 6);

    void getState(uint64_t state[2]) const;
    void setState(const uint64_t state[2]);

private:
    uint64_t m_state;
    uint64_t m_inc;
};

#endif // PCG_RANDOM_H
