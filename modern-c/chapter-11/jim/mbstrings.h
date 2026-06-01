#include <stddef.h>

#if (__STDC_UTF_16__ != 1) || (__STDC_UTF_32__ != 1)
    # error"wide character strings should use UTF encodings"
#endif

enum mbcode {
    //! An invalid encoding was encountered.
    mbinvalid = (size_t)-1,
    //! The encoding was incomplete.
    mbincomplete = (size_t)-2,
    //! A previously detect output character was stored.
    mbstored = (size_t)-3,
};