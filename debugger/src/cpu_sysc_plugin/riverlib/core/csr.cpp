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

#include "csr.h"
#include "api_core.h"

namespace debugger {

CsrRegs::CsrRegs(sc_module_name name,
                 bool async_reset,
                 uint32_t hartid)
    : sc_module(name),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_sp("i_sp"),
    i_req_valid("i_req_valid"),
    o_req_ready("o_req_ready"),
    i_req_type("i_req_type"),
    i_req_addr("i_req_addr"),
    i_req_data("i_req_data"),
    o_resp_valid("o_resp_valid"),
    i_resp_ready("i_resp_ready"),
    o_resp_data("o_resp_data"),
    o_resp_exception("o_resp_exception"),
    i_e_halted("i_e_halted"),
    i_e_pc("i_e_pc"),
    i_e_instr("i_e_instr"),
    i_irq_pending("i_irq_pending"),
    o_irq_software("o_irq_software"),
    o_irq_timer("o_irq_timer"),
    o_irq_external("o_irq_external"),
    o_stack_overflow("o_stack_overflow"),
    o_stack_underflow("o_stack_underflow"),
    i_e_valid("i_e_valid"),
    o_executed_cnt("o_executed_cnt"),
    o_step("o_step"),
    i_dbg_progbuf_ena("i_dbg_progbuf_ena"),
    o_progbuf_end("o_progbuf_end"),
    o_progbuf_error("o_progbuf_error"),
    o_flushi_ena("o_flushi_ena"),
    o_flushi_addr("o_flushi_addr"),
    o_mpu_region_we("o_mpu_region_we"),
    o_mpu_region_idx("o_mpu_region_idx"),
    o_mpu_region_addr("o_mpu_region_addr"),
    o_mpu_region_mask("o_mpu_region_mask"),
    o_mpu_region_flags("o_mpu_region_flags"),
    o_mmu_ena("o_mmu_ena"),
    o_mmu_ppn("o_mmu_ppn") {

    async_reset_ = async_reset;
    hartid_ = hartid;

    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_sp;
    sensitive << i_req_valid;
    sensitive << i_req_type;
    sensitive << i_req_addr;
    sensitive << i_req_data;
    sensitive << i_resp_ready;
    sensitive << i_e_halted;
    sensitive << i_e_pc;
    sensitive << i_e_instr;
    sensitive << i_irq_pending;
    sensitive << i_e_valid;
    sensitive << i_dbg_progbuf_ena;
    for (int i = 0; i < 4; i++) {
        sensitive << r.xmode[i].xepc;
        sensitive << r.xmode[i].xpp;
        sensitive << r.xmode[i].xpie;
        sensitive << r.xmode[i].xie;
    }
    sensitive << r.state;
    sensitive << r.cmd_type;
    sensitive << r.cmd_addr;
    sensitive << r.cmd_data;
    sensitive << r.cmd_exception;
    sensitive << r.progbuf_end;
    sensitive << r.progbuf_err;
    sensitive << r.mtvec;
    sensitive << r.mtvec_mode;
    sensitive << r.mtval;
    sensitive << r.mscratch;
    sensitive << r.mstackovr;
    sensitive << r.mstackund;
    sensitive << r.mpu_addr;
    sensitive << r.mpu_mask;
    sensitive << r.mpu_idx;
    sensitive << r.mpu_flags;
    sensitive << r.mpu_we;
    sensitive << r.mmu_ena;
    sensitive << r.satp_ppn;
    sensitive << r.satp_mode;
    sensitive << r.mode;
    sensitive << r.mprv;
    sensitive << r.usie;
    sensitive << r.ssie;
    sensitive << r.msie;
    sensitive << r.utie;
    sensitive << r.stie;
    sensitive << r.mtie;
    sensitive << r.ueie;
    sensitive << r.seie;
    sensitive << r.meie;
    sensitive << r.usip;
    sensitive << r.ssip;
    sensitive << r.msip;
    sensitive << r.utip;
    sensitive << r.stip;
    sensitive << r.mtip;
    sensitive << r.ueip;
    sensitive << r.seip;
    sensitive << r.meip;
    sensitive << r.ex_fpu_invalidop;
    sensitive << r.ex_fpu_divbyzero;
    sensitive << r.ex_fpu_overflow;
    sensitive << r.ex_fpu_underflow;
    sensitive << r.ex_fpu_inexact;
    sensitive << r.trap_irq;
    sensitive << r.trap_cause;
    sensitive << r.trap_addr;
    sensitive << r.timer;
    sensitive << r.cycle_cnt;
    sensitive << r.executed_cnt;
    sensitive << r.dscratch0;
    sensitive << r.dscratch1;
    sensitive << r.dpc;
    sensitive << r.halt_cause;
    sensitive << r.dcsr_ebreakm;
    sensitive << r.dcsr_stopcount;
    sensitive << r.dcsr_stoptimer;
    sensitive << r.dcsr_step;
    sensitive << r.dcsr_stepie;
    sensitive << r.stepping_mode_cnt;
    sensitive << r.ins_per_step;
    sensitive << r.flushi_ena;
    sensitive << r.flushi_addr;

    SC_METHOD(registers);
    sensitive << i_nrst;
    sensitive << i_clk.pos();
}

void CsrRegs::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    std::string pn(name());
    if (o_vcd) {
        sc_trace(o_vcd, i_sp, i_sp.name());
        sc_trace(o_vcd, i_req_valid, i_req_valid.name());
        sc_trace(o_vcd, o_req_ready, o_req_ready.name());
        sc_trace(o_vcd, i_req_type, i_req_type.name());
        sc_trace(o_vcd, i_req_addr, i_req_addr.name());
        sc_trace(o_vcd, i_req_data, i_req_data.name());
        sc_trace(o_vcd, o_resp_valid, o_resp_valid.name());
        sc_trace(o_vcd, i_resp_ready, i_resp_ready.name());
        sc_trace(o_vcd, o_resp_data, o_resp_data.name());
        sc_trace(o_vcd, o_resp_exception, o_resp_exception.name());
        sc_trace(o_vcd, i_e_halted, i_e_halted.name());
        sc_trace(o_vcd, i_e_pc, i_e_pc.name());
        sc_trace(o_vcd, i_e_instr, i_e_instr.name());
        sc_trace(o_vcd, i_irq_pending, i_irq_pending.name());
        sc_trace(o_vcd, o_irq_software, o_irq_software.name());
        sc_trace(o_vcd, o_irq_timer, o_irq_timer.name());
        sc_trace(o_vcd, o_irq_external, o_irq_external.name());
        sc_trace(o_vcd, o_stack_overflow, o_stack_overflow.name());
        sc_trace(o_vcd, o_stack_underflow, o_stack_underflow.name());
        sc_trace(o_vcd, i_e_valid, i_e_valid.name());
        sc_trace(o_vcd, o_executed_cnt, o_executed_cnt.name());
        sc_trace(o_vcd, o_step, o_step.name());
        sc_trace(o_vcd, i_dbg_progbuf_ena, i_dbg_progbuf_ena.name());
        sc_trace(o_vcd, o_progbuf_end, o_progbuf_end.name());
        sc_trace(o_vcd, o_progbuf_error, o_progbuf_error.name());
        sc_trace(o_vcd, o_flushi_ena, o_flushi_ena.name());
        sc_trace(o_vcd, o_flushi_addr, o_flushi_addr.name());
        sc_trace(o_vcd, o_mpu_region_we, o_mpu_region_we.name());
        sc_trace(o_vcd, o_mpu_region_idx, o_mpu_region_idx.name());
        sc_trace(o_vcd, o_mpu_region_addr, o_mpu_region_addr.name());
        sc_trace(o_vcd, o_mpu_region_mask, o_mpu_region_mask.name());
        sc_trace(o_vcd, o_mpu_region_flags, o_mpu_region_flags.name());
        sc_trace(o_vcd, o_mmu_ena, o_mmu_ena.name());
        sc_trace(o_vcd, o_mmu_ppn, o_mmu_ppn.name());
        for (int i = 0; i < 4; i++) {
            char tstr[1024];
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_xmode%d_xepc", pn.c_str(), i);
            sc_trace(o_vcd, r.xmode[i].xepc, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_xmode%d_xpp", pn.c_str(), i);
            sc_trace(o_vcd, r.xmode[i].xpp, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_xmode%d_xpie", pn.c_str(), i);
            sc_trace(o_vcd, r.xmode[i].xpie, tstr);
            RISCV_sprintf(tstr, sizeof(tstr), "%s.r_xmode%d_xie", pn.c_str(), i);
            sc_trace(o_vcd, r.xmode[i].xie, tstr);
        }
        sc_trace(o_vcd, r.state, pn + ".r_state");
        sc_trace(o_vcd, r.cmd_type, pn + ".r_cmd_type");
        sc_trace(o_vcd, r.cmd_addr, pn + ".r_cmd_addr");
        sc_trace(o_vcd, r.cmd_data, pn + ".r_cmd_data");
        sc_trace(o_vcd, r.cmd_exception, pn + ".r_cmd_exception");
        sc_trace(o_vcd, r.progbuf_end, pn + ".r_progbuf_end");
        sc_trace(o_vcd, r.progbuf_err, pn + ".r_progbuf_err");
        sc_trace(o_vcd, r.mtvec, pn + ".r_mtvec");
        sc_trace(o_vcd, r.mtvec_mode, pn + ".r_mtvec_mode");
        sc_trace(o_vcd, r.mtval, pn + ".r_mtval");
        sc_trace(o_vcd, r.mscratch, pn + ".r_mscratch");
        sc_trace(o_vcd, r.mstackovr, pn + ".r_mstackovr");
        sc_trace(o_vcd, r.mstackund, pn + ".r_mstackund");
        sc_trace(o_vcd, r.mpu_addr, pn + ".r_mpu_addr");
        sc_trace(o_vcd, r.mpu_mask, pn + ".r_mpu_mask");
        sc_trace(o_vcd, r.mpu_idx, pn + ".r_mpu_idx");
        sc_trace(o_vcd, r.mpu_flags, pn + ".r_mpu_flags");
        sc_trace(o_vcd, r.mpu_we, pn + ".r_mpu_we");
        sc_trace(o_vcd, r.mmu_ena, pn + ".r_mmu_ena");
        sc_trace(o_vcd, r.satp_ppn, pn + ".r_satp_ppn");
        sc_trace(o_vcd, r.satp_mode, pn + ".r_satp_mode");
        sc_trace(o_vcd, r.mode, pn + ".r_mode");
        sc_trace(o_vcd, r.mprv, pn + ".r_mprv");
        sc_trace(o_vcd, r.usie, pn + ".r_usie");
        sc_trace(o_vcd, r.ssie, pn + ".r_ssie");
        sc_trace(o_vcd, r.msie, pn + ".r_msie");
        sc_trace(o_vcd, r.utie, pn + ".r_utie");
        sc_trace(o_vcd, r.stie, pn + ".r_stie");
        sc_trace(o_vcd, r.mtie, pn + ".r_mtie");
        sc_trace(o_vcd, r.ueie, pn + ".r_ueie");
        sc_trace(o_vcd, r.seie, pn + ".r_seie");
        sc_trace(o_vcd, r.meie, pn + ".r_meie");
        sc_trace(o_vcd, r.usip, pn + ".r_usip");
        sc_trace(o_vcd, r.ssip, pn + ".r_ssip");
        sc_trace(o_vcd, r.msip, pn + ".r_msip");
        sc_trace(o_vcd, r.utip, pn + ".r_utip");
        sc_trace(o_vcd, r.stip, pn + ".r_stip");
        sc_trace(o_vcd, r.mtip, pn + ".r_mtip");
        sc_trace(o_vcd, r.ueip, pn + ".r_ueip");
        sc_trace(o_vcd, r.seip, pn + ".r_seip");
        sc_trace(o_vcd, r.meip, pn + ".r_meip");
        sc_trace(o_vcd, r.ex_fpu_invalidop, pn + ".r_ex_fpu_invalidop");
        sc_trace(o_vcd, r.ex_fpu_divbyzero, pn + ".r_ex_fpu_divbyzero");
        sc_trace(o_vcd, r.ex_fpu_overflow, pn + ".r_ex_fpu_overflow");
        sc_trace(o_vcd, r.ex_fpu_underflow, pn + ".r_ex_fpu_underflow");
        sc_trace(o_vcd, r.ex_fpu_inexact, pn + ".r_ex_fpu_inexact");
        sc_trace(o_vcd, r.trap_irq, pn + ".r_trap_irq");
        sc_trace(o_vcd, r.trap_cause, pn + ".r_trap_cause");
        sc_trace(o_vcd, r.trap_addr, pn + ".r_trap_addr");
        sc_trace(o_vcd, r.timer, pn + ".r_timer");
        sc_trace(o_vcd, r.cycle_cnt, pn + ".r_cycle_cnt");
        sc_trace(o_vcd, r.executed_cnt, pn + ".r_executed_cnt");
        sc_trace(o_vcd, r.dscratch0, pn + ".r_dscratch0");
        sc_trace(o_vcd, r.dscratch1, pn + ".r_dscratch1");
        sc_trace(o_vcd, r.dpc, pn + ".r_dpc");
        sc_trace(o_vcd, r.halt_cause, pn + ".r_halt_cause");
        sc_trace(o_vcd, r.dcsr_ebreakm, pn + ".r_dcsr_ebreakm");
        sc_trace(o_vcd, r.dcsr_stopcount, pn + ".r_dcsr_stopcount");
        sc_trace(o_vcd, r.dcsr_stoptimer, pn + ".r_dcsr_stoptimer");
        sc_trace(o_vcd, r.dcsr_step, pn + ".r_dcsr_step");
        sc_trace(o_vcd, r.dcsr_stepie, pn + ".r_dcsr_stepie");
        sc_trace(o_vcd, r.stepping_mode_cnt, pn + ".r_stepping_mode_cnt");
        sc_trace(o_vcd, r.ins_per_step, pn + ".r_ins_per_step");
        sc_trace(o_vcd, r.flushi_ena, pn + ".r_flushi_ena");
        sc_trace(o_vcd, r.flushi_addr, pn + ".r_flushi_addr");
    }

}

void CsrRegs::comb() {
    int iM;
    int iH;
    int iS;
    int iU;
    sc_uint<2> vb_xpp;
    bool v_step_irq;
    bool v_sw_irq;
    bool v_tmr_irq;
    bool v_ext_irq;
    bool w_trap_valid;
    bool v_trap_irq;
    sc_uint<5> wb_trap_cause;
    sc_uint<RISCV_ARCH> vb_mtval;                           // additional exception information
    bool w_mstackovr;
    bool w_mstackund;
    bool v_csr_rena;
    bool v_csr_wena;
    bool v_csr_trapreturn;
    sc_uint<RISCV_ARCH> vb_rdata;
    bool v_req_halt;
    bool v_req_resume;
    bool v_req_progbuf;
    bool v_req_ready;
    bool v_resp_valid;
    sc_uint<RISCV_ARCH> vb_mtvec_off;                       // 4-bytes aligned

    iM = PRV_M;
    iH = PRV_H;
    iS = PRV_S;
    iU = PRV_U;
    vb_xpp = 0;
    v_step_irq = 0;
    v_sw_irq = 0;
    v_tmr_irq = 0;
    v_ext_irq = 0;
    w_trap_valid = 0;
    v_trap_irq = 0;
    wb_trap_cause = 0;
    vb_mtval = 0;
    w_mstackovr = 0;
    w_mstackund = 0;
    v_csr_rena = 0;
    v_csr_wena = 0;
    v_csr_trapreturn = 0;
    vb_rdata = 0;
    v_req_halt = 0;
    v_req_resume = 0;
    v_req_progbuf = 0;
    v_req_ready = 0;
    v_resp_valid = 0;
    vb_mtvec_off = 0;

    for (int i = 0; i < 4; i++) {
        v.xmode[i].xepc = r.xmode[i].xepc;
        v.xmode[i].xpp = r.xmode[i].xpp;
        v.xmode[i].xpie = r.xmode[i].xpie;
        v.xmode[i].xie = r.xmode[i].xie;
    }
    v.state = r.state;
    v.cmd_type = r.cmd_type;
    v.cmd_addr = r.cmd_addr;
    v.cmd_data = r.cmd_data;
    v.cmd_exception = r.cmd_exception;
    v.progbuf_end = r.progbuf_end;
    v.progbuf_err = r.progbuf_err;
    v.mtvec = r.mtvec;
    v.mtvec_mode = r.mtvec_mode;
    v.mtval = r.mtval;
    v.mscratch = r.mscratch;
    v.mstackovr = r.mstackovr;
    v.mstackund = r.mstackund;
    v.mpu_addr = r.mpu_addr;
    v.mpu_mask = r.mpu_mask;
    v.mpu_idx = r.mpu_idx;
    v.mpu_flags = r.mpu_flags;
    v.mpu_we = r.mpu_we;
    v.mmu_ena = r.mmu_ena;
    v.satp_ppn = r.satp_ppn;
    v.satp_mode = r.satp_mode;
    v.mode = r.mode;
    v.mprv = r.mprv;
    v.usie = r.usie;
    v.ssie = r.ssie;
    v.msie = r.msie;
    v.utie = r.utie;
    v.stie = r.stie;
    v.mtie = r.mtie;
    v.ueie = r.ueie;
    v.seie = r.seie;
    v.meie = r.meie;
    v.usip = r.usip;
    v.ssip = r.ssip;
    v.msip = r.msip;
    v.utip = r.utip;
    v.stip = r.stip;
    v.mtip = r.mtip;
    v.ueip = r.ueip;
    v.seip = r.seip;
    v.meip = r.meip;
    v.ex_fpu_invalidop = r.ex_fpu_invalidop;
    v.ex_fpu_divbyzero = r.ex_fpu_divbyzero;
    v.ex_fpu_overflow = r.ex_fpu_overflow;
    v.ex_fpu_underflow = r.ex_fpu_underflow;
    v.ex_fpu_inexact = r.ex_fpu_inexact;
    v.trap_irq = r.trap_irq;
    v.trap_cause = r.trap_cause;
    v.trap_addr = r.trap_addr;
    v.timer = r.timer;
    v.cycle_cnt = r.cycle_cnt;
    v.executed_cnt = r.executed_cnt;
    v.dscratch0 = r.dscratch0;
    v.dscratch1 = r.dscratch1;
    v.dpc = r.dpc;
    v.halt_cause = r.halt_cause;
    v.dcsr_ebreakm = r.dcsr_ebreakm;
    v.dcsr_stopcount = r.dcsr_stopcount;
    v.dcsr_stoptimer = r.dcsr_stoptimer;
    v.dcsr_step = r.dcsr_step;
    v.dcsr_stepie = r.dcsr_stepie;
    v.stepping_mode_cnt = r.stepping_mode_cnt;
    v.ins_per_step = r.ins_per_step;
    v.flushi_ena = r.flushi_ena;
    v.flushi_addr = r.flushi_addr;

    v.mpu_we = 0;
    vb_mtvec_off = (r.mtvec.read()((RISCV_ARCH - 1), 2) << 2);
    vb_xpp = r.xmode[r.mode.read().to_int()].xpp;

    switch (r.state.read()) {
    case State_Idle:
        v.progbuf_end = 0;
        v.progbuf_err = 0;
        v_req_ready = 1;
        if (i_req_valid.read() == 1) {
            v.cmd_type = i_req_type;
            v.cmd_addr = i_req_addr;
            v.cmd_data = i_req_data;
            v.cmd_exception = 0;
            if (i_req_type.read()[CsrReq_ExceptionBit] == 1) {
                v.state = State_Exception;
            } else if (i_req_type.read()[CsrReq_BreakpointBit] == 1) {
                v.state = State_Breakpoint;
            } else if (i_req_type.read()[CsrReq_HaltBit] == 1) {
                v.state = State_Halt;
            } else if (i_req_type.read()[CsrReq_ResumeBit] == 1) {
                v.state = State_Resume;
            } else if (i_req_type.read()[CsrReq_InterruptBit] == 1) {
                v.state = State_Interrupt;
            } else if (i_req_type.read()[CsrReq_TrapReturnBit] == 1) {
                v.state = State_TrapReturn;
            } else if (i_req_type.read()[CsrReq_WfiBit] == 1) {
                v.state = State_Wfi;
            } else {
                v.state = State_RW;
            }
        }
        break;
    case State_Exception:
        v.state = State_Response;
        w_trap_valid = 1;
        vb_mtval = r.cmd_data;
        wb_trap_cause = r.cmd_addr.read()(4, 0);
        v.cmd_data = vb_mtvec_off;
        if (i_dbg_progbuf_ena.read() == 1) {
            v.progbuf_err = 1;
            v.progbuf_end = 1;
            v.cmd_exception = 1;
        }
        if (r.cmd_addr.read() == EXCEPTION_CallFromUmode) {
            wb_trap_cause = (r.cmd_addr.read() + r.mode.read());
        }
        break;
    case State_Breakpoint:                                  // software breakpoint
        v.state = State_Response;
        if (i_dbg_progbuf_ena.read() == 1) {
            // do not modify halt cause in debug mode
            v.progbuf_end = 1;
            v.cmd_data = ~0ull;                             // signal to executor to switch into Debug Mode and halt
        } else if (r.dcsr_ebreakm.read() == 1) {
            v.halt_cause = HALT_CAUSE_EBREAK;
            v.dpc = r.cmd_data;
            v.cmd_data = ~0ull;                             // signal to executor to switch into Debug Mode and halt
        } else {
            w_trap_valid = 1;
            wb_trap_cause = r.cmd_addr.read()(4, 0);
            vb_mtval = i_e_pc;
            v.cmd_data = vb_mtvec_off;                      // Jump to exception handler
        }
        break;
    case State_Halt:
        v.state = State_Response;
        v.halt_cause = r.cmd_addr.read()(2, 0);             // Halt Request or Step done
        v.dpc = i_e_pc;
        break;
    case State_Resume:
        v.state = State_Response;
        if (i_dbg_progbuf_ena.read() == 1) {
            v.cmd_data = 0;
        } else {
            v.cmd_data = r.dpc;
        }
        break;
    case State_Interrupt:
        v.state = State_Response;
        w_trap_valid = 1;
        wb_trap_cause = ((4 * r.cmd_addr.read()) + PRV_M);
        v_trap_irq = 1;
        if (r.mtvec_mode.read() == 1) {
            // vectorized
            v.cmd_data = (vb_mtvec_off + (4 * wb_trap_cause));
        } else {
            v.cmd_data = vb_mtvec_off;
        }
        break;
    case State_TrapReturn:
        v.state = State_Response;
        v_csr_trapreturn = 1;
        v.cmd_data = r.xmode[r.mode.read().to_int()].xepc;
        break;
    case State_RW:
        v.state = State_Response;
        // csr[9:8] encode the loweset priviledge level that can access to CSR
        // csr[11:10] register is read/write (00, 01 or 10) or read-only (11)
        if (r.mode.read() < r.cmd_addr.read()(9, 8)) {
            // Not enough priv to access this register
            v.cmd_exception = 1;
        } else {
            v_csr_rena = r.cmd_type.read()[CsrReq_ReadBit];
            v_csr_wena = r.cmd_type.read()[CsrReq_WriteBit];
        }
        // All operation into CSR implemented through the Read-Modify-Write
        // and we cannot generate exception on write access into read-only regs
        // So do not check bits csr[11:10], otherwise always will be the exception.
        break;
    case State_Wfi:
        v.state = State_Response;
        v.cmd_data = 0;                                     // no error, valid for all mdoes
        break;
    case State_Response:
        v_resp_valid = 1;
        if (i_resp_ready.read() == 1) {
            v.state = State_Idle;
        }
        break;
    default:
        break;
    }

    // CSR registers. Priviledge Arch. V20211203, page 9(19):
    //     CSR[11:10] indicate whether the register is read/write (00, 01, or 10) or read-only (11)
    //     CSR[9:8] encode the lowest privilege level that can access the CSR
    switch (r.cmd_addr.read()) {
    case 0x000:                                             // ustatus: [URW] User status register
        break;
    case 0x004:                                             // uie: [URW] User interrupt-enable register
        break;
    case 0x005:                                             // ustatus: [URW] User trap handler base address
        break;
    case 0x040:                                             // uscratch: [URW] Scratch register for user trap handlers
        break;
    case 0x041:                                             // uepc: [URW] User exception program counter
        vb_rdata = r.xmode[iU].xepc;
        if (v_csr_wena) {
            v.xmode[iU].xepc = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    case 0x042:                                             // ucause: [URW] User trap cause
        break;
    case 0x043:                                             // utval: [URW] User bad address or instruction
        break;
    case 0x044:                                             // uip: [URW] User interrupt pending
        break;
    case 0x001:                                             // fflags: [URW] Floating-Point Accrued Exceptions
        vb_rdata[0] = r.ex_fpu_inexact;
        vb_rdata[1] = r.ex_fpu_underflow;
        vb_rdata[2] = r.ex_fpu_overflow;
        vb_rdata[3] = r.ex_fpu_divbyzero;
        vb_rdata[4] = r.ex_fpu_invalidop;
        break;
    case 0x002:                                             // fflags: [URW] Floating-Point Dynamic Rounding Mode
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata(2, 0) = 4;                             // Round mode: round to Nearest (RMM)
        }
        break;
    case 0x003:                                             // fcsr: [URW] Floating-Point Control and Status Register (frm + fflags)
        vb_rdata[0] = r.ex_fpu_inexact;
        vb_rdata[1] = r.ex_fpu_underflow;
        vb_rdata[2] = r.ex_fpu_overflow;
        vb_rdata[3] = r.ex_fpu_divbyzero;
        vb_rdata[4] = r.ex_fpu_invalidop;
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata(7, 5) = 4;                             // Round mode: round to Nearest (RMM)
        }
        break;
    case 0xC00:                                             // cycle: [URO] User Cycle counter for RDCYCLE pseudo-instruction
        vb_rdata = r.cycle_cnt;
        break;
    case 0xC01:                                             // time: [URO] User Timer for RDTIME pseudo-instruction
        vb_rdata = r.timer;
        break;
    case 0xC03:                                             // insret: [URO] User Instructions-retired counter for RDINSTRET pseudo-instruction
        vb_rdata = r.executed_cnt;
        break;
    case 0x100:                                             // sstatus: [SRW] Supervisor status register
        break;
    case 0x104:                                             // sie: [SRW] Supervisor interrupt-enable register
        break;
    case 0x105:                                             // sstatus: [SRW] Supervisor trap handler base address
        break;
    case 0x106:                                             // scounteren: [SRW] Supervisor counter enable
        break;
    case 0x10A:                                             // senvcfg: [SRW] Supervisor environment configuration register
        break;
    case 0x140:                                             // sscratch: [SRW] Supervisor register for supervisor trap handlers
        break;
    case 0x141:                                             // sepc: [SRW] Supervisor exception program counter
        vb_rdata = r.xmode[iS].xepc;
        if (v_csr_wena) {
            v.xmode[iS].xepc = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    case 0x142:                                             // scause: [SRW] Supervisor trap cause
        break;
    case 0x143:                                             // stval: [SRW] Supervisor bad address or instruction
        break;
    case 0x144:                                             // sip: [SRW] Supervisor interrupt pending
        break;
    case 0x180:                                             // satp: [SRW] Supervisor address translation and protection
        // Writing unssoprted MODE[63:60], entire write has no effect
        //     MODE = 0 Bare. No translation or protection
        //     MODE = 9 Sv48. Page based 48-bit virtual addressing
        vb_rdata(43, 0) = r.satp_ppn;
        vb_rdata(63, 60) = r.satp_mode;
        if ((v_csr_wena == 1)
                && ((r.cmd_data.read()(63, 60).or_reduce() == 0)
                        || (r.cmd_data.read()(63, 60).to_uint() == SATP_MODE_SV48))) {
            v.satp_ppn = r.cmd_data.read()(43, 0);
            v.satp_mode = r.cmd_data.read()(63, 60);
        }
        break;
    case 0x5A8:                                             // scontext: [SRW] Supervisor-mode context register
        break;
    case 0xF11:                                             // mvendorid: [MRO] Vendor ID
        vb_rdata = CFG_VENDOR_ID;
        break;
    case 0xF12:                                             // marchid: [MRO] Architecture ID
        break;
    case 0xF13:                                             // mimplementationid: [MRO] Implementation ID
        vb_rdata = CFG_IMPLEMENTATION_ID;
        break;
    case 0xF14:                                             // mhartid: [MRO] Hardware thread ID
        vb_rdata(63, 0) = hartid_;
        break;
    case 0xF15:                                             // mconfigptr: [MRO] Pointer to configuration data structure
        break;
    case 0x300:                                             // mstatus: [MRW] Machine mode status register
        // [0] WPRI
        vb_rdata[1] = r.xmode[iS].xie;
        // [2] WPRI
        vb_rdata[3] = r.xmode[iM].xie;
        // [4] WPRI
        vb_rdata[5] = r.xmode[iS].xpie;
        // [6] UBE
        vb_rdata[7] = r.xmode[iM].xpie;
        vb_rdata[8] = r.xmode[iS].xpp.read()[0];            // SPP can have onle 0 or 1 values, so 1 bit only
        // [10:9] VS
        vb_rdata(12, 11) = r.xmode[iM].xpp;
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata(14, 13) = 0x1;                         // FS field: Initial state
        }
        // [16:15] XS
        vb_rdata[17] = r.mprv;
        // [18] SUM
        // [19] MXR
        // [20] TVM
        // [21] TW
        // [22] TSR
        // [31:23] WPRI
        vb_rdata(33, 32) = 0x2;                             // UXL: User is 64-bits
        vb_rdata(35, 34) = 0x2;                             // SXL: Supervisor is 64-bits
        // [36] SBE
        // [37] MBE
        // [62:38] WPRI
        // [63] SD. Read-only bit that summize FS, VS or XS fields
        if (v_csr_wena == 1) {
            v.xmode[iS].xie = r.cmd_data.read()[1];
            v.xmode[iM].xie = r.cmd_data.read()[3];
            v.xmode[iS].xpie = r.cmd_data.read()[5];
            v.xmode[iM].xpie = r.cmd_data.read()[7];
            v.xmode[iS].xpp = (0, r.cmd_data.read()[8]);
            v.xmode[iM].xpp = r.cmd_data.read()(12, 11);
            v.mprv = r.cmd_data.read()[17];
        }
        break;
    case 0x301:                                             // misa: [MRW] ISA and extensions
        // Base[XLEN-1:XLEN-2]
        //      1 = 32
        //      2 = 64
        //      3 = 128

        vb_rdata((RISCV_ARCH - 1), (RISCV_ARCH - 2)) = 2;
        // BitCharacterDescription
        //      0  A Atomic extension
        //      1  B Tentatively reserved for Bit operations extension
        //      2  C Compressed extension
        //      3  D Double-precision Foating-point extension
        //      4  E RV32E base ISA (embedded)
        //      5  F Single-precision Foating-point extension
        //      6  G Additional standard extensions present
        //      7  H Hypervisor mode implemented
        //      8  I RV32I/64I/128I base ISA
        //      9  J Reserved
        //      10 K Reserved
        //      11 L Tentatively reserved for Decimal Floating-Point extension
        //      12 M Integer Multiply/Divide extension
        //      13 N User-level interrupts supported
        //      14 O Reserved
        //      15 P Tentatively reserved for Packed-SIMD extension
        //      16 Q Quad-precision Foating-point extension
        //      17 R Reserved
        //      18 S Supervisor mode implemented
        //      19 T Tentatively reserved for Transactional Memory extension
        //      20 U User mode implemented
        //      21 V Tentatively reserved for Vector extension
        //      22 W Reserved
        //      23 X Non-standard extensions present
        //      24 Y Reserved
        //      25 Z Reserve
        vb_rdata[0] = 1;                                    // A-extension
        vb_rdata[8] = 1;                                    // I-extension
        vb_rdata[12] = 1;                                   // M-extension
        vb_rdata[18] = 1;                                   // S-extension
        vb_rdata[20] = 1;                                   // U-extension
        vb_rdata[2] = 1;                                    // C-extension
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata[3] = 1;                                // D-extension
        }
        break;
    case 0x302:                                             // medeleg: [MRW] Machine exception delegation
        break;
    case 0x303:                                             // mideleg: [MRW] Machine interrupt delegation
        break;
    case 0x304:                                             // mie: [MRW] Machine interrupt enable bit
        vb_rdata[0] = r.usie;                               // user software interrupt
        vb_rdata[1] = r.ssie;                               // super-user software interrupt
        vb_rdata[3] = r.msie;                               // machine software interrupt
        vb_rdata[4] = r.utie;                               // user timer interrupt
        vb_rdata[5] = r.stie;                               // super-user timer interrupt
        vb_rdata[7] = r.mtie;                               // machine timer interrupt
        vb_rdata[8] = r.ueie;                               // user external interrupt
        vb_rdata[9] = r.seie;                               // super-user external interrupt
        vb_rdata[11] = r.meie;                              // machine external interrupt
        if (v_csr_wena) {
            v.usie = r.cmd_data.read()[0];
            v.ssie = r.cmd_data.read()[1];
            v.msie = r.cmd_data.read()[3];
            v.utie = r.cmd_data.read()[4];
            v.stie = r.cmd_data.read()[5];
            v.mtie = r.cmd_data.read()[7];
            v.ueie = r.cmd_data.read()[8];
            v.seie = r.cmd_data.read()[9];
            v.meie = r.cmd_data.read()[11];
        }
        break;
    case 0x305:                                             // mtvec: [MRW] Machine trap-handler base address
        vb_rdata = r.mtvec;
        vb_rdata(1, 0) = r.mtvec_mode;
        if (v_csr_wena == 1) {
            v.mtvec = (r.cmd_data.read()((RISCV_ARCH - 1), 2) << 2);
            v.mtvec_mode = r.cmd_data.read()(1, 0);
        }
        break;
    case 0x306:                                             // mcounteren: [MRW] Machine counter enable
        break;
    case 0x340:                                             // mscratch: [MRW] Machine scratch register
        vb_rdata = r.mscratch;
        if (v_csr_wena) {
            v.mscratch = r.cmd_data;
        }
        break;
    case 0x341:                                             // mepc: [MRW] Machine program counter
        vb_rdata = r.xmode[iM].xepc;
        if (v_csr_wena) {
            v.xmode[iM].xepc = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    case 0x342:                                             // mcause: [MRW] Machine trap cause
        vb_rdata = 0;
        vb_rdata[63] = r.trap_irq;
        vb_rdata(4, 0) = r.trap_cause;
        break;
    case 0x343:                                             // mtval: [MRW] Machine bad address or instruction
        vb_rdata = r.mtval;
        if (v_csr_wena) {
            v.mtval = r.cmd_data;
        }
        break;
    case 0x344:                                             // mip: [MRW] Machine interrupt pending
        vb_rdata[0] = r.usip;                               // user software pending bit
        vb_rdata[1] = r.ssip;                               // super-user software pending bit
        vb_rdata[3] = r.msip;                               // machine software pending bit
        vb_rdata[4] = r.utip;                               // user timer pending bit
        vb_rdata[5] = r.stip;                               // super-user timer pending bit
        vb_rdata[7] = r.mtip;                               // RO: machine timer pending bit
        vb_rdata[8] = r.ueip;                               // user external pending bit
        vb_rdata[9] = r.seip;                               // super-user external pending bit
        vb_rdata[11] = r.meip;                              // RO: machine external pending bit (cleared by writing into mtimecmp)
        break;
    case 0x34A:                                             // mtinst: [MRW] Machine trap instruction (transformed)
        break;
    case 0x34B:                                             // mtval2: [MRW] Machine bad guest physical register
        break;
    case 0x30A:                                             // menvcfg: [MRW] Machine environment configuration register
        break;
    case 0x747:                                             // mseccfg: [MRW] Machine security configuration register
        break;
    case 0x3A0:                                             // pmpcfg0: [MRW] Physical memory protection configuration
        break;
    case 0x3A2:                                             // pmpcfg2: [MRW] Physical memory protection configuration
        break;
    case 0x3AE:                                             // pmpcfg14: [MRW] Physical memory protection configuration
        break;
    case 0x3B0:                                             // pmpaddr0: [MRW] Physical memory protection address register
        break;
    case 0x3B1:                                             // pmpaddr1: [MRW] Physical memory protection address register
        break;
    case 0x3EF:                                             // pmpaddr63: [MRW] Physical memory protection address register
        break;
    case 0xB00:                                             // mcycle: [MRW] Machine cycle counter
        vb_rdata = r.cycle_cnt;
        break;
    case 0xB02:                                             // minstret: [MRW] Machine instructions-retired counter
        vb_rdata = r.executed_cnt;
        break;
    case 0x320:                                             // mcounterinhibit: [MRW] Machine counter-inhibit register
        break;
    case 0x323:                                             // mpevent3: [MRW] Machine performance-monitoring event selector
        break;
    case 0x324:                                             // mpevent4: [MRW] Machine performance-monitoring event selector
        break;
    case 0x33F:                                             // mpevent31: [MRW] Machine performance-monitoring event selector
        break;
    case 0x7A0:                                             // tselect: [MRW] Debug/Trace trigger register select
        break;
    case 0x7A1:                                             // tdata1: [MRW] First Debug/Trace trigger data register
        break;
    case 0x7A2:                                             // tdata2: [MRW] Second Debug/Trace trigger data register
        break;
    case 0x7A3:                                             // tdata3: [MRW] Third Debug/Trace trigger data register
        break;
    case 0x7A8:                                             // mcontext: [MRW] Machine-mode context register
        break;
    case 0x7B0:                                             // dcsr: [DRW] Debug control and status register
        vb_rdata(31, 28) = 4;                               // xdebugver: 4=External debug supported
        vb_rdata[15] = r.dcsr_ebreakm;
        vb_rdata[11] = r.dcsr_stepie;                       // interrupt dis/ena during step
        vb_rdata[10] = r.dcsr_stopcount;                    // don't increment any counter
        vb_rdata[9] = r.dcsr_stoptimer;                     // don't increment timer
        vb_rdata(8, 6) = r.halt_cause;
        vb_rdata[2] = r.dcsr_step;
        vb_rdata(1, 0) = 3;                                 // prv: privilege in debug mode: 3=machine
        if (v_csr_wena == 1) {
            v.dcsr_ebreakm = r.cmd_data.read()[15];
            v.dcsr_stepie = r.cmd_data.read()[11];
            v.dcsr_stopcount = r.cmd_data.read()[10];
            v.dcsr_stoptimer = r.cmd_data.read()[9];
            v.dcsr_step = r.cmd_data.read()[2];
        }
        break;
    case 0x7B1:                                             // dpc: [DRW] Debug PC
        // Upon entry into debug mode DPC must contains:
        //        cause        |   Address
        // --------------------|----------------
        //  ebreak             |  Address of ebreak instruction
        //  single step        |  Address of next instruction to be executed
        //  trigger (HW BREAK) |  if timing=0, cause isntruction, if timing=1 enxt instruction
        //  halt request       |  next instruction

        if (i_e_halted.read() == 1) {
            vb_rdata = r.dpc;
        } else {
            // make visible current pc for the debugger even in running state
            vb_rdata = i_e_pc;
        }
        if (v_csr_wena == 1) {
            v.dpc = r.cmd_data;
        }
        break;
    case 0x7B2:                                             // dscratch0: [DRW] Debug scratch register 0
        vb_rdata = r.dscratch0;
        if (v_csr_wena == 1) {
            v.dscratch0 = r.cmd_data;
        }
        break;
    case 0x7B3:                                             // dscratch1: [DRW] Debug scratch register 1
        vb_rdata = r.dscratch1;
        if (v_csr_wena) {
            v.dscratch1 = r.cmd_data;
        }
        break;
    case 0xBC0:                                             // mstackovr: [MRW] Machine Stack Overflow
        vb_rdata = r.mstackovr;
        if (v_csr_wena == 1) {
            v.mstackovr = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    case 0xBC1:                                             // mstackund: [MRW] Machine Stack Underflow
        vb_rdata = r.mstackund;
        if (v_csr_wena == 1) {
            v.mstackund = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    case 0xBC2:                                             // mpu_addr: [MWO] MPU address
        if (v_csr_wena == 1) {
            v.mpu_addr = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    case 0xBC3:                                             // mpu_mask: [MWO] MPU mask
        if (v_csr_wena == 1) {
            v.mpu_mask = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    case 0xBC4:                                             // mpu_ctrl: [MRW] MPU flags and write ena
        vb_rdata = (CFG_MPU_TBL_SIZE << 8);
        if (v_csr_wena == 1) {
            v.mpu_idx = r.cmd_data.read()((8 + (CFG_MPU_TBL_WIDTH - 1)), 8);
            v.mpu_flags = r.cmd_data.read()((CFG_MPU_FL_TOTAL - 1), 0);
            v.mpu_we = r.cmd_data.read()[7];
        }
        break;
    case 0x800:                                             // flushi: [UWO]
        if (v_csr_wena == 1) {
            v.flushi_ena = 1;
            v.flushi_addr = r.cmd_data.read()((CFG_CPU_ADDR_BITS - 1), 0);
        }
        break;
    default:
        // Not implemented CSR:
        if (r.state.read() == State_RW) {
            v.cmd_exception = 1;
        }
        break;
    }

    if (v_csr_rena == 1) {
        v.cmd_data = vb_rdata;
    }


    if (v_csr_trapreturn == 1) {
        if (r.mode.read() == r.cmd_addr.read()) {
            v.mode = vb_xpp;
            v.xmode[r.mode.read().to_int()].xpie = 1;       // xPIE is set to 1 always, page 21
            v.xmode[r.mode.read().to_int()].xpp = PRV_U;    // Set to least-privildged supported mode (U), page 21
        } else {
            v.cmd_exception = 1;                            // xret not matched to current mode
        }
        if (r.xmode[r.mode.read().to_int()].xpp.read() != PRV_M) {// see page 21
            v.mprv = 0;
        }

        // Check MMU:
        if (vb_xpp[1] == 1) {
            // H and M modes
            v.mmu_ena = 0;
        } else {
            // S and U modes
            if (r.satp_mode.read() == SATP_MODE_SV48) {     // Only SV48 implemented
                v.mmu_ena = 1;
            }
        }
    }

    if (w_trap_valid == 1) {
        // By default all excpetions and interrupts handled in M-mode (not delegations)
        v.xmode[iM].xpp = r.mode;
        v.xmode[iM].xpie = r.xmode[r.mode.read().to_int()].xie;
        v.xmode[iM].xie = 0;
        v.xmode[iM].xepc = i_e_pc;                          // current latched instruction not executed overwritten by exception/interrupt
        v.mtval = vb_mtval;                                 // additional information for hwbreakpoint, memaccess faults and illegal opcodes
        v.trap_cause = wb_trap_cause;
        v.trap_irq = v_trap_irq;
        v.mode = PRV_M;
        v.mmu_ena = 0;
    }

    // Interrupt enabled during stepping
    v_step_irq = ((!r.dcsr_step) || r.dcsr_stepie);
    v.msip = i_irq_pending.read()[IRQ_HART_MSIP];
    v_sw_irq = (r.msip && r.msie && r.xmode[iM].xie && v_step_irq);

    v.mtip = i_irq_pending.read()[IRQ_HART_MTIP];
    v_tmr_irq = (r.mtip && r.mtie && r.xmode[iM].xie && v_step_irq);

    v.meip = i_irq_pending.read()[IRQ_HART_MEIP];
    v.seip = i_irq_pending.read()[IRQ_HART_SEIP];
    v_ext_irq = (r.meip && r.meie && r.xmode[iM].xie && v_step_irq);

    w_mstackovr = 0;
    if ((r.mstackovr.read().or_reduce() == 1) && (i_sp.read()((CFG_CPU_ADDR_BITS - 1), 0) < r.mstackovr.read())) {
        w_mstackovr = 1;
        v.mstackovr = 0;
    }
    w_mstackund = 0;
    if ((r.mstackund.read().or_reduce() == 1) && (i_sp.read()((CFG_CPU_ADDR_BITS - 1), 0) > r.mstackund.read())) {
        w_mstackund = 1;
        v.mstackund = 0;
    }

    // if (i_fpu_valid.read()) {
    //     v.ex_fpu_invalidop = i_ex_fpu_invalidop.read();
    //     v.ex_fpu_divbyzero = i_ex_fpu_divbyzero.read();
    //     v.ex_fpu_overflow = i_ex_fpu_overflow.read();
    //     v.ex_fpu_underflow = i_ex_fpu_underflow.read();
    //     v.ex_fpu_inexact = i_ex_fpu_inexact.read();
    // }

    if ((i_e_halted.read() == 0) || (r.dcsr_stopcount.read() == 0)) {
        v.cycle_cnt = (r.cycle_cnt.read() + 1);
    }
    if (((i_e_valid && (!r.dcsr_stopcount)) == 1)
            || ((i_e_valid && (!(i_dbg_progbuf_ena && r.dcsr_stopcount))) == 1)) {
        v.executed_cnt = (r.executed_cnt.read() + 1);
    }
    if (((i_e_halted || i_dbg_progbuf_ena) && r.dcsr_stoptimer) == 0) {
        v.timer = (r.timer.read() + 1);
    }

    if (!async_reset_ && i_nrst.read() == 0) {
        for (int i = 0; i < 4; i++) {
            v.xmode[i].xepc = 0ull;
            v.xmode[i].xpp = 0;
            v.xmode[i].xpie = 0;
            v.xmode[i].xie = 0;
        }
        v.state = State_Idle;
        v.cmd_type = 0;
        v.cmd_addr = 0;
        v.cmd_data = 0ull;
        v.cmd_exception = 0;
        v.progbuf_end = 0;
        v.progbuf_err = 0;
        v.mtvec = 0ull;
        v.mtvec_mode = 0;
        v.mtval = 0ull;
        v.mscratch = 0ull;
        v.mstackovr = 0ull;
        v.mstackund = 0ull;
        v.mpu_addr = 0ull;
        v.mpu_mask = 0ull;
        v.mpu_idx = 0;
        v.mpu_flags = 0;
        v.mpu_we = 0;
        v.mmu_ena = 0;
        v.satp_ppn = 0ull;
        v.satp_mode = 0;
        v.mode = PRV_M;
        v.mprv = 0;
        v.usie = 0;
        v.ssie = 0;
        v.msie = 0;
        v.utie = 0;
        v.stie = 0;
        v.mtie = 0;
        v.ueie = 0;
        v.seie = 0;
        v.meie = 0;
        v.usip = 0;
        v.ssip = 0;
        v.msip = 0;
        v.utip = 0;
        v.stip = 0;
        v.mtip = 0;
        v.ueip = 0;
        v.seip = 0;
        v.meip = 0;
        v.ex_fpu_invalidop = 0;
        v.ex_fpu_divbyzero = 0;
        v.ex_fpu_overflow = 0;
        v.ex_fpu_underflow = 0;
        v.ex_fpu_inexact = 0;
        v.trap_irq = 0;
        v.trap_cause = 0;
        v.trap_addr = 0ull;
        v.timer = 0ull;
        v.cycle_cnt = 0ull;
        v.executed_cnt = 0ull;
        v.dscratch0 = 0ull;
        v.dscratch1 = 0ull;
        v.dpc = CFG_RESET_VECTOR;
        v.halt_cause = 0;
        v.dcsr_ebreakm = 0;
        v.dcsr_stopcount = 0;
        v.dcsr_stoptimer = 0;
        v.dcsr_step = 0;
        v.dcsr_stepie = 0;
        v.stepping_mode_cnt = 0ull;
        v.ins_per_step = 1ull;
        v.flushi_ena = 0;
        v.flushi_addr = 0ull;
    }

    o_req_ready = v_req_ready;
    o_resp_valid = v_resp_valid;
    o_resp_data = r.cmd_data;
    o_resp_exception = r.cmd_exception;
    o_progbuf_end = (r.progbuf_end && i_resp_ready);
    o_progbuf_error = (r.progbuf_err && i_resp_ready);
    o_irq_software = v_sw_irq;
    o_irq_timer = v_tmr_irq;
    o_irq_external = v_ext_irq;
    o_stack_overflow = w_mstackovr;
    o_stack_underflow = w_mstackund;
    o_executed_cnt = r.executed_cnt;
    o_mpu_region_we = r.mpu_we;
    o_mpu_region_idx = r.mpu_idx;
    o_mpu_region_addr = r.mpu_addr;
    o_mpu_region_mask = r.mpu_mask;
    o_mpu_region_flags = r.mpu_flags;
    o_mmu_ena = r.mmu_ena;
    o_mmu_ppn = r.satp_ppn;
    o_step = r.dcsr_step;
    o_flushi_ena = r.flushi_ena;
    o_flushi_addr = r.flushi_addr;
}

void CsrRegs::registers() {
    if (async_reset_ && i_nrst.read() == 0) {
        for (int i = 0; i < 4; i++) {
            r.xmode[i].xepc = 0ull;
            r.xmode[i].xpp = 0;
            r.xmode[i].xpie = 0;
            r.xmode[i].xie = 0;
        }
        r.state = State_Idle;
        r.cmd_type = 0;
        r.cmd_addr = 0;
        r.cmd_data = 0ull;
        r.cmd_exception = 0;
        r.progbuf_end = 0;
        r.progbuf_err = 0;
        r.mtvec = 0ull;
        r.mtvec_mode = 0;
        r.mtval = 0ull;
        r.mscratch = 0ull;
        r.mstackovr = 0ull;
        r.mstackund = 0ull;
        r.mpu_addr = 0ull;
        r.mpu_mask = 0ull;
        r.mpu_idx = 0;
        r.mpu_flags = 0;
        r.mpu_we = 0;
        r.mmu_ena = 0;
        r.satp_ppn = 0ull;
        r.satp_mode = 0;
        r.mode = PRV_M;
        r.mprv = 0;
        r.usie = 0;
        r.ssie = 0;
        r.msie = 0;
        r.utie = 0;
        r.stie = 0;
        r.mtie = 0;
        r.ueie = 0;
        r.seie = 0;
        r.meie = 0;
        r.usip = 0;
        r.ssip = 0;
        r.msip = 0;
        r.utip = 0;
        r.stip = 0;
        r.mtip = 0;
        r.ueip = 0;
        r.seip = 0;
        r.meip = 0;
        r.ex_fpu_invalidop = 0;
        r.ex_fpu_divbyzero = 0;
        r.ex_fpu_overflow = 0;
        r.ex_fpu_underflow = 0;
        r.ex_fpu_inexact = 0;
        r.trap_irq = 0;
        r.trap_cause = 0;
        r.trap_addr = 0ull;
        r.timer = 0ull;
        r.cycle_cnt = 0ull;
        r.executed_cnt = 0ull;
        r.dscratch0 = 0ull;
        r.dscratch1 = 0ull;
        r.dpc = CFG_RESET_VECTOR;
        r.halt_cause = 0;
        r.dcsr_ebreakm = 0;
        r.dcsr_stopcount = 0;
        r.dcsr_stoptimer = 0;
        r.dcsr_step = 0;
        r.dcsr_stepie = 0;
        r.stepping_mode_cnt = 0ull;
        r.ins_per_step = 1ull;
        r.flushi_ena = 0;
        r.flushi_addr = 0ull;
    } else {
        for (int i = 0; i < 4; i++) {
            r.xmode[i].xepc = v.xmode[i].xepc;
            r.xmode[i].xpp = v.xmode[i].xpp;
            r.xmode[i].xpie = v.xmode[i].xpie;
            r.xmode[i].xie = v.xmode[i].xie;
        }
        r.state = v.state;
        r.cmd_type = v.cmd_type;
        r.cmd_addr = v.cmd_addr;
        r.cmd_data = v.cmd_data;
        r.cmd_exception = v.cmd_exception;
        r.progbuf_end = v.progbuf_end;
        r.progbuf_err = v.progbuf_err;
        r.mtvec = v.mtvec;
        r.mtvec_mode = v.mtvec_mode;
        r.mtval = v.mtval;
        r.mscratch = v.mscratch;
        r.mstackovr = v.mstackovr;
        r.mstackund = v.mstackund;
        r.mpu_addr = v.mpu_addr;
        r.mpu_mask = v.mpu_mask;
        r.mpu_idx = v.mpu_idx;
        r.mpu_flags = v.mpu_flags;
        r.mpu_we = v.mpu_we;
        r.mmu_ena = v.mmu_ena;
        r.satp_ppn = v.satp_ppn;
        r.satp_mode = v.satp_mode;
        r.mode = v.mode;
        r.mprv = v.mprv;
        r.usie = v.usie;
        r.ssie = v.ssie;
        r.msie = v.msie;
        r.utie = v.utie;
        r.stie = v.stie;
        r.mtie = v.mtie;
        r.ueie = v.ueie;
        r.seie = v.seie;
        r.meie = v.meie;
        r.usip = v.usip;
        r.ssip = v.ssip;
        r.msip = v.msip;
        r.utip = v.utip;
        r.stip = v.stip;
        r.mtip = v.mtip;
        r.ueip = v.ueip;
        r.seip = v.seip;
        r.meip = v.meip;
        r.ex_fpu_invalidop = v.ex_fpu_invalidop;
        r.ex_fpu_divbyzero = v.ex_fpu_divbyzero;
        r.ex_fpu_overflow = v.ex_fpu_overflow;
        r.ex_fpu_underflow = v.ex_fpu_underflow;
        r.ex_fpu_inexact = v.ex_fpu_inexact;
        r.trap_irq = v.trap_irq;
        r.trap_cause = v.trap_cause;
        r.trap_addr = v.trap_addr;
        r.timer = v.timer;
        r.cycle_cnt = v.cycle_cnt;
        r.executed_cnt = v.executed_cnt;
        r.dscratch0 = v.dscratch0;
        r.dscratch1 = v.dscratch1;
        r.dpc = v.dpc;
        r.halt_cause = v.halt_cause;
        r.dcsr_ebreakm = v.dcsr_ebreakm;
        r.dcsr_stopcount = v.dcsr_stopcount;
        r.dcsr_stoptimer = v.dcsr_stoptimer;
        r.dcsr_step = v.dcsr_step;
        r.dcsr_stepie = v.dcsr_stepie;
        r.stepping_mode_cnt = v.stepping_mode_cnt;
        r.ins_per_step = v.ins_per_step;
        r.flushi_ena = v.flushi_ena;
        r.flushi_addr = v.flushi_addr;
    }
}

}  // namespace debugger

