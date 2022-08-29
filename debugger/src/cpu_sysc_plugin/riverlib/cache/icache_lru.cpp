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

#include "icache_lru.h"

namespace debugger {

ICacheLru::ICacheLru(sc_module_name name_, bool async_reset)
    : sc_module(name_),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_req_valid("i_req_valid"),
    i_req_addr("i_req_addr"),
    o_req_ready("o_req_ready"),
    o_resp_valid("o_resp_valid"),
    o_resp_addr("o_resp_addr"),
    o_resp_data("o_resp_data"),
    o_resp_load_fault("o_resp_load_fault"),
    o_resp_executable("o_resp_executable"),
    o_resp_writable("o_resp_writable"),
    o_resp_readable("o_resp_readable"),
    i_resp_ready("i_resp_ready"),
    i_req_mem_ready("i_req_mem_ready"),
    o_req_mem_valid("o_req_mem_valid"),
    o_req_mem_type("o_req_mem_type"),
    o_req_mem_size("o_req_mem_size"),
    o_req_mem_addr("o_req_mem_addr"),
    o_req_mem_strob("o_req_mem_strob"),
    o_req_mem_data("o_req_mem_data"),
    i_mem_data_valid("i_mem_data_valid"),
    i_mem_data("i_mem_data"),
    i_mem_load_fault("i_mem_load_fault"),
    o_mpu_addr("o_mpu_addr"),
    i_mpu_flags("i_mpu_flags"),
    i_flush_address("i_flush_address"),
    i_flush_valid("i_flush_valid") {
    async_reset_ = async_reset;

    memcouple = new TagMemCoupled<CFG_CPU_ADDR_BITS,
                            CFG_ILOG2_NWAYS,
                            CFG_ILOG2_LINES_PER_WAY,
                            CFG_ILOG2_BYTES_PER_LINE,
                            ITAG_FL_TOTAL>("memcouple0", async_reset);

    memcouple->i_clk(i_clk);
    memcouple->i_nrst(i_nrst);
    memcouple->i_direct_access(line_direct_access_i);
    memcouple->i_invalidate(line_invalidate_i);
    memcouple->i_re(line_re_i);
    memcouple->i_we(line_we_i);
    memcouple->i_addr(line_addr_i);
    memcouple->i_wdata(line_wdata_i);
    memcouple->i_wstrb(line_wstrb_i);
    memcouple->i_wflags(line_wflags_i);
    memcouple->o_raddr(line_raddr_o);
    memcouple->o_rdata(line_rdata_o);
    memcouple->o_rflags(line_rflags_o);
    memcouple->o_hit(line_hit_o);
    memcouple->o_hit_next(line_hit_next_o);


    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_req_valid;
    sensitive << i_req_addr;
    sensitive << i_mem_data_valid;
    sensitive << i_mem_data;
    sensitive << i_mem_load_fault;
    sensitive << i_resp_ready;
    sensitive << i_flush_address;
    sensitive << i_flush_valid;
    sensitive << i_req_mem_ready;
    sensitive << i_mpu_flags;
    sensitive << line_raddr_o;
    sensitive << line_rdata_o;
    sensitive << line_rflags_o;
    sensitive << line_hit_o;
    sensitive << line_hit_next_o;
    sensitive << r.req_addr;
    sensitive << r.state;
    sensitive << r.req_mem_valid;
    sensitive << r.mem_addr;
    sensitive << r.req_mem_type;
    sensitive << r.req_mem_size;
    sensitive << r.load_fault;
    sensitive << r.req_flush;
    sensitive << r.req_flush_all;
    sensitive << r.req_flush_addr;
    sensitive << r.req_flush_cnt;
    sensitive << r.flush_cnt;

    SC_METHOD(registers);
    sensitive << i_nrst;
    sensitive << i_clk.pos();
};

ICacheLru::~ICacheLru() {
    delete memcouple;
}

void ICacheLru::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    if (o_vcd) {
        sc_trace(o_vcd, i_nrst, i_nrst.name());
        sc_trace(o_vcd, i_req_valid, i_req_valid.name());
        sc_trace(o_vcd, i_req_addr, i_req_addr.name());
        sc_trace(o_vcd, o_req_ready, o_req_ready.name());
        sc_trace(o_vcd, o_resp_valid, o_resp_valid.name());
        sc_trace(o_vcd, o_resp_addr, o_resp_addr.name());
        sc_trace(o_vcd, o_resp_data, o_resp_data.name());
        sc_trace(o_vcd, o_resp_load_fault, o_resp_load_fault.name());
        sc_trace(o_vcd, i_resp_ready, i_resp_ready.name());
        sc_trace(o_vcd, o_resp_executable, o_resp_executable.name());

        sc_trace(o_vcd, i_req_mem_ready, i_req_mem_ready.name());
        sc_trace(o_vcd, o_req_mem_valid, o_req_mem_valid.name());
        sc_trace(o_vcd, o_req_mem_type, o_req_mem_type.name());
        sc_trace(o_vcd, o_req_mem_addr, o_req_mem_addr.name());
        sc_trace(o_vcd, o_req_mem_strob, o_req_mem_strob.name());
        sc_trace(o_vcd, o_req_mem_data, o_req_mem_data.name());
        sc_trace(o_vcd, i_mem_data_valid, i_mem_data_valid.name());
        sc_trace(o_vcd, i_mem_data, i_mem_data.name());
        sc_trace(o_vcd, i_mem_load_fault, i_mem_load_fault.name());

        sc_trace(o_vcd, i_flush_address, i_flush_address.name());
        sc_trace(o_vcd, i_flush_valid, i_flush_valid.name());

        std::string pn(name());
        sc_trace(o_vcd, r.state, pn + ".r_state");
        sc_trace(o_vcd, r.req_addr, pn + ".r_req_addr");
        sc_trace(o_vcd, r.req_mem_type, pn + ".r_req_mem_type");
        sc_trace(o_vcd, r.cache_line_i, pn + ".r_cache_line_i");
        sc_trace(o_vcd, r.executable, pn + ".r_executable");
        sc_trace(o_vcd, r.flush_cnt, pn + ".r_flush_cnt");
        sc_trace(o_vcd, r.req_flush, pn + ".r_req_flush");
        sc_trace(o_vcd, r.req_flush_addr, pn + ".r_req_flush_addr");
        sc_trace(o_vcd, r.req_flush_cnt, pn + ".r_req_flush_cnt");
    }
    //memcouple->generateVCD(i_vcd, o_vcd);
}

void ICacheLru::comb() {
    sc_biguint<ICACHE_LINE_BITS> t_cache_line_i;
    bool v_req_ready;
    bool v_resp_valid;
    sc_uint<64> vb_cached_data;
    sc_uint<64> vb_uncached_data;
    sc_uint<64> vb_resp_data;
    bool v_resp_er_load_fault;
    bool v_direct_access;
    bool v_invalidate;
    bool v_line_cs_read;
    bool v_line_cs_write;
    sc_uint<CFG_CPU_ADDR_BITS> vb_line_addr;
    sc_biguint<ICACHE_LINE_BITS> vb_line_wdata;
    sc_uint<ICACHE_BYTES_PER_LINE> vb_line_wstrb;
    sc_uint<ITAG_FL_TOTAL> v_line_wflags;
    sc_uint<CFG_ILOG2_BYTES_PER_LINE-2> sel_cached;
    int sel_uncached;
    bool v_ready_next;
    sc_uint<CFG_CPU_ADDR_BITS> vb_addr_direct_next;

    v = r;

    v_ready_next = 0;
    v_req_ready = 0;
    v_resp_valid = 0;
    vb_resp_data = 0;
    v_resp_er_load_fault = 0;
    v_direct_access = 0;
    v_invalidate = 0;
    sel_cached = r.req_addr.read()(CFG_ILOG2_BYTES_PER_LINE-1, 2).to_int();
    sel_uncached = r.req_addr.read()(2, 2).to_int();

    vb_cached_data = line_rdata_o.read()(32*sel_cached.to_int() + 63,
                                         32*sel_cached.to_int());
    vb_uncached_data = r.cache_line_i.read()(32*sel_uncached + 63,
                                             32*sel_uncached);


    // flush request via debug interface
    if (i_flush_valid.read() == 1) {
        v.req_flush = 1;
        v.req_flush_all = i_flush_address.read()[0];
        if (i_flush_address.read()[0] == 1) {
            v.req_flush_cnt = ~0u;
            v.req_flush_addr = 0;
        } else if (i_flush_address.read()(CFG_ILOG2_BYTES_PER_LINE-1, 1).and_reduce() == 1) {
            v.req_flush_cnt = 1;
            v.req_flush_addr = i_flush_address.read();
        } else {
            v.req_flush_cnt = 0;
            v.req_flush_addr = i_flush_address.read();
        }
    }

    // Flush counter when direct access
    if (r.req_addr.read()(CFG_ILOG2_NWAYS-1, 0) == ICACHE_WAYS-1) {
        vb_addr_direct_next = (r.req_addr.read() + ICACHE_BYTES_PER_LINE) 
                    & ~((1<<CFG_ILOG2_BYTES_PER_LINE)-1);
    } else {
        vb_addr_direct_next = r.req_addr.read() + 1;
    }

    v_line_cs_read = 0;
    v_line_cs_write = 0;
    vb_line_addr = r.req_addr.read();
    vb_line_wdata = r.cache_line_i.read();
    vb_line_wstrb = 0;
    v_line_wflags = 0;

    switch (r.state.read()) {
    case State_Idle:
        v.executable = 1;
        v_ready_next = 1;
        break;
    case State_CheckHit:
        vb_resp_data = vb_cached_data;
        if (line_hit_o.read() == 1 && line_hit_next_o.read() == 1) {
            // Hit
            v_resp_valid = 1;
            if (i_resp_ready.read() == 1) {
                v_ready_next = 1;
                v.state = State_Idle;
            }
        } else {
            // Miss
            v.state = State_TranslateAddress;
        }
        break;
    case State_TranslateAddress:
        if (i_mpu_flags.read()[CFG_MPU_FL_EXEC] == 0) {
            t_cache_line_i = 0;
            v.cache_line_i = ~t_cache_line_i;
            v.state = State_CheckResp;
        } else {
            v.req_mem_valid = 1;
            v.state = State_WaitGrant;
            v.write_addr = r.req_addr;

            if (i_mpu_flags.read()[CFG_MPU_FL_CACHABLE] == 1) {
                if (line_hit_o.read() == 0) {
                    v.mem_addr = r.req_addr.read()(CFG_CPU_ADDR_BITS-1,
                            CFG_ILOG2_BYTES_PER_LINE) << CFG_ILOG2_BYTES_PER_LINE;
                } else {
                    v.write_addr = r.req_addr_next;
                    v.mem_addr = r.req_addr_next.read()(CFG_CPU_ADDR_BITS-1,
                            CFG_ILOG2_BYTES_PER_LINE) << CFG_ILOG2_BYTES_PER_LINE;
                }
                v.req_mem_type = ReadShared();
                v.req_mem_size = CFG_ILOG2_BYTES_PER_LINE;
            } else {
                v.mem_addr = r.req_addr.read()(CFG_CPU_ADDR_BITS-1, 3) << 3;
                v.req_mem_type = ReadNoSnoop();
                v.req_mem_size = 4; // uncached, 16 B
            }
        }

        v.load_fault = 0;
        v.executable = i_mpu_flags.read()[CFG_MPU_FL_EXEC];
        break;
    case State_WaitGrant:
        if (i_req_mem_ready.read()) {
            v.state = State_WaitResp;
            v.req_mem_valid = 0;
        }
        break;
    case State_WaitResp:
        if (i_mem_data_valid.read()) {
            v.cache_line_i = i_mem_data.read();
            v.state = State_CheckResp;
            v.write_addr = r.req_addr;      // Swap addres for 1 clock to write line
            v.req_addr = r.write_addr;
            if (i_mem_load_fault.read() == 1) {
                v.load_fault = 1;
            }
        }
        break;
    case State_CheckResp:
        v.req_addr = r.write_addr;              // Restore req_addr after line write
        if (r.req_mem_type.read()[REQ_MEM_TYPE_CACHED] == 0 ||
            r.load_fault.read() == 1) {
            v_resp_valid = 1;
            vb_resp_data = vb_uncached_data;
            v_resp_er_load_fault = r.load_fault;
            if (i_resp_ready.read() == 1) {
                v.state = State_Idle;
            }
        } else {
            v.state = State_SetupReadAdr;
            v_line_cs_write = 1;
            v_line_wflags[TAG_FL_VALID] = 1;
            vb_line_wstrb = ~0ul;  // write full line
        }
        break;
    case State_SetupReadAdr:
        v.state = State_CheckHit;
        break;
    case State_FlushAddr:
        v.state = State_FlushCheck;
        v_direct_access = r.req_flush_all;      // 0=only if hit; 1=will be applied ignoring hit
        v_invalidate = 1;                       // generate: wstrb='1; wflags='0
        v.cache_line_i = 0;
        break;
    case State_FlushCheck:
        v.state = State_FlushAddr;
        v_direct_access = r.req_flush_all;
        v_line_cs_write = r.req_flush_all;
        if (r.flush_cnt.read().or_reduce() == 1) {
            v.flush_cnt = r.flush_cnt.read() - 1;
            if (r.req_flush_all == 1) {
                v.req_addr = vb_addr_direct_next;
            } else {
                v.req_addr = r.req_addr.read() + ICACHE_BYTES_PER_LINE;
            }
        } else {
            v.state = State_Idle;
        }
        break;
    case State_Reset:
        /** Write clean line */
        v_direct_access = 1;
        v_invalidate = 1;                       // generate: wstrb='1; wflags='0
        v.state = State_ResetWrite;
        break;
    case State_ResetWrite:
        v_direct_access = 1;
        v_line_cs_write = 1;
        v.state = State_Reset;

        if (r.flush_cnt.read().or_reduce() == 1) {
            v.flush_cnt = r.flush_cnt.read() - 1;
            v.req_addr = vb_addr_direct_next;
        } else {
            v.state = State_Idle;
        }
        break;
    default:;
    }

    if (v_ready_next == 1) {
        if (r.req_flush.read() == 1) {
            v.state = State_FlushAddr;
            v.req_flush = 0;
            v.cache_line_i = 0;
            v.req_addr = r.req_flush_addr.read() & ~((1<<CFG_ILOG2_BYTES_PER_LINE)-1);
            v.flush_cnt = r.req_flush_cnt.read();
        } else {
            v_req_ready = 1;
            v_line_cs_read = i_req_valid.read();
            vb_line_addr = i_req_addr.read();
            if (i_req_valid.read() == 1) {
                v.req_addr = i_req_addr.read();
                v.req_addr_next = i_req_addr.read() + ICACHE_BYTES_PER_LINE;
                v.state = State_CheckHit;
            }
        }
    }

    if (!async_reset_ && !i_nrst.read()) {
        R_RESET(v);
    }

    line_direct_access_i = v_direct_access;
    line_invalidate_i = v_invalidate;
    line_re_i = v_line_cs_read;
    line_we_i = v_line_cs_write;
    line_addr_i = vb_line_addr;
    line_wdata_i = vb_line_wdata;
    line_wstrb_i = vb_line_wstrb;
    line_wflags_i = v_line_wflags;

    o_req_ready = v_req_ready;

    o_req_mem_valid = r.req_mem_valid.read();
    o_req_mem_addr = r.mem_addr.read();
    o_req_mem_type = r.req_mem_type.read();
    o_req_mem_size = r.req_mem_size.read();
    o_req_mem_strob = 0;
    o_req_mem_data = 0;

    o_resp_valid = v_resp_valid;
    o_resp_data = vb_resp_data;
    o_resp_addr = r.req_addr.read();
    o_resp_load_fault = v_resp_er_load_fault;
    o_resp_executable = r.executable;
    o_resp_writable = 0;
    o_resp_readable = 0;
    o_mpu_addr = r.req_addr.read();
}

void ICacheLru::registers() {
    if (async_reset_ && i_nrst.read() == 0) {
        R_RESET(r);
    } else {
        r = v;
    }
}

#ifdef DBG_ICACHE_LRU_TB
ICacheLru_tb::ICacheLru_tb(sc_module_name name_) : sc_module(name_),
    w_clk("clk0", 10, SC_NS) {
    SC_METHOD(comb0);
    sensitive << w_nrst;
    sensitive << r.clk_cnt;

    SC_METHOD(comb_fetch);
    sensitive << w_nrst;
    sensitive << w_req_ctrl_ready;
    sensitive << w_resp_ctrl_valid;
    sensitive << wb_resp_ctrl_addr;
    sensitive << wb_resp_ctrl_data;
    sensitive << w_resp_ctrl_load_fault;
    sensitive << r.clk_cnt;

    SC_METHOD(comb_bus);
    sensitive << w_nrst;
    sensitive << w_req_mem_valid;
    sensitive << w_req_mem_write;
    sensitive << wb_req_mem_addr;
    sensitive << wb_req_mem_strob;
    sensitive << wb_req_mem_data;
    sensitive << rbus.state;
    sensitive << rbus.burst_addr;
    sensitive << rbus.burst_cnt;

    SC_METHOD(registers);
    sensitive << w_clk.posedge_event();

    tt = new ICacheLru("tt", 0, CFG_IINDEX_WIDTH);
    tt->i_clk(w_clk);
    tt->i_nrst(w_nrst);
    tt->i_req_ctrl_valid(w_req_ctrl_valid);
    tt->i_req_ctrl_addr(wb_req_ctrl_addr);
    tt->o_req_ctrl_ready(w_req_ctrl_ready);
    tt->o_resp_ctrl_valid(w_resp_ctrl_valid);
    tt->o_resp_ctrl_addr(wb_resp_ctrl_addr);
    tt->o_resp_ctrl_data(wb_resp_ctrl_data);
    tt->o_resp_ctrl_load_fault(w_resp_ctrl_load_fault);
    tt->i_resp_ctrl_ready(w_resp_ctrl_ready);
    tt->i_req_mem_ready(w_req_mem_ready);
    tt->o_req_mem_valid(w_req_mem_valid);
    tt->o_req_mem_write(w_req_mem_write);
    tt->o_req_mem_addr(wb_req_mem_addr);
    tt->o_req_mem_strob(wb_req_mem_strob);
    tt->o_req_mem_data(wb_req_mem_data);
    tt->i_resp_mem_data_valid(w_resp_mem_data_valid);
    tt->i_resp_mem_data(wb_resp_mem_data);
    tt->i_resp_mem_load_fault(w_resp_mem_load_fault);
    tt->i_flush_address(wb_flush_address);
    tt->i_flush_valid(w_flush_valid);
    tt->o_istate(wb_istate);

    tb_vcd = sc_create_vcd_trace_file("ICacheLru_tb");
    tb_vcd->set_time_unit(1, SC_PS);
    sc_trace(tb_vcd, w_nrst, "w_nrst");
    sc_trace(tb_vcd, w_clk, "w_clk");
    sc_trace(tb_vcd, r.clk_cnt, "clk_cnt");
    sc_trace(tb_vcd, w_req_ctrl_valid, "w_req_ctrl_valid");
    sc_trace(tb_vcd, wb_req_ctrl_addr, "wb_req_ctrl_addr");
    sc_trace(tb_vcd, w_req_ctrl_ready, "w_req_ctrl_ready");
    sc_trace(tb_vcd, w_resp_ctrl_valid, "w_resp_ctrl_valid");
    sc_trace(tb_vcd, wb_resp_ctrl_addr, "wb_resp_ctrl_addr");
    sc_trace(tb_vcd, wb_resp_ctrl_data, "wb_resp_ctrl_data");
    sc_trace(tb_vcd, w_resp_ctrl_ready, "w_resp_ctrl_ready");
    sc_trace(tb_vcd, w_req_mem_ready, "w_req_mem_ready");
    sc_trace(tb_vcd, w_req_mem_valid, "w_req_mem_valid");
    sc_trace(tb_vcd, w_req_mem_write, "w_req_mem_write");
    sc_trace(tb_vcd, wb_req_mem_addr, "wb_req_mem_addr");
    sc_trace(tb_vcd, wb_req_mem_strob, "wb_req_mem_strob");
    sc_trace(tb_vcd, wb_req_mem_data, "wb_req_mem_data");
    sc_trace(tb_vcd, w_resp_mem_data_valid, "w_resp_mem_data_valid");
    sc_trace(tb_vcd, wb_resp_mem_data, "wb_resp_mem_data");
    sc_trace(tb_vcd, wb_istate, "wb_istate");
    sc_trace(tb_vcd, rbus.burst_addr, "rbus_burst_addr");
    sc_trace(tb_vcd, rbus.burst_cnt, "rbus_burst_cnt");

    tt->generateVCD(tb_vcd, tb_vcd);
}


void ICacheLru_tb::comb0() {
    v = r;
    v.clk_cnt = r.clk_cnt.read() + 1;

    w_flush_valid = 0;
    wb_flush_address = 0;

    if (r.clk_cnt.read() < 10) {
        w_nrst = 0;
    } else {
        w_nrst = 1;
    }

}

void ICacheLru_tb::comb_fetch() {
    w_req_ctrl_valid = 0;
    wb_req_ctrl_addr = 0;

    switch (r.clk_cnt.read()) {
    case 10 + 1 + (1 << (2*(CFG_IINDEX_WIDTH+1))):
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x00000000;
        break;
    case 1021:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x100008f4;
        break;
    case 1024:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x100008f6;
        break;
    case 1025:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x100008fa;
        break;
    case 1026:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x100008fe;
        break;

    case 1050:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x0000001e;
        break;
    case 1060:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x0000201e;
        break;
    case 1070:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x0010001e;
        break;
    case 1081:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x0010201e;
        break;

    case 1100:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x00000004;
        break;
    case 1101:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x00000008;
        break;
    case 1102:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x00002008;
        break;
    case 1103:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x0000200C;
        break;
    case 1104:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x00102010;
        break;
    case 1105:
        w_req_ctrl_valid = 1;
        wb_req_ctrl_addr = 0x00102014;
        break;
    default:;
    }
}

void ICacheLru_tb::comb_bus() {
    sc_uint<CFG_IOFFSET_WIDTH> wrap_offset;
    sc_uint<32> wb_addr4;
    vbus = rbus;

    w_req_mem_ready = 0;
    w_resp_mem_data_valid = 0;
    wb_resp_mem_data = 0;
    wb_addr4 = rbus.burst_addr.read() + 4;

    switch (rbus.state.read()) {
    case BUS_Idle:
        w_req_mem_ready = 1;
        if (w_req_mem_valid.read() == 1) {
            vbus.state = BUS_Read;
            vbus.burst_addr = wb_req_mem_addr.read();
            vbus.burst_cnt = 3;
        }
        break;
    case BUS_Read:
        w_resp_mem_data_valid = 1;
        wb_resp_mem_data = (wb_addr4, rbus.burst_addr.read());
        vbus.burst_cnt = rbus.burst_cnt.read() - 1;
        wrap_offset = rbus.burst_addr.read()(CFG_IOFFSET_WIDTH-1, 0) + 8;
        // WRAP burst transaction type
        vbus.burst_addr =
            (rbus.burst_addr.read()(31, CFG_IOFFSET_WIDTH), wrap_offset);
        if (rbus.burst_cnt.read() == 1) {
            vbus.state = BUS_ReadLast;
        }
        break;
    case BUS_ReadLast:
        w_req_mem_ready = 1;
        w_resp_mem_data_valid = 1;
        wb_resp_mem_data = (wb_addr4, rbus.burst_addr.read());
        if (w_req_mem_valid.read() == 1) {
            vbus.state = BUS_Read;
            vbus.burst_addr = wb_req_mem_addr.read();
            vbus.burst_cnt = 3;
        } else {
            vbus.state = BUS_Idle;
            vbus.burst_cnt = 0;
        }
        break;
    default:;
    }

    if (w_nrst.read() == 0) {
        vbus.state = BUS_Idle;
        vbus.burst_addr = 0;
        vbus.burst_cnt = 0;
    }
}

#endif

}  // namespace debugger

