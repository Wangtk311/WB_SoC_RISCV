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
 *
 * @details    Use the following targets attributes to generate trace files:
 *             GenerateRef - Generate memory and registers write accesses
 *                           trace files to compare them with functional model
 *             InVcdFile   - Stimulus VCD file
 *             OutVcdFile  - Reference VCD file with any number of signals
 *
 * @note       When GenerateRef is true Core uses step counter instead 
 *             of clock counter to generate callbacks.
 */

#pragma once

#include "iclass.h"
#include "iservice.h"
#include "ihap.h"
#include "async_tqueue.h"
#include "coreservices/ithread.h"
#include "coreservices/icpuriscv.h"
#include "coreservices/imemop.h"
#include "coreservices/iclock.h"
#include "coreservices/icmdexec.h"
#include "coreservices/itap.h"
#include "coreservices/iirq.h"
#include "cmds/cmd_br_riscv.h"
#include "rtl_wrapper.h"
#include "l1serdes.h"
#include "ambalib/types_amba.h"
#include "riverlib/river_amba.h"
#include "riverlib/l2cache/l2_top.h"
#include "dmi/dmidebug.h"
#include <systemc.h>

namespace debugger {

class CpuRiscV_RTL : public IService, 
                 public IThread,
                 public IClock,
                 public IHap {
 public:
    CpuRiscV_RTL(const char *name);
    virtual ~CpuRiscV_RTL();

    /** IService interface */
    virtual void initService(const AttributeType *args) override;
    virtual void postinitService() override;
    virtual void predeleteService() override ;

    /** IClock */
    virtual uint64_t getStepCounter() {
        return wrapper_->getClockCounter();
    }


    virtual void registerStepCallback(IClockListener *cb, uint64_t t) {
        wrapper_->registerStepCallback(cb, t);
    }

    bool moveStepCallback(IClockListener *cb, uint64_t t) {
        registerStepCallback(cb, t);
        return false;
    }

    virtual double getFreqHz() {
        return static_cast<double>(freqHz_.to_uint64());
    }

    /** IHap */
    virtual void hapTriggered(EHapType type, uint64_t param,
                              const char *descr);

    virtual void stop();

 protected:
    /** IThread interface */
    virtual void busyLoop();

 private:
    void createSystemC();
    void deleteSystemC();

 private:
    AttributeType hartid_;
    AttributeType asyncReset_;
    AttributeType fpuEnable_;
    AttributeType tracerEnable_;
    AttributeType l2CacheEnable_;
    AttributeType coherenceEnable_;
    AttributeType clint_;
    AttributeType plic_;
    AttributeType bus_;
    AttributeType cmdexec_;
    AttributeType dmibar_;
    AttributeType freqHz_;
    AttributeType InVcdFile_;
    AttributeType OutVcdFile_;
    event_def config_done_;

    IIrqController *iirqloc_;
    IIrqController *iirqext_;
    ICmdExecutor *icmdexec_;
    IMemoryOperation *ibus_;

    sc_signal<bool> w_clk;
    sc_signal<bool> w_sys_nrst;
    sc_signal<bool> w_dmi_nrst;

    // AXI4 input structure:
    sc_signal<axi4_l1_in_type> corei0;
    sc_signal<axi4_l1_out_type> coreo0;
    sc_signal<axi4_l1_in_type> corei1;
    sc_signal<axi4_l1_out_type> coreo1;
    sc_signal<axi4_l1_in_type> corei2;
    sc_signal<axi4_l1_out_type> coreo2;
    sc_signal<axi4_l1_in_type> corei3;
    sc_signal<axi4_l1_out_type> coreo3;
    sc_signal<axi4_l1_in_type> acpi;
    sc_signal<axi4_l1_out_type> acpo;
    sc_signal<axi4_master_config_type> xcfg;
    sc_signal<bool> w_flush_l2;
    // Interrupt lines:
    sc_vector<sc_signal<bool>> wb_irq_pending;       // Per Hart pending interrupts pins
    // Debug interface
    sc_signal<bool> w_ndmreset;
    sc_signal<sc_uint<CFG_CPU_MAX>> wb_halted;
    sc_signal<sc_uint<CFG_CPU_MAX>> wb_available;
    sc_signal<sc_uint<CFG_LOG2_CPU_MAX>> wb_hartsel;
    sc_signal<bool> w_haltreq;
    sc_signal<bool> w_resumereq;
    sc_signal<bool> w_resethaltreq;
    sc_signal<bool> w_hartreset;
    sc_signal<bool> w_dport_req_valid;
    sc_signal<sc_uint<DPortReq_Total>> wb_dport_type;
    sc_signal<sc_uint<CFG_CPU_ADDR_BITS>> wb_dport_addr;
    sc_signal<sc_uint<RISCV_ARCH>> wb_dport_wdata;
    sc_signal<sc_uint<3>> wb_dport_size;
    sc_signal<bool> w_dport_req_ready;
    sc_signal<bool> w_dport_resp_ready;
    sc_signal<bool> w_dport_resp_valid;
    sc_signal<bool> w_dport_resp_error;
    sc_signal<sc_uint<RISCV_ARCH>> wb_dport_rdata;
    sc_signal<sc_biguint<32*CFG_PROGBUF_REG_TOTAL>> wb_progbuf;
    sc_signal<bool> w_halted0;
    sc_signal<bool> w_available0;
    
    sc_signal<axi4_master_in_type> msti;
    sc_signal<axi4_master_out_type> msto;
    sc_signal<dport_in_type> wb_dporti;
    sc_signal<dport_out_type> wb_dporto;

    sc_trace_file *i_vcd_;      // stimulus pattern
    sc_trace_file *o_vcd_;      // reference pattern for comparision
    RiverAmba *core_;
    RtlWrapper *wrapper_;
    L1SerDes *l1serdes_;
    L2Top *l2cache_;
    DmiDebug *dmi_;

    CmdBrRiscv *pcmd_br_;
    ICommand *pcmd_cpu_;
};

DECLARE_CLASS(CpuRiscV_RTL)

}  // namespace debugger

