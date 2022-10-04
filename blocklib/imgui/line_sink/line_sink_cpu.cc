/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "line_sink_cpu.h"
#include "line_sink_cpu_gen.h"

namespace gr {
namespace imgui {

template <class T>
line_sink_cpu<T>::line_sink_cpu(const typename line_sink<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T)
{
    _buf.resize(args.nplots);
    for (size_t i = 0; i < args.nplots; i++) {
        _buf[i].resize(args.npoints);
    }
}

template <class T>
work_return_t line_sink_cpu<T>::work(work_io& wio)
{
    std::scoped_lock lock(_mutex);
    auto nitems = wio.min_ninput_items();

    auto npoints = pmtf::get_as<size_t>(*this->param_npoints);

    size_t idx = 0;
    for (const auto& inp : wio.inputs()) {
        if (nitems >= npoints) {
            std::copy(inp.items<T>(), inp.items<T>() + npoints, _buf[idx].data());
        } else {
            std::copy(_buf[idx].data() + nitems,
                      _buf[idx].data() + 2 * nitems,
                      _buf[idx].data());
            std::copy(inp.items<T>(),
                      inp.items<T>() + nitems,
                      _buf[idx].data() + npoints - nitems);
        }
    }


    // Always keep the read pointer npoints back from the write pointer

    return work_return_t::OK;
}

} /* namespace imgui */
} /* namespace gr */
