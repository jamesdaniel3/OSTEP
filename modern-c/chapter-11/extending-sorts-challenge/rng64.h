// Source - https://stackoverflow.com/a/53900554
// Posted by Nominal Animal, modified by community. See post 'Timeline' for change history
// Retrieved 2026-04-13, License - CC BY-SA 4.0

#ifndef   RNG64_H
#define   RNG64_H
#include <inttypes.h>
#include <time.h>

typedef struct {
    uint64_t  limit;
    int64_t   base;
    int       shift;
} rng64_intrange_spec;

static uint64_t  rng64_state = 1;

static inline uint64_t  rng64(void)
{
    uint64_t  x = rng64_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    rng64_state = x;
    return x * UINT64_C(2685821657736338717);
}

static inline uint64_t  rng64_randomize(void)
{
    uint64_t  x;
    int       n = 1000;

    x = ((uint64_t)time(NULL) * UINT64_C(19076794157513))
      ^ ((uint64_t)clock() * UINT64_C(809712647));
    if (!x)
        x = 1;
    while (n-->0) {
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
    }

    rng64_state = x;
    return x;
}

static inline double rng64_one(void)
{
    return (double)rng64() / 18446744073709551616.0;
}

static inline int64_t rng64_intrange(rng64_intrange_spec *spec)
{
    const uint64_t  limit = spec->limit;
    const int       shift = spec->shift;
    uint64_t        value;

    do {
        value = rng64() >> shift;
    } while (value > limit);

    return spec->base + value;
}

static inline void rng64_set_intrange(rng64_intrange_spec *spec,
                                  int64_t minimum,
                                  int64_t maximum)
{
    int64_t   base;
    uint64_t  limit;
    int       bits = 0;

    if (minimum <= maximum) {
        base  = minimum;
        limit = maximum - minimum;
    } else {
        base  = maximum;
        limit = minimum - maximum;
    }

    spec->base  = base;
    spec->limit = limit;

    while (limit >= 32768) {
        limit >>= 16;
        bits   += 16;
    }
    while (limit >= 8) {
        limit >>= 4;
        bits   += 4;
    }
    while (limit > 0) {
        limit >>= 1;
        bits   += 1;
    }

    spec->shift = 64 - bits;
}

#endif /* RNG64_H */
