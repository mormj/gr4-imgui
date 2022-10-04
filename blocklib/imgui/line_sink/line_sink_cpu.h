/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/imgui/line_sink.h>
#include <mutex>

namespace gr {
namespace imgui {

template <class T>
class line_sink_cpu : public line_sink<T>
{
public:
    line_sink_cpu(const typename line_sink<T>::block_args& args);

    work_return_t work(work_io& wio) override;

    // // DANGER!!!
    // virtual void lock() { _mutex.lock(); };
    // virtual void unlock() { _mutex.unlock(); };
    // virtual T* ptr() { return _last_buf_ptr; };
    std::vector<std::vector<T>> get_buf() override {
        std::scoped_lock lock(_mutex);
        return _buf;
    };

private:
    std::mutex _mutex;
    // std::vector<T*> _last_buf_ptrs;
    std::vector<std::vector<T>> _buf;
    size_t _buf_idx = 0;
};


} // namespace imgui
} // namespace gr
