// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
// 
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// 
#pragma once

#include <systemc.h>
#include "../../../../rtl/techmap/bufg/iobuf_tech.h"
#include "vip_sdcard_cmdio.h"
#include "vip_sdcard_ctrl.h"

namespace debugger {

SC_MODULE(vip_sdcard_top) {
 public:
    sc_in<bool> i_nrst;                                     // To avoid undefined states of registers (xxx)
    sc_in<bool> i_sclk;
    sc_inout<bool> io_cmd;
    sc_inout<bool> io_dat0;
    sc_inout<bool> io_dat1;
    sc_inout<bool> io_dat2;
    sc_inout<bool> io_cd_dat3;

    void comb();

    SC_HAS_PROCESS(vip_sdcard_top);

    vip_sdcard_top(sc_module_name name,
                   bool async_reset);
    virtual ~vip_sdcard_top();

    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);

 private:
    bool async_reset_;

    // Generic config parameters
    static const int CFG_SDCARD_POWERUP_DONE_DELAY = 700;
    static const uint8_t CFG_SDCARD_VHS = 0x1;
    static const bool CFG_SDCARD_PCIE_1_2V = 0;
    static const bool CFG_SDCARD_PCIE_AVAIL = 0;
    static const uint32_t CFG_SDCARD_VDD_VOLTAGE_WINDOW = 0xff8000;

    sc_signal<bool> w_clk;
    sc_signal<sc_uint<8>> wb_rdata;
    sc_signal<bool> w_cmd_in;
    sc_signal<bool> w_cmd_out;
    sc_signal<bool> w_cmd_dir;
    sc_signal<bool> w_cmd_req_valid;
    sc_signal<sc_uint<6>> wb_cmd_req_cmd;
    sc_signal<sc_uint<32>> wb_cmd_req_data;
    sc_signal<bool> w_cmd_req_ready;
    sc_signal<bool> w_cmd_resp_valid;
    sc_signal<sc_uint<32>> wb_cmd_resp_data32;
    sc_signal<bool> w_cmd_resp_ready;

    iobuf_tech *iobufcmd0;
    vip_sdcard_cmdio *cmdio0;
    vip_sdcard_ctrl *ctrl0;

};

}  // namespace debugger

