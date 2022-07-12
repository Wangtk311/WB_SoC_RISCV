/*
 *  Copyright 2019 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __DEBUGGER_RIVERLIB_REGIBANK_H__
#define __DEBUGGER_RIVERLIB_REGIBANK_H__

#include <systemc.h>
#include "../river_cfg.h"

namespace debugger {

SC_MODULE(RegIntBank) {
    sc_in<bool> i_clk;                          // Clock
    sc_in<bool> i_nrst;                         // Reset. Active LOW
    sc_in<sc_uint<6>> i_radr1;                  // Port 1 read address
    sc_out<sc_uint<RISCV_ARCH>> o_rdata1;       // Port 1 read value
    sc_out<sc_uint<CFG_REG_TAG_WIDTH>> o_rtag1;  // Port 1 read tag value

    sc_in<sc_uint<6>> i_radr2;                  // Port 2 read address
    sc_out<sc_uint<RISCV_ARCH>> o_rdata2;       // Port 2 read value
    sc_out<sc_uint<CFG_REG_TAG_WIDTH>> o_rtag2;  // Port 2 read tag value

    sc_in<sc_uint<6>> i_waddr;                  // Writing value
    sc_in<bool> i_wena;                         // Writing is enabled
    sc_in<sc_uint<CFG_REG_TAG_WIDTH>> i_wtag;    // Writing register tag
    sc_in<sc_uint<RISCV_ARCH>> i_wdata;         // Writing value
    sc_in<bool> i_inorder;                      // Writing only if tag sequenced
    sc_out<bool> o_ignored;                     // Sequenced writing is ignored because it was overwritten by executor (need for tracer)

    sc_in<sc_uint<6>> i_dport_addr;             // Debug port address
    sc_in<bool> i_dport_ena;                    // Debug port is enabled
    sc_in<bool> i_dport_write;                  // Debug port write is enabled
    sc_in<sc_uint<RISCV_ARCH>> i_dport_wdata;   // Debug port write value
    sc_out<sc_uint<RISCV_ARCH>> o_dport_rdata;  // Debug port read value

    sc_out<sc_uint<RISCV_ARCH>> o_ra;       // Return address for branch predictor
    sc_out<sc_uint<RISCV_ARCH>> o_sp;       // Stack Pointer for border control

    void comb();
    void registers();

    SC_HAS_PROCESS(RegIntBank);

    RegIntBank(sc_module_name name_, bool async_reset);

    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);

private:
    friend struct Processor; // for debug purposes(remove it)s

    struct reg_score_type {
        sc_uint<RISCV_ARCH> val;
        sc_uint<CFG_REG_TAG_WIDTH> tag;
    };

    struct RegistersType {
        sc_signal<bool> update;             // To generate SystemC delta event only.
        reg_score_type reg[REGS_TOTAL]; // Multi-ports memory
    } v, r;

    bool async_reset_;
};

}  // namespace debugger

#endif  // __DEBUGGER_RIVERLIB_REGIBANK_H__
