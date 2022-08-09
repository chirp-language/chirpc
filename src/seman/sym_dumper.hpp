/// \file Symbol table dumper for debug output

#pragma once

#include "tracker.hpp"
#include "../shared/text_dumper_base.hpp"

enum class color;

class text_symbol_dumper : public text_dumper_base {
    bool dump_syms_extra;
    int depth = 0;
    tracker& m_tracker;

    public:
    text_symbol_dumper(bool enable_colors, bool dump_syms_extra, ::tracker& tracker)
        : text_dumper_base(enable_colors), dump_syms_extra(dump_syms_extra), m_tracker(tracker)
    {}

    void dump_symbols();

    private:
    void dump(tracker::symbol& sym);
};
