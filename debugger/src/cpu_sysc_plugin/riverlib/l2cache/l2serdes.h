/*
 *  Copyright 2020 Sergey Khabarov, sergeykhbr@gmail.com
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

#ifndef __DEBUGGER_SRC_CPU_SYSC_PLUGIN_L2_L2SERDES_H__
#define __DEBUGGER_SRC_CPU_SYSC_PLUGIN_L2_L2SERDES_H__

#include "api_core.h"
#include "../types_river.h"
#include "../../ambalib/types_amba.h"
#include <systemc.h>

namespace debugger {

SC_MODULE(L2SerDes) {
    sc_in<bool> i_clk;                                  // CPU clock
    sc_in<bool> i_nrst;
    sc_out<axi4_l2_in_type> o_l2i;
    sc_in<axi4_l2_out_type> i_l2o;
    sc_in<axi4_master_in_type> i_msti;
    sc_out<axi4_master_out_type> o_msto;

    void comb();
    void registers();

    SC_HAS_PROCESS(L2SerDes);

    L2SerDes(sc_module_name name, bool async_reset);
    virtual ~L2SerDes();

    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);

 private:
    sc_uint<8> size2len(sc_uint<3> size);

    // TODO as generic parameters
    static const int linew = L2CACHE_LINE_BITS;
    static const int busw = CFG_SYSBUS_DATA_BITS;

    static const int lineb = linew / 8;
    static const int busb = busw / 8;
    static const int SERDES_BURST_LEN = lineb / busb;

    enum EState {
        State_Idle,
        State_Read,
        State_Write,
    };

    struct RegistersType {
        sc_signal<sc_uint<3>> state;
        sc_signal<sc_uint<8>> req_len;
        sc_signal<bool> b_wait;
        sc_signal<sc_biguint<linew>> line;
        sc_signal<sc_uint<lineb>> wstrb;
        sc_signal<sc_uint<SERDES_BURST_LEN>> rmux;
    } r, v;

    void R_RESET(RegistersType &iv) {
        iv.state = State_Idle;
        iv.req_len = 0;
        iv.b_wait = 0;
        iv.line = 0;
        iv.wstrb = 0;
        iv.rmux = 0;
    }

    bool async_reset_;
};

}  // namespace debugger

#endif  // __DEBUGGER_SRC_CPU_SYSC_PLUGIN_L2_L2SERDES_H__
