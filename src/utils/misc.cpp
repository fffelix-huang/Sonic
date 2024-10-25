#include "misc.h"

namespace sonic {

#ifdef NO_PREFETCH

void prefetch(const void*) {}

#else

void prefetch(const void* addr) {
    #ifdef _MSC_VER
    _mm_prefetch((const char*) addr, _MM_HINT_T0);
    #else
    __builtin_prefetch(addr);
    #endif
}

#endif

} // namespace sonic