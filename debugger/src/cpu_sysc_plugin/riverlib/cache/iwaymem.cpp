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

#include "iwaymem.h"

namespace debugger {

IWayMem::IWayMem(sc_module_name name_, int wayidx) : sc_module(name_) {
    wayidx_ = wayidx;

    tag0 = new DpRamTagi("itag0");

    tag0->i_clk(i_clk);
    tag0->i_radr(wb_radr);
    tag0->o_rdata(wb_tag_rdata);
    tag0->i_wadr(wb_wadr);
    tag0->i_wena(w_tag_wena);
    tag0->i_wdata(wb_tag_wdata);

    char tstr[32] = "data0";
    for (int i = 0; i < RAM64_BLOCK_TOTAL; i++) {
        tstr[5] = '0' + static_cast<char>(i);
        datan[i] = new DpRam64i(tstr);

        datan[i]->i_clk(i_clk);
        datan[i]->i_radr(wb_radr);
        datan[i]->o_rdata(wb_data_rdata[i]);
        datan[i]->i_wadr(wb_wadr);
        datan[i]->i_wena(w_data_wena[i]);
        datan[i]->i_wdata(i_wdata);
    }

    SC_METHOD(comb);
    sensitive << i_radr;
    sensitive << i_wadr;
    sensitive << i_wena;
    sensitive << i_wstrb;
    sensitive << i_wdata;
    sensitive << wb_tag_rdata;
    for (int i = 0; i < RAM64_BLOCK_TOTAL; i++) {
        sensitive << wb_data_rdata[i];
    }
};

IWayMem::~IWayMem() {
    delete tag0;
    for (int i = 0; i < RAM64_BLOCK_TOTAL; i++) {
        delete datan[i];
    }
}

void IWayMem::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    //if (o_vcd) {
    //    sc_trace(o_vcd, i_nrst, "/top/cache0/i0/i_nrst");
    //}
}

void IWayMem::comb() {
    sc_uint<32> vb_rdata;
    bool v_valid;

    wb_radr = i_radr.read()(IINDEX_END, IINDEX_START);
    wb_wadr = i_wadr.read()(IINDEX_END, IINDEX_START);

    w_tag_wena = i_wena.read();
    wb_tag_wdata = (i_wadr.read()(ITAG_END, ITAG_START), i_wvalid.read());

    for (int i = 0; i < RAM64_BLOCK_TOTAL; i++) {
        w_data_wena[i] = i_wena.read() && i_wstrb.read()[i];
    }

    v_valid = 0;
    switch (i_radr.read()(CFG_IOFFSET_WIDTH-1, 1)) {
    case 0x0:
        vb_rdata = wb_data_rdata[0].read()(31, 0);
        v_valid = wb_tag_rdata.read()[0];
        break;
    case 0x1:
        vb_rdata = wb_data_rdata[0].read()(47, 16);
        v_valid = wb_tag_rdata.read()[0];
        break;
    case 0x2:
        vb_rdata = wb_data_rdata[0].read()(63, 32);
        v_valid = wb_tag_rdata.read()[0];
        break;
    case 0x3:
        vb_rdata(15, 0) = wb_data_rdata[0].read()(63, 48);
        vb_rdata(31, 16) = wb_data_rdata[1].read()(15, 0);
        v_valid = wb_tag_rdata.read()[0] & wb_tag_rdata.read()[1];
        break;
    case 0x4:
        vb_rdata = wb_data_rdata[1].read()(31, 0);
        v_valid = wb_tag_rdata.read()[1];
        break;
    case 0x5:
        vb_rdata = wb_data_rdata[1].read()(47, 16);
        v_valid = wb_tag_rdata.read()[1];
        break;
    case 0x6:
        vb_rdata = wb_data_rdata[1].read()(63, 32);
        v_valid = wb_tag_rdata.read()[1];
        break;
    case 0x7:
        vb_rdata(15, 0) = wb_data_rdata[1].read()(63, 48);
        vb_rdata(31, 16) = wb_data_rdata[2].read()(15, 0);
        v_valid = wb_tag_rdata.read()[1] & wb_tag_rdata.read()[2];
        break;
    case 0x8:
        vb_rdata = wb_data_rdata[2].read()(31, 0);
        v_valid = wb_tag_rdata.read()[2];
        break;
    case 0x9:
        vb_rdata = wb_data_rdata[2].read()(47, 16);
        v_valid = wb_tag_rdata.read()[2];
        break;
    case 0xA:
        vb_rdata = wb_data_rdata[2].read()(63, 32);
        v_valid = wb_tag_rdata.read()[2];
        break;
    case 0xB:
        vb_rdata(15, 0) = wb_data_rdata[2].read()(63, 48);
        vb_rdata(31, 16) = wb_data_rdata[3].read()(15, 0);
        v_valid = wb_tag_rdata.read()[2] & wb_tag_rdata.read()[3];
        break;
    case 0xC:
        vb_rdata = wb_data_rdata[3].read()(31, 0);
        v_valid = wb_tag_rdata.read()[3];
        break;
    case 0xD:
        vb_rdata = wb_data_rdata[3].read()(47, 16);
        v_valid = wb_tag_rdata.read()[3];
        break;
    case 0xE:
        vb_rdata = wb_data_rdata[3].read()(63, 32);
        v_valid = wb_tag_rdata.read()[3];
        break;
    case 0xF:
        vb_rdata(15, 0) = wb_data_rdata[3].read()(63, 48);
        vb_rdata(31, 16) = wb_data_rdata[0].read()(15, 0);
        v_valid = wb_tag_rdata.read()[3] & wb_tag_rdata.read()[0];
        break;
    }

    o_rdata = vb_rdata;
    o_rtag = wb_tag_rdata.read()(CFG_ITAG_WIDTH_TOTAL-1, 4);
    o_valid = v_valid;
}

}  // namespace debugger

