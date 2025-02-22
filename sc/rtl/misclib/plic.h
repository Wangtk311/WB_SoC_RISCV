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
#include "../ambalib/types_amba.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/axi_slv.h"
#include "api_core.h"

namespace debugger {

template<int ctxmax = 8,
         int irqmax = 128>
SC_MODULE(plic) {
 public:
    sc_in<bool> i_clk;                                      // CPU clock
    sc_in<bool> i_nrst;                                     // Reset: active LOW
    sc_in<mapinfo_type> i_mapinfo;                          // interconnect slot information
    sc_out<dev_config_type> o_cfg;                          // Device descriptor
    sc_in<axi4_slave_in_type> i_xslvi;                      // AXI Slave to Bridge interface
    sc_out<axi4_slave_out_type> o_xslvo;                    // AXI Bridge to Slave interface
    sc_in<sc_biguint<irqmax>> i_irq_request;                // [0] must be tight to GND
    sc_out<sc_uint<ctxmax>> o_ip;

    void comb();
    void registers();

    SC_HAS_PROCESS(plic);

    plic(sc_module_name name,
         bool async_reset);
    virtual ~plic();

    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);

 private:
    bool async_reset_;

    struct plic_context_type {
        sc_uint<4> priority_th;
        sc_bv<1024> ie;                                     // interrupt enable per context
        sc_bv<(4 * 1024)> ip_prio;                          // interrupt pending priority per context
        sc_uint<16> prio_mask;                              // pending interrupts priorites
        sc_uint<4> sel_prio;                                // the most available priority
        sc_uint<10> irq_idx;                                // currently selected most prio irq
        sc_uint<10> irq_prio;                               // currently selected prio level
    };


    struct plic_registers {
        sc_signal<sc_bv<(4 * 1024)>> src_priority;
        sc_signal<sc_bv<1024>> pending;
        sc_signal<sc_uint<ctxmax>> ip;
        plic_context_type ctx[ctxmax];
        sc_signal<sc_uint<64>> rdata;
    } v, r;

    sc_signal<bool> w_req_valid;
    sc_signal<sc_uint<CFG_SYSBUS_ADDR_BITS>> wb_req_addr;
    sc_signal<sc_uint<8>> wb_req_size;
    sc_signal<bool> w_req_write;
    sc_signal<sc_uint<CFG_SYSBUS_DATA_BITS>> wb_req_wdata;
    sc_signal<sc_uint<CFG_SYSBUS_DATA_BYTES>> wb_req_wstrb;
    sc_signal<bool> w_req_last;
    sc_signal<bool> w_req_ready;
    sc_signal<bool> w_resp_valid;
    sc_signal<sc_uint<CFG_SYSBUS_DATA_BITS>> wb_resp_rdata;
    sc_signal<bool> wb_resp_err;

    axi_slv *xslv0;

};

template<int ctxmax, int irqmax>
plic<ctxmax, irqmax>::plic(sc_module_name name,
                           bool async_reset)
    : sc_module(name),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_mapinfo("i_mapinfo"),
    o_cfg("o_cfg"),
    i_xslvi("i_xslvi"),
    o_xslvo("o_xslvo"),
    i_irq_request("i_irq_request"),
    o_ip("o_ip") {

    async_reset_ = async_reset;
    xslv0 = 0;

    xslv0 = new axi_slv("xslv0", async_reset,
                         VENDOR_OPTIMITECH,
                         OPTIMITECH_PLIC);
    xslv0->i_clk(i_clk);
    xslv0->i_nrst(i_nrst);
    xslv0->i_mapinfo(i_mapinfo);
    xslv0->o_cfg(o_cfg);
    xslv0->i_xslvi(i_xslvi);
    xslv0->o_xslvo(o_xslvo);
    xslv0->o_req_valid(w_req_valid);
    xslv0->o_req_addr(wb_req_addr);
    xslv0->o_req_size(wb_req_size);
    xslv0->o_req_write(w_req_write);
    xslv0->o_req_wdata(wb_req_wdata);
    xslv0->o_req_wstrb(wb_req_wstrb);
    xslv0->o_req_last(w_req_last);
    xslv0->i_req_ready(w_req_ready);
    xslv0->i_resp_valid(w_resp_valid);
    xslv0->i_resp_rdata(wb_resp_rdata);
    xslv0->i_resp_err(wb_resp_err);

    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_mapinfo;
    sensitive << i_xslvi;
    sensitive << i_irq_request;
    sensitive << w_req_valid;
    sensitive << wb_req_addr;
    sensitive << wb_req_size;
    sensitive << w_req_write;
    sensitive << wb_req_wdata;
    sensitive << wb_req_wstrb;
    sensitive << w_req_last;
    sensitive << w_req_ready;
    sensitive << w_resp_valid;
    sensitive << wb_resp_rdata;
    sensitive << wb_resp_err;
    sensitive << r.src_priority;
    sensitive << r.pending;
    sensitive << r.ip;
    sensitive << r.rdata;

    SC_METHOD(registers);
    sensitive << i_nrst;
    sensitive << i_clk.pos();
}

template<int ctxmax, int irqmax>
plic<ctxmax, irqmax>::~plic() {
    if (xslv0) {
        delete xslv0;
    }
}

template<int ctxmax, int irqmax>
void plic<ctxmax, irqmax>::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    std::string pn(name());
    if (o_vcd) {
        sc_trace(o_vcd, i_xslvi, i_xslvi.name());
        sc_trace(o_vcd, o_xslvo, o_xslvo.name());
        sc_trace(o_vcd, i_irq_request, i_irq_request.name());
        sc_trace(o_vcd, o_ip, o_ip.name());
        sc_trace(o_vcd, r.src_priority, pn + ".r_src_priority");
        sc_trace(o_vcd, r.pending, pn + ".r_pending");
        sc_trace(o_vcd, r.ip, pn + ".r_ip");
        for (int i = 0; i < ctxmax; i++) {
            char tstr[1024];
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_ctx%d_priority_th", pn.c_str(), i);
            sc_trace(o_vcd, r.ctx[i].priority_th, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_ctx%d_ie", pn.c_str(), i);
            sc_trace(o_vcd, r.ctx[i].ie, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_ctx%d_ip_prio", pn.c_str(), i);
            sc_trace(o_vcd, r.ctx[i].ip_prio, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_ctx%d_prio_mask", pn.c_str(), i);
            sc_trace(o_vcd, r.ctx[i].prio_mask, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_ctx%d_sel_prio", pn.c_str(), i);
            sc_trace(o_vcd, r.ctx[i].sel_prio, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_ctx%d_irq_idx", pn.c_str(), i);
            sc_trace(o_vcd, r.ctx[i].irq_idx, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_ctx%d_irq_prio", pn.c_str(), i);
            sc_trace(o_vcd, r.ctx[i].irq_prio, tstr);
        }
        sc_trace(o_vcd, r.rdata, pn + ".r_rdata");
    }

    if (xslv0) {
        xslv0->generateVCD(i_vcd, o_vcd);
    }
}

template<int ctxmax, int irqmax>
void plic<ctxmax, irqmax>::comb() {
    sc_uint<CFG_SYSBUS_DATA_BITS> vrdata;
    sc_uint<10> vb_irq_idx[ctxmax];                         // Currently selected most prio irq
    sc_uint<10> vb_irq_prio[ctxmax];                        // Currently selected prio level
    plic_context_type vb_ctx[ctxmax];
    sc_bv<(4 * 1024)> vb_src_priority;
    sc_bv<1024> vb_pending;
    sc_uint<ctxmax> vb_ip;
    int rctx_idx;

    vrdata = 0;
    for (int i = 0; i < ctxmax; i++) {
        vb_irq_idx[i] = 0;
    }
    for (int i = 0; i < ctxmax; i++) {
        vb_irq_prio[i] = 0;
    }
    for (int i = 0; i < ctxmax; i++) {
        vb_ctx[i].priority_th = 0;
        vb_ctx[i].ie = 0;
        vb_ctx[i].ip_prio = 0;
        vb_ctx[i].prio_mask = 0;
        vb_ctx[i].sel_prio = 0;
        vb_ctx[i].irq_idx = 0;
        vb_ctx[i].irq_prio = 0;
    }
    vb_src_priority = 0;
    vb_pending = 0;
    vb_ip = 0;
    rctx_idx = 0;

    v.src_priority = r.src_priority;
    v.pending = r.pending;
    v.ip = r.ip;
    for (int i = 0; i < ctxmax; i++) {
        v.ctx[i].priority_th = r.ctx[i].priority_th;
        v.ctx[i].ie = r.ctx[i].ie;
        v.ctx[i].ip_prio = r.ctx[i].ip_prio;
        v.ctx[i].prio_mask = r.ctx[i].prio_mask;
        v.ctx[i].sel_prio = r.ctx[i].sel_prio;
        v.ctx[i].irq_idx = r.ctx[i].irq_idx;
        v.ctx[i].irq_prio = r.ctx[i].irq_prio;
    }
    v.rdata = r.rdata;

    // Warning SystemC limitation workaround:
    //   Cannot directly write into bitfields of the signals v.* registers
    //   So, use the following vb_* logic variables for that and then copy them.
    vb_src_priority = r.src_priority;
    vb_pending = r.pending;
    for (int i = 0; i < ctxmax; i++) {
        vb_ctx[i].priority_th = r.ctx[i].priority_th;
        vb_ctx[i].ie = r.ctx[i].ie;
        vb_ctx[i].irq_idx = r.ctx[i].irq_idx;
        vb_ctx[i].irq_prio = r.ctx[i].irq_prio;
    }

    for (int i = 1; i < irqmax; i++) {
        if ((i_irq_request.read()[i] == 1) && (r.src_priority.read()((4 * i) + 4 - 1, (4 * i)).to_int() > 0)) {
            vb_pending[i] = 1;
        }
    }

    for (int n = 0; n < ctxmax; n++) {
        for (int i = 0; i < irqmax; i++) {
            if ((r.pending.read()[i] == 1)
                    && (r.ctx[n].ie[i] == 1)
                    && (r.src_priority.read()((4 * i) + 4 - 1, (4 * i)).to_int() > r.ctx[n].priority_th)) {
                vb_ctx[n].ip_prio((4 * i) + 4 - 1, (4 * i)) = r.src_priority.read()((4 * i) + 4 - 1, (4 * i));
                vb_ctx[n].prio_mask[r.src_priority.read()((4 * i) + 4 - 1, (4 * i)).to_int()] = 1;
            }
        }
    }

    // Select max priority in each context
    for (int n = 0; n < ctxmax; n++) {
        for (int i = 0; i < 16; i++) {
            if (r.ctx[n].prio_mask[i] == 1) {
                vb_ctx[n].sel_prio = i;
            }
        }
    }

    // Select max priority in each context
    for (int n = 0; n < ctxmax; n++) {
        for (int i = 0; i < irqmax; i++) {
            if (r.ctx[n].sel_prio.or_reduce()
                    && (r.ctx[n].ip_prio((4 * i) + 4 - 1, (4 * i)) == r.ctx[n].sel_prio)) {
                // Most prio irq and prio level
                vb_irq_idx[n] = i;
                vb_irq_prio[n] = r.ctx[n].sel_prio;
            }
        }
    }

    for (int n = 0; n < ctxmax; n++) {
        vb_ctx[n].irq_idx = vb_irq_idx[n];
        vb_ctx[n].irq_prio = vb_irq_prio[n];
        vb_ip[n] = vb_irq_idx[n].or_reduce();
    }

    // R/W registers access:
    rctx_idx = wb_req_addr.read()(20, 12).to_int();
    if (wb_req_addr.read()(21, 12) == 0) {                  // src_prioirty
        // 0x000000..0x001000: Irq 0 unused
        if (wb_req_addr.read()(11, 3).or_reduce() == 1) {
            vrdata(3, 0) = r.src_priority.read()((8 * wb_req_addr.read()(11, 3)) + 4 - 1, (8 * wb_req_addr.read()(11, 3))).to_uint64();
            if ((w_req_valid.read() == 1) && (w_req_write.read() == 1)) {
                if (wb_req_wstrb.read()(3, 0).or_reduce() == 1) {
                    vb_src_priority((8 * wb_req_addr.read()(11, 3)) + 4 - 1, (8 * wb_req_addr.read()(11, 3))) = wb_req_wdata.read()(3, 0);
                }
            }
        }

        vrdata(35, 32) = r.src_priority.read()(((8 * wb_req_addr.read()(11, 3)) + 32) + 4 - 1, ((8 * wb_req_addr.read()(11, 3)) + 32)).to_uint64();
        if ((w_req_valid.read() == 1) && (w_req_write.read() == 1)) {
            if (wb_req_wstrb.read()(7, 4).or_reduce() == 1) {
                vb_src_priority(((8 * wb_req_addr.read()(11, 3)) + 32) + 4 - 1, ((8 * wb_req_addr.read()(11, 3)) + 32)) = wb_req_wdata.read()(35, 32);
            }
        }
    } else if (wb_req_addr.read()(21, 12) == 1) {
        // 0x001000..0x001080
        vrdata = r.pending.read()((64 * wb_req_addr.read()(6, 3)) + 64 - 1, (64 * wb_req_addr.read()(6, 3))).to_uint64();
        if ((w_req_valid.read() == 1) && (w_req_write.read() == 1)) {
            if (wb_req_wstrb.read()(3, 0).or_reduce() == 1) {
                vb_pending((64 * wb_req_addr.read()(6, 3)) + 32 - 1, (64 * wb_req_addr.read()(6, 3))) = wb_req_wdata.read()(31, 0);
            }
            if (wb_req_wstrb.read()(7, 4).or_reduce() == 1) {
                vb_pending(((64 * wb_req_addr.read()(6, 3)) + 32) + 32 - 1, ((64 * wb_req_addr.read()(6, 3)) + 32)) = wb_req_wdata.read()(63, 32);
            }
        }
    } else if ((wb_req_addr.read()(21, 12) == 2)
                && (wb_req_addr.read()(11, 7) < ctxmax)) {
        // First 32 context of 15867 support only
        // 0x002000,0x002080,...,0x200000
        vrdata = r.ctx[wb_req_addr.read()(11, 7)].ie((64 * wb_req_addr.read()(6, 3)) + 64 - 1, (64 * wb_req_addr.read()(6, 3))).to_uint64();
        if ((w_req_valid.read() == 1) && (w_req_write.read() == 1)) {
            if (wb_req_wstrb.read()(3, 0).or_reduce() == 1) {
                vb_ctx[wb_req_addr.read()(11, 7)].ie((64 * wb_req_addr.read()(6, 3)) + 32 - 1, (64 * wb_req_addr.read()(6, 3))) = wb_req_wdata.read()(31, 0);
            }
            if (wb_req_wstrb.read()(7, 4).or_reduce() == 1) {
                vb_ctx[wb_req_addr.read()(11, 7)].ie(((64 * wb_req_addr.read()(6, 3)) + 32) + 32 - 1, ((64 * wb_req_addr.read()(6, 3)) + 32)) = wb_req_wdata.read()(63, 32);
            }
        }
    } else if ((wb_req_addr.read()(21, 12) >= 0x200) && (wb_req_addr.read()(20, 12) < ctxmax)) {
        // 0x200000,0x201000,...,0x4000000
        if (wb_req_addr.read()(11, 3) == 0) {
            // masking (disabling) all interrupt with <= priority
            vrdata(3, 0) = r.ctx[rctx_idx].priority_th;
            vrdata(41, 32) = r.ctx[rctx_idx].irq_idx;
            // claim/ complete. Reading clears pending bit
            if (r.ip.read()[rctx_idx] == 1) {
                vb_pending[r.ctx[rctx_idx].irq_idx] = 0;
            }
            if ((w_req_valid.read() == 1) && (w_req_write.read() == 1)) {
                if (wb_req_wstrb.read()(3, 0).or_reduce() == 1) {
                    vb_ctx[rctx_idx].priority_th = wb_req_wdata.read()(3, 0);
                }
                if (wb_req_wstrb.read()(7, 4).or_reduce() == 1) {
                    // claim/ complete. Reading clears pedning bit
                    vb_ctx[rctx_idx].irq_idx = 0;
                }
            }
        } else {
            // reserved
        }
    }
    v.rdata = vrdata;

    v.src_priority = vb_src_priority;
    v.pending = vb_pending;
    v.ip = vb_ip;
    for (int n = 0; n < ctxmax; n++) {
        v.ctx[n].priority_th = vb_ctx[n].priority_th;
        v.ctx[n].ie = vb_ctx[n].ie;
        v.ctx[n].ip_prio = vb_ctx[n].ip_prio;
        v.ctx[n].prio_mask = vb_ctx[n].prio_mask;
        v.ctx[n].sel_prio = vb_ctx[n].sel_prio;
        v.ctx[n].irq_idx = vb_ctx[n].irq_idx;
        v.ctx[n].irq_prio = vb_ctx[n].irq_prio;
    }

    if (!async_reset_ && i_nrst.read() == 0) {
        v.src_priority = 0;
        v.pending = 0;
        v.ip = 0;
        for (int i = 0; i < ctxmax; i++) {
            v.ctx[i].priority_th = 0;
            v.ctx[i].ie = 0;
            v.ctx[i].ip_prio = 0;
            v.ctx[i].prio_mask = 0;
            v.ctx[i].sel_prio = 0;
            v.ctx[i].irq_idx = 0;
            v.ctx[i].irq_prio = 0;
        }
        v.rdata = 0;
    }

    w_req_ready = 1;
    w_resp_valid = 1;
    wb_resp_rdata = r.rdata;
    wb_resp_err = 0;
    o_ip = r.ip;
}

template<int ctxmax, int irqmax>
void plic<ctxmax, irqmax>::registers() {
    if (async_reset_ && i_nrst.read() == 0) {
        r.src_priority = 0;
        r.pending = 0;
        r.ip = 0;
        for (int i = 0; i < ctxmax; i++) {
            r.ctx[i].priority_th = 0;
            r.ctx[i].ie = 0;
            r.ctx[i].ip_prio = 0;
            r.ctx[i].prio_mask = 0;
            r.ctx[i].sel_prio = 0;
            r.ctx[i].irq_idx = 0;
            r.ctx[i].irq_prio = 0;
        }
        r.rdata = 0;
    } else {
        r.src_priority = v.src_priority;
        r.pending = v.pending;
        r.ip = v.ip;
        for (int i = 0; i < ctxmax; i++) {
            r.ctx[i].priority_th = v.ctx[i].priority_th;
            r.ctx[i].ie = v.ctx[i].ie;
            r.ctx[i].ip_prio = v.ctx[i].ip_prio;
            r.ctx[i].prio_mask = v.ctx[i].prio_mask;
            r.ctx[i].sel_prio = v.ctx[i].sel_prio;
            r.ctx[i].irq_idx = v.ctx[i].irq_idx;
            r.ctx[i].irq_prio = v.ctx[i].irq_prio;
        }
        r.rdata = v.rdata;
    }
}

}  // namespace debugger

