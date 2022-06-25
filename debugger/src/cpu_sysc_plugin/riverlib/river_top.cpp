/*
 *  Copyright 2018 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "river_top.h"

namespace debugger {

RiverTop::RiverTop(sc_module_name name_, bool async_reset, uint32_t hartid,
    bool fpu_ena, bool coherence_ena, bool tracer_ena) : sc_module(name_),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_req_mem_ready("i_req_mem_ready"),
    o_req_mem_path("o_req_mem_path"),
    o_req_mem_valid("o_req_mem_valid"),
    o_req_mem_type("o_req_mem_type"),
    o_req_mem_size("o_req_mem_size"),
    o_req_mem_addr("o_req_mem_addr"),
    o_req_mem_strob("o_req_mem_strob"),
    o_req_mem_data("o_req_mem_data"),
    i_resp_mem_valid("i_resp_mem_valid"),
    i_resp_mem_path("i_resp_mem_path"),
    i_resp_mem_data("i_resp_mem_data"),
    i_resp_mem_load_fault("i_resp_mem_load_fault"),
    i_resp_mem_store_fault("i_resp_mem_store_fault"),
    i_req_snoop_valid("i_req_snoop_valid"),
    i_req_snoop_type("i_req_snoop_type"),
    o_req_snoop_ready("o_req_snoop_ready"),
    i_req_snoop_addr("i_req_snoop_addr"),
    i_resp_snoop_ready("i_resp_snoop_ready"),
    o_resp_snoop_valid("o_resp_snoop_valid"),
    o_resp_snoop_data("o_resp_snoop_data"),
    o_resp_snoop_flags("o_resp_snoop_flags"),
    i_msip("i_msip"),
    i_mtip("i_mtip"),
    i_meip("i_meip"),
    i_seip("i_seip"),
    i_haltreq("i_haltreq"),
    i_resumereq("i_resumereq"),
    i_dport_req_valid("i_dport_req_valid"),
    i_dport_type("i_dport_type"),
    i_dport_addr("i_dport_addr"),
    i_dport_wdata("i_dport_wdata"),
    i_dport_size("i_dport_size"),
    o_dport_req_ready("o_dport_req_ready"),
    i_dport_resp_ready("i_dport_resp_ready"),
    o_dport_resp_valid("o_dport_resp_valid"),
    o_dport_resp_error("o_dport_resp_error"),
    o_dport_rdata("o_dport_rdata"),
    i_progbuf("i_progbuf"),
    o_halted("o_halted") {

    proc0 = new Processor("proc0", async_reset, hartid, fpu_ena, tracer_ena);
    proc0->i_clk(i_clk);
    proc0->i_nrst(i_nrst);
    proc0->i_req_ctrl_ready(w_req_ctrl_ready);
    proc0->o_req_ctrl_valid(w_req_ctrl_valid);
    proc0->o_req_ctrl_addr(wb_req_ctrl_addr);
    proc0->i_resp_ctrl_valid(w_resp_ctrl_valid);
    proc0->i_resp_ctrl_addr(wb_resp_ctrl_addr);
    proc0->i_resp_ctrl_data(wb_resp_ctrl_data);
    proc0->i_resp_ctrl_load_fault(w_resp_ctrl_load_fault);
    proc0->i_resp_ctrl_executable(w_resp_ctrl_executable);
    proc0->o_resp_ctrl_ready(w_resp_ctrl_ready);
    proc0->i_req_data_ready(w_req_data_ready);
    proc0->o_req_data_valid(w_req_data_valid);
    proc0->o_req_data_type(wb_req_data_type);
    proc0->o_req_data_addr(wb_req_data_addr);
    proc0->o_req_data_wdata(wb_req_data_wdata);
    proc0->o_req_data_wstrb(wb_req_data_wstrb);
    proc0->o_req_data_size(wb_req_data_size);
    proc0->i_resp_data_valid(w_resp_data_valid);
    proc0->i_resp_data_addr(wb_resp_data_addr);
    proc0->i_resp_data_data(wb_resp_data_data);
    proc0->i_resp_data_fault_addr(wb_resp_data_fault_addr);
    proc0->i_resp_data_load_fault(w_resp_data_load_fault);
    proc0->i_resp_data_store_fault(w_resp_data_store_fault);
    proc0->i_resp_data_er_mpu_load(w_resp_data_er_mpu_load);
    proc0->i_resp_data_er_mpu_store(w_resp_data_er_mpu_store);
    proc0->o_resp_data_ready(w_resp_data_ready);
    proc0->i_msip(i_msip);
    proc0->i_mtip(i_mtip);
    proc0->i_meip(i_meip);
    proc0->i_seip(i_seip);
    proc0->o_mpu_region_we(w_mpu_region_we);
    proc0->o_mpu_region_idx(wb_mpu_region_idx);
    proc0->o_mpu_region_addr(wb_mpu_region_addr);
    proc0->o_mpu_region_mask(wb_mpu_region_mask);
    proc0->o_mpu_region_flags(wb_mpu_region_flags);
    proc0->i_haltreq(i_haltreq);
    proc0->i_resumereq(i_resumereq);
    proc0->i_dport_req_valid(i_dport_req_valid);
    proc0->i_dport_type(i_dport_type);
    proc0->i_dport_addr(i_dport_addr);
    proc0->i_dport_wdata(i_dport_wdata);
    proc0->i_dport_size(i_dport_size);
    proc0->o_dport_req_ready(o_dport_req_ready);
    proc0->i_dport_resp_ready(i_dport_resp_ready);
    proc0->o_dport_resp_valid(o_dport_resp_valid);
    proc0->o_dport_resp_error(o_dport_resp_error);
    proc0->o_dport_rdata(o_dport_rdata);
    proc0->i_progbuf(i_progbuf);
    proc0->o_halted(o_halted);
    proc0->o_flush_address(wb_flush_address);
    proc0->o_flush_valid(w_flush_valid);
    proc0->o_data_flush_address(wb_data_flush_address);
    proc0->o_data_flush_valid(w_data_flush_valid);
    proc0->i_data_flush_end(w_data_flush_end);

    cache0 = new CacheTop("cache0", async_reset, coherence_ena);
    cache0->i_clk(i_clk);
    cache0->i_nrst(i_nrst);
    cache0->i_req_ctrl_valid(w_req_ctrl_valid);
    cache0->i_req_ctrl_addr(wb_req_ctrl_addr);
    cache0->o_req_ctrl_ready(w_req_ctrl_ready);
    cache0->o_resp_ctrl_valid(w_resp_ctrl_valid);
    cache0->o_resp_ctrl_addr(wb_resp_ctrl_addr);
    cache0->o_resp_ctrl_data(wb_resp_ctrl_data);
    cache0->o_resp_ctrl_load_fault(w_resp_ctrl_load_fault);
    cache0->o_resp_ctrl_executable(w_resp_ctrl_executable);
    cache0->i_resp_ctrl_ready(w_resp_ctrl_ready);
    cache0->i_req_data_valid(w_req_data_valid);
    cache0->i_req_data_type(wb_req_data_type);
    cache0->i_req_data_addr(wb_req_data_addr);
    cache0->i_req_data_wdata(wb_req_data_wdata);
    cache0->i_req_data_wstrb(wb_req_data_wstrb);
    cache0->i_req_data_size(wb_req_data_size);
    cache0->o_req_data_ready(w_req_data_ready);
    cache0->o_resp_data_valid(w_resp_data_valid);
    cache0->o_resp_data_addr(wb_resp_data_addr);
    cache0->o_resp_data_data(wb_resp_data_data);
    cache0->o_resp_data_fault_addr(wb_resp_data_fault_addr);
    cache0->o_resp_data_load_fault(w_resp_data_load_fault);
    cache0->o_resp_data_store_fault(w_resp_data_store_fault);
    cache0->o_resp_data_er_mpu_load(w_resp_data_er_mpu_load);
    cache0->o_resp_data_er_mpu_store(w_resp_data_er_mpu_store);
    cache0->i_resp_data_ready(w_resp_data_ready);
    cache0->i_req_mem_ready(i_req_mem_ready);
    cache0->o_req_mem_path(o_req_mem_path);
    cache0->o_req_mem_valid(o_req_mem_valid);
    cache0->o_req_mem_type(o_req_mem_type);
    cache0->o_req_mem_size(o_req_mem_size);
    cache0->o_req_mem_addr(o_req_mem_addr);
    cache0->o_req_mem_strob(o_req_mem_strob);
    cache0->o_req_mem_data(o_req_mem_data);
    cache0->i_resp_mem_valid(i_resp_mem_valid);
    cache0->i_resp_mem_path(i_resp_mem_path);
    cache0->i_resp_mem_data(i_resp_mem_data);
    cache0->i_resp_mem_load_fault(i_resp_mem_load_fault);
    cache0->i_resp_mem_store_fault(i_resp_mem_store_fault);
    cache0->i_mpu_region_we(w_mpu_region_we);
    cache0->i_mpu_region_idx(wb_mpu_region_idx);
    cache0->i_mpu_region_addr(wb_mpu_region_addr);
    cache0->i_mpu_region_mask(wb_mpu_region_mask);
    cache0->i_mpu_region_flags(wb_mpu_region_flags);
    cache0->i_req_snoop_valid(i_req_snoop_valid);
    cache0->i_req_snoop_type(i_req_snoop_type);
    cache0->o_req_snoop_ready(o_req_snoop_ready);
    cache0->i_req_snoop_addr(i_req_snoop_addr);
    cache0->i_resp_snoop_ready(i_resp_snoop_ready);
    cache0->o_resp_snoop_valid(o_resp_snoop_valid);
    cache0->o_resp_snoop_data(o_resp_snoop_data);
    cache0->o_resp_snoop_flags(o_resp_snoop_flags);
    cache0->i_flush_address(wb_flush_address);
    cache0->i_flush_valid(w_flush_valid);
    cache0->i_data_flush_address(wb_data_flush_address);
    cache0->i_data_flush_valid(w_data_flush_valid);
    cache0->o_data_flush_end(w_data_flush_end);
};

RiverTop::~RiverTop() {
    delete cache0;
    delete proc0;
}

void RiverTop::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    /**
     * ModelSim commands for automatic comparision Stimulus vs SystemC reference:
     *
     * Convert VCD to WLF and back to VCD because ModelSim supports only 
     * 1-bit signals, such conversion allows to create compatible VCD-file.
     * 
     * 1. Prepare compatible VCD/wlf files:
     *      vcd2wlf E:/../win32build/Debug/i_river.vcd -o e:/i_river.wlf
     *      vcd2wlf E:/../win32build/Debug/o_river.vcd -o e:/o_river.wlf
     *      wlf2vcd e:/i_river.wlf -o e:/i_river.vcd
     *
     * 2. Add waves to simulation view and simulate 350 us:
     *      vsim -t 1ps -vcdstim E:/i_river.vcd riverlib.RiverTop
     *      vsim -view e:/o_river.wlf
     *      add wave o_river:/SystemC/ *
     *      add wave sim:/rivertop/ *
     *      run 350us
     *
     * 3. Start automatic comparision:
     *      compare start o_river sim
     *      compare add -wave sim:/RiverTop/o_req_mem_valid o_river:/SystemC/o_req_mem_valid
     *      compare add -wave sim:/RiverTop/o_req_mem_write o_river:/SystemC/o_req_mem_write
     *      compare add -wave sim:/RiverTop/o_req_mem_addr o_river:/SystemC/o_req_mem_addr
     *      compare add -wave sim:/RiverTop/o_req_mem_strob o_river:/SystemC/o_req_mem_strob
     *      compare add -wave sim:/RiverTop/o_req_mem_data o_river:/SystemC/o_req_mem_data
     *      compare add -wave sim:/RiverTop/o_step_cnt o_river:/SystemC/o_step_cnt
     *      compare run
     *
     */
    if (i_vcd) {
        sc_trace(i_vcd, i_clk, i_clk.name());
        sc_trace(i_vcd, i_nrst, i_nrst.name());
        sc_trace(i_vcd, i_req_mem_ready, i_req_mem_ready.name());
        sc_trace(i_vcd, i_resp_mem_valid, i_resp_mem_valid.name());
        sc_trace(i_vcd, i_resp_mem_data, i_resp_mem_data.name());
        sc_trace(i_vcd, i_resp_mem_load_fault, i_resp_mem_load_fault.name());
        sc_trace(i_vcd, i_resp_mem_store_fault, i_resp_mem_store_fault.name());
        sc_trace(i_vcd, i_msip, i_msip.name());
        sc_trace(i_vcd, i_mtip, i_mtip.name());
        sc_trace(i_vcd, i_meip, i_meip.name());
        sc_trace(i_vcd, i_seip, i_seip.name());
        sc_trace(i_vcd, i_haltreq, i_haltreq.name());
        sc_trace(i_vcd, i_resumereq, i_resumereq.name());
        sc_trace(i_vcd, i_dport_req_valid, i_dport_req_valid.name());
        sc_trace(i_vcd, i_dport_type, i_dport_type.name());
        sc_trace(i_vcd, i_dport_addr, i_dport_addr.name());
        sc_trace(i_vcd, i_dport_wdata, i_dport_wdata.name());
        sc_trace(i_vcd, i_dport_size, i_dport_size.name());
        sc_trace(i_vcd, i_dport_resp_ready, i_dport_resp_ready.name());
        sc_trace(i_vcd, i_progbuf, i_progbuf.name());
    }
    if (o_vcd) {
        sc_trace(o_vcd, o_req_mem_valid, o_req_mem_valid.name());
        sc_trace(o_vcd, o_req_mem_type, o_req_mem_type.name());
        sc_trace(o_vcd, o_req_mem_addr, o_req_mem_addr.name());
        sc_trace(o_vcd, o_req_mem_strob, o_req_mem_strob.name());
        sc_trace(o_vcd, o_req_mem_data, o_req_mem_data.name());
        sc_trace(o_vcd, o_halted, o_halted.name());
        sc_trace(o_vcd, o_dport_req_ready, o_dport_req_ready.name());
        sc_trace(o_vcd, o_dport_resp_valid, o_dport_resp_valid.name());
        sc_trace(o_vcd, o_dport_resp_error, o_dport_resp_error.name());
        sc_trace(o_vcd, o_dport_rdata, o_dport_rdata.name());
    }

    proc0->generateVCD(i_vcd, o_vcd);
    cache0->generateVCD(i_vcd, o_vcd);
}

}  // namespace debugger

