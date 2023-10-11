#pragma once

namespace xxs::tools
{
    static inline void switch_on_bit_flag(unsigned int& flags, unsigned int bit) { flags |= bit; }
    static inline void switch_off_bit_flag(unsigned int& flags, unsigned int bit) { flags &= (~bit); }
    static inline bool check_bit_flag(unsigned int flags, unsigned int bit) { return (flags & bit) == bit; }
    static inline bool check_bit_flag_overlap(unsigned int flag0, unsigned int flag1) { return (flag0 & flag1) != 0; }
}

