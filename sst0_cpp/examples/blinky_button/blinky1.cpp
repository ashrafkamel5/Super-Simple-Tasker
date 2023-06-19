//============================================================================
// Super-Simple Tasker (SST0/C++) Example
//
// Copyright (C) 2006-2023 Quantum Leaps, <state-machine.com>.
//
// SPDX-License-Identifier: MIT
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//============================================================================
#include "sst.hpp"           // SST framework
#include "bsp.hpp"           // Board Support Package interface
#include "blinky_button.hpp" // application shared interface

namespace {

DBC_MODULE_NAME("blinky1")   // for DBC assertions in this module */

} // unnamed namespace

namespace App {

//............................................................................
class Blinky1 : public SST::Task {
    SST::TimeEvt m_te;
    std::uint16_t m_toggles;

public:
    Blinky1(void);
    void init(SST::Evt const * const ie) override;
    void dispatch(SST::Evt const * const e) override;
    static Blinky1 inst;
};

//............................................................................
Blinky1 Blinky1::inst; // the Blinky1 instance
SST::Task * const AO_Blinky1 = &Blinky1::inst; // opaque AO pointer

//............................................................................
Blinky1::Blinky1(void)
  : m_te(TIMEOUT_SIG, this)
{}
//............................................................................
void Blinky1::init(SST::Evt const * const ie) {
    /* the initial event must be provided and must be WORKLOAD_SIG */
    DBC_REQUIRE(300,
        (ie != nullptr) && (ie->sig == BLINKY_WORK_SIG));

    m_te.arm(
        SST::evt_downcast<BlinkyWorkEvt>(ie)->ticks,
        SST::evt_downcast<BlinkyWorkEvt>(ie)->ticks);
    m_toggles = SST::evt_downcast<BlinkyWorkEvt>(ie)->toggles;
}
//............................................................................
void Blinky1::dispatch(SST::Evt const * const e) {
    switch (e->sig) {
        case TIMEOUT_SIG: {
            for (std::uint16_t i = m_toggles; i > 0U; --i) {
                // SST scheduler lock is not needed in non-preemptive SST0
                //SST::LockKey key = lock(3U);
                BSP::d5on();
                BSP::d5off();
                //unlock(key);
            }
            break;
        }
        case BLINKY_WORK_SIG: {
            BSP::d5on();
            m_te.arm(
                SST::evt_downcast<BlinkyWorkEvt>(e)->ticks,
                SST::evt_downcast<BlinkyWorkEvt>(e)->ticks);
            m_toggles = SST::evt_downcast<BlinkyWorkEvt>(e)->toggles;
            BSP::d5off();
            break;
        }
        default: {
            DBC_ERROR(500); // unexpected event
            break;
        }
    }
}

} // namespace App
