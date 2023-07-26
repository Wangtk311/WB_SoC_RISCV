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
#include "../ambalib/apb_slv.h"
#include "../ambalib/axi_slv.h"
#include "../misclib/sfifo.h"
#include "api_core.h"

namespace debugger {

template<int log2_fifosz = 9>
SC_MODULE(sdctrl) {
 public:
    sc_in<bool> i_clk;                                      // CPU clock
    sc_in<bool> i_nrst;                                     // Reset: active LOW
    sc_in<mapinfo_type> i_xmapinfo;                         // APB interconnect slot information
    sc_out<dev_config_type> o_xcfg;                         // APB Device descriptor
    sc_in<axi4_slave_in_type> i_xslvi;                      // AXI input interface to access SD-card memory
    sc_out<axi4_slave_out_type> o_xslvo;                    // AXI output interface to access SD-card memory
    sc_in<mapinfo_type> i_pmapinfo;                         // APB interconnect slot information
    sc_out<dev_config_type> o_pcfg;                         // APB sd-controller configuration registers descriptor
    sc_in<apb_in_type> i_apbi;                              // APB Slave to Bridge interface
    sc_out<apb_out_type> o_apbo;                            // APB Bridge to Slave interface
    sc_out<bool> o_sclk;                                    // Clock up to 50 MHz
    sc_in<bool> i_cmd;                                      // Command response;
    sc_out<bool> o_cmd;                                     // Command request; DO in SPI mode
    sc_out<bool> o_cmd_dir;                                 // Direction bit: 1=input; 0=output
    sc_in<bool> i_dat0;                                     // Data Line[0] input; DI in SPI mode
    sc_out<bool> o_dat0;                                    // Data Line[0] output
    sc_out<bool> o_dat0_dir;                                // Direction bit: 1=input; 0=output
    sc_in<bool> i_dat1;                                     // Data Line[1] input
    sc_out<bool> o_dat1;                                    // Data Line[1] output
    sc_out<bool> o_dat1_dir;                                // Direction bit: 1=input; 0=output
    sc_in<bool> i_dat2;                                     // Data Line[2] input
    sc_out<bool> o_dat2;                                    // Data Line[2] output
    sc_out<bool> o_dat2_dir;                                // Direction bit: 1=input; 0=output
    sc_in<bool> i_cd_dat3;                                  // Card Detect / Data Line[3] input
    sc_out<bool> o_cd_dat3;                                 // Card Detect / Data Line[3] output; CS output in SPI mode
    sc_out<bool> o_cd_dat3_dir;                             // Direction bit: 1=input; 0=output
    sc_in<bool> i_detected;
    sc_in<bool> i_protect;

    void comb();
    void registers();

    SC_HAS_PROCESS(sdctrl);

    sdctrl(sc_module_name name,
           bool async_reset);
    virtual ~sdctrl();

    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);

 private:
    bool async_reset_;

    static const int fifo_dbits = 8;
    // SPI states
    static const uint8_t idle = 0;
    static const uint8_t wait_edge = 1;
    static const uint8_t send_data = 2;
    static const uint8_t recv_data = 3;
    static const uint8_t recv_sync = 4;
    static const uint8_t ending = 5;

    struct sdctrl_registers {
        sc_signal<sc_uint<32>> scaler;
        sc_signal<sc_uint<32>> scaler_cnt;
        sc_signal<sc_uint<16>> wdog;
        sc_signal<sc_uint<16>> wdog_cnt;
        sc_signal<bool> generate_crc;
        sc_signal<bool> rx_ena;
        sc_signal<bool> rx_synced;
        sc_signal<bool> rx_data_block;                      // Wait 0xFE start data block marker
        sc_signal<bool> level;
        sc_signal<bool> cs;
        sc_signal<sc_uint<3>> state;
        sc_signal<sc_uint<8>> shiftreg;
        sc_signal<sc_uint<16>> ena_byte_cnt;
        sc_signal<sc_uint<3>> bit_cnt;
        sc_signal<sc_uint<8>> tx_val;
        sc_signal<sc_uint<8>> rx_val;
        sc_signal<bool> rx_ready;
        sc_signal<sc_uint<7>> crc7;
        sc_signal<sc_uint<16>> crc16;
        sc_signal<sc_uint<8>> spi_resp;
        sc_signal<sc_uint<log2_fifosz>> txmark;
        sc_signal<sc_uint<log2_fifosz>> rxmark;
        sc_signal<bool> presp_valid;
        sc_signal<sc_uint<32>> presp_rdata;
        sc_signal<bool> presp_err;
    } v, r;

    void sdctrl_r_reset(sdctrl_registers &iv) {
        iv.scaler = 0;
        iv.scaler_cnt = 0;
        iv.wdog = 0;
        iv.wdog_cnt = 0;
        iv.generate_crc = 0;
        iv.rx_ena = 0;
        iv.rx_synced = 0;
        iv.rx_data_block = 0;
        iv.level = 1;
        iv.cs = 0;
        iv.state = idle;
        iv.shiftreg = ~0ul;
        iv.ena_byte_cnt = 0;
        iv.bit_cnt = 0;
        iv.tx_val = 0;
        iv.rx_val = 0;
        iv.rx_ready = 0;
        iv.crc7 = 0;
        iv.crc16 = 0;
        iv.spi_resp = 0;
        iv.txmark = 0;
        iv.rxmark = 0;
        iv.presp_valid = 0;
        iv.presp_rdata = 0;
        iv.presp_err = 0;
    }

    sc_signal<bool> w_preq_valid;
    sc_signal<sc_uint<32>> wb_preq_addr;
    sc_signal<bool> w_preq_write;
    sc_signal<sc_uint<32>> wb_preq_wdata;
    sc_signal<bool> w_mem_req_valid;
    sc_signal<sc_uint<CFG_SYSBUS_ADDR_BITS>> wb_mem_req_addr;
    sc_signal<sc_uint<8>> wb_mem_req_size;
    sc_signal<bool> w_mem_req_write;
    sc_signal<sc_uint<CFG_SYSBUS_DATA_BITS>> wb_mem_req_wdata;
    sc_signal<sc_uint<CFG_SYSBUS_DATA_BYTES>> wb_mem_req_wstrb;
    sc_signal<bool> w_mem_req_last;
    sc_signal<bool> w_mem_req_ready;
    sc_signal<bool> w_mem_resp_valid;
    sc_signal<sc_uint<CFG_SYSBUS_DATA_BITS>> wb_mem_resp_rdata;
    sc_signal<bool> wb_mem_resp_err;
    // Rx FIFO signals:
    sc_signal<bool> w_rxfifo_we;
    sc_signal<sc_uint<8>> wb_rxfifo_wdata;
    sc_signal<bool> w_rxfifo_re;
    sc_signal<sc_uint<8>> wb_rxfifo_rdata;
    sc_signal<sc_uint<(log2_fifosz + 1)>> wb_rxfifo_count;
    // Tx FIFO signals:
    sc_signal<bool> w_txfifo_we;
    sc_signal<sc_uint<8>> wb_txfifo_wdata;
    sc_signal<bool> w_txfifo_re;
    sc_signal<sc_uint<8>> wb_txfifo_rdata;
    sc_signal<sc_uint<(log2_fifosz + 1)>> wb_txfifo_count;

    apb_slv *pslv0;
    axi_slv *xslv0;
    sfifo<fifo_dbits, 9> *rxfifo;
    sfifo<fifo_dbits, 9> *txfifo;

};

template<int log2_fifosz>
sdctrl<log2_fifosz>::sdctrl(sc_module_name name,
                            bool async_reset)
    : sc_module(name),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_xmapinfo("i_xmapinfo"),
    o_xcfg("o_xcfg"),
    i_xslvi("i_xslvi"),
    o_xslvo("o_xslvo"),
    i_pmapinfo("i_pmapinfo"),
    o_pcfg("o_pcfg"),
    i_apbi("i_apbi"),
    o_apbo("o_apbo"),
    o_sclk("o_sclk"),
    i_cmd("i_cmd"),
    o_cmd("o_cmd"),
    o_cmd_dir("o_cmd_dir"),
    i_dat0("i_dat0"),
    o_dat0("o_dat0"),
    o_dat0_dir("o_dat0_dir"),
    i_dat1("i_dat1"),
    o_dat1("o_dat1"),
    o_dat1_dir("o_dat1_dir"),
    i_dat2("i_dat2"),
    o_dat2("o_dat2"),
    o_dat2_dir("o_dat2_dir"),
    i_cd_dat3("i_cd_dat3"),
    o_cd_dat3("o_cd_dat3"),
    o_cd_dat3_dir("o_cd_dat3_dir"),
    i_detected("i_detected"),
    i_protect("i_protect") {

    async_reset_ = async_reset;
    pslv0 = 0;
    xslv0 = 0;
    rxfifo = 0;
    txfifo = 0;

    pslv0 = new apb_slv("pslv0", async_reset,
                         VENDOR_OPTIMITECH,
                         OPTIMITECH_SDCTRL_REG);
    pslv0->i_clk(i_clk);
    pslv0->i_nrst(i_nrst);
    pslv0->i_mapinfo(i_pmapinfo);
    pslv0->o_cfg(o_pcfg);
    pslv0->i_apbi(i_apbi);
    pslv0->o_apbo(o_apbo);
    pslv0->o_req_valid(w_preq_valid);
    pslv0->o_req_addr(wb_preq_addr);
    pslv0->o_req_write(w_preq_write);
    pslv0->o_req_wdata(wb_preq_wdata);
    pslv0->i_resp_valid(r.presp_valid);
    pslv0->i_resp_rdata(r.presp_rdata);
    pslv0->i_resp_err(r.presp_err);


    xslv0 = new axi_slv("xslv0", async_reset,
                         VENDOR_OPTIMITECH,
                         OPTIMITECH_SDCTRL_MEM);
    xslv0->i_clk(i_clk);
    xslv0->i_nrst(i_nrst);
    xslv0->i_mapinfo(i_xmapinfo);
    xslv0->o_cfg(o_xcfg);
    xslv0->i_xslvi(i_xslvi);
    xslv0->o_xslvo(o_xslvo);
    xslv0->o_req_valid(w_mem_req_valid);
    xslv0->o_req_addr(wb_mem_req_addr);
    xslv0->o_req_size(wb_mem_req_size);
    xslv0->o_req_write(w_mem_req_write);
    xslv0->o_req_wdata(wb_mem_req_wdata);
    xslv0->o_req_wstrb(wb_mem_req_wstrb);
    xslv0->o_req_last(w_mem_req_last);
    xslv0->i_req_ready(w_mem_req_ready);
    xslv0->i_resp_valid(w_mem_resp_valid);
    xslv0->i_resp_rdata(wb_mem_resp_rdata);
    xslv0->i_resp_err(wb_mem_resp_err);


    rxfifo = new sfifo<fifo_dbits,
                       9>("rxfifo", async_reset);
    rxfifo->i_clk(i_clk);
    rxfifo->i_nrst(i_nrst);
    rxfifo->i_we(w_rxfifo_we);
    rxfifo->i_wdata(wb_rxfifo_wdata);
    rxfifo->i_re(w_rxfifo_re);
    rxfifo->o_rdata(wb_rxfifo_rdata);
    rxfifo->o_count(wb_rxfifo_count);


    txfifo = new sfifo<fifo_dbits,
                       9>("txfifo", async_reset);
    txfifo->i_clk(i_clk);
    txfifo->i_nrst(i_nrst);
    txfifo->i_we(w_txfifo_we);
    txfifo->i_wdata(wb_txfifo_wdata);
    txfifo->i_re(w_txfifo_re);
    txfifo->o_rdata(wb_txfifo_rdata);
    txfifo->o_count(wb_txfifo_count);



    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_xmapinfo;
    sensitive << i_xslvi;
    sensitive << i_pmapinfo;
    sensitive << i_apbi;
    sensitive << i_cmd;
    sensitive << i_dat0;
    sensitive << i_dat1;
    sensitive << i_dat2;
    sensitive << i_cd_dat3;
    sensitive << i_detected;
    sensitive << i_protect;
    sensitive << w_preq_valid;
    sensitive << wb_preq_addr;
    sensitive << w_preq_write;
    sensitive << wb_preq_wdata;
    sensitive << w_mem_req_valid;
    sensitive << wb_mem_req_addr;
    sensitive << wb_mem_req_size;
    sensitive << w_mem_req_write;
    sensitive << wb_mem_req_wdata;
    sensitive << wb_mem_req_wstrb;
    sensitive << w_mem_req_last;
    sensitive << w_mem_req_ready;
    sensitive << w_mem_resp_valid;
    sensitive << wb_mem_resp_rdata;
    sensitive << wb_mem_resp_err;
    sensitive << w_rxfifo_we;
    sensitive << wb_rxfifo_wdata;
    sensitive << w_rxfifo_re;
    sensitive << wb_rxfifo_rdata;
    sensitive << wb_rxfifo_count;
    sensitive << w_txfifo_we;
    sensitive << wb_txfifo_wdata;
    sensitive << w_txfifo_re;
    sensitive << wb_txfifo_rdata;
    sensitive << wb_txfifo_count;
    sensitive << r.scaler;
    sensitive << r.scaler_cnt;
    sensitive << r.wdog;
    sensitive << r.wdog_cnt;
    sensitive << r.generate_crc;
    sensitive << r.rx_ena;
    sensitive << r.rx_synced;
    sensitive << r.rx_data_block;
    sensitive << r.level;
    sensitive << r.cs;
    sensitive << r.state;
    sensitive << r.shiftreg;
    sensitive << r.ena_byte_cnt;
    sensitive << r.bit_cnt;
    sensitive << r.tx_val;
    sensitive << r.rx_val;
    sensitive << r.rx_ready;
    sensitive << r.crc7;
    sensitive << r.crc16;
    sensitive << r.spi_resp;
    sensitive << r.txmark;
    sensitive << r.rxmark;
    sensitive << r.presp_valid;
    sensitive << r.presp_rdata;
    sensitive << r.presp_err;

    SC_METHOD(registers);
    sensitive << i_nrst;
    sensitive << i_clk.pos();
}

template<int log2_fifosz>
sdctrl<log2_fifosz>::~sdctrl() {
    if (pslv0) {
        delete pslv0;
    }
    if (xslv0) {
        delete xslv0;
    }
    if (rxfifo) {
        delete rxfifo;
    }
    if (txfifo) {
        delete txfifo;
    }
}

template<int log2_fifosz>
void sdctrl<log2_fifosz>::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    std::string pn(name());
    if (o_vcd) {
        sc_trace(o_vcd, i_xmapinfo, i_xmapinfo.name());
        sc_trace(o_vcd, o_xcfg, o_xcfg.name());
        sc_trace(o_vcd, i_xslvi, i_xslvi.name());
        sc_trace(o_vcd, o_xslvo, o_xslvo.name());
        sc_trace(o_vcd, i_pmapinfo, i_pmapinfo.name());
        sc_trace(o_vcd, o_pcfg, o_pcfg.name());
        sc_trace(o_vcd, i_apbi, i_apbi.name());
        sc_trace(o_vcd, o_apbo, o_apbo.name());
        sc_trace(o_vcd, o_sclk, o_sclk.name());
        sc_trace(o_vcd, i_cmd, i_cmd.name());
        sc_trace(o_vcd, o_cmd, o_cmd.name());
        sc_trace(o_vcd, o_cmd_dir, o_cmd_dir.name());
        sc_trace(o_vcd, i_dat0, i_dat0.name());
        sc_trace(o_vcd, o_dat0, o_dat0.name());
        sc_trace(o_vcd, o_dat0_dir, o_dat0_dir.name());
        sc_trace(o_vcd, i_dat1, i_dat1.name());
        sc_trace(o_vcd, o_dat1, o_dat1.name());
        sc_trace(o_vcd, o_dat1_dir, o_dat1_dir.name());
        sc_trace(o_vcd, i_dat2, i_dat2.name());
        sc_trace(o_vcd, o_dat2, o_dat2.name());
        sc_trace(o_vcd, o_dat2_dir, o_dat2_dir.name());
        sc_trace(o_vcd, i_cd_dat3, i_cd_dat3.name());
        sc_trace(o_vcd, o_cd_dat3, o_cd_dat3.name());
        sc_trace(o_vcd, o_cd_dat3_dir, o_cd_dat3_dir.name());
        sc_trace(o_vcd, i_detected, i_detected.name());
        sc_trace(o_vcd, i_protect, i_protect.name());
        sc_trace(o_vcd, r.scaler, pn + ".r_scaler");
        sc_trace(o_vcd, r.scaler_cnt, pn + ".r_scaler_cnt");
        sc_trace(o_vcd, r.wdog, pn + ".r_wdog");
        sc_trace(o_vcd, r.wdog_cnt, pn + ".r_wdog_cnt");
        sc_trace(o_vcd, r.generate_crc, pn + ".r_generate_crc");
        sc_trace(o_vcd, r.rx_ena, pn + ".r_rx_ena");
        sc_trace(o_vcd, r.rx_synced, pn + ".r_rx_synced");
        sc_trace(o_vcd, r.rx_data_block, pn + ".r_rx_data_block");
        sc_trace(o_vcd, r.level, pn + ".r_level");
        sc_trace(o_vcd, r.cs, pn + ".r_cs");
        sc_trace(o_vcd, r.state, pn + ".r_state");
        sc_trace(o_vcd, r.shiftreg, pn + ".r_shiftreg");
        sc_trace(o_vcd, r.ena_byte_cnt, pn + ".r_ena_byte_cnt");
        sc_trace(o_vcd, r.bit_cnt, pn + ".r_bit_cnt");
        sc_trace(o_vcd, r.tx_val, pn + ".r_tx_val");
        sc_trace(o_vcd, r.rx_val, pn + ".r_rx_val");
        sc_trace(o_vcd, r.rx_ready, pn + ".r_rx_ready");
        sc_trace(o_vcd, r.crc7, pn + ".r_crc7");
        sc_trace(o_vcd, r.crc16, pn + ".r_crc16");
        sc_trace(o_vcd, r.spi_resp, pn + ".r_spi_resp");
        sc_trace(o_vcd, r.txmark, pn + ".r_txmark");
        sc_trace(o_vcd, r.rxmark, pn + ".r_rxmark");
        sc_trace(o_vcd, r.presp_valid, pn + ".r_presp_valid");
        sc_trace(o_vcd, r.presp_rdata, pn + ".r_presp_rdata");
        sc_trace(o_vcd, r.presp_err, pn + ".r_presp_err");
    }

    if (pslv0) {
        pslv0->generateVCD(i_vcd, o_vcd);
    }
    if (xslv0) {
        xslv0->generateVCD(i_vcd, o_vcd);
    }
    if (rxfifo) {
        rxfifo->generateVCD(i_vcd, o_vcd);
    }
    if (txfifo) {
        txfifo->generateVCD(i_vcd, o_vcd);
    }
}

template<int log2_fifosz>
void sdctrl<log2_fifosz>::comb() {
    bool v_posedge;
    bool v_negedge;
    bool v_txfifo_re;
    bool v_txfifo_we;
    sc_uint<8> vb_txfifo_wdata;
    bool v_rxfifo_re;
    bool v_inv7;
    sc_uint<7> vb_crc7;
    bool v_inv16;
    sc_uint<16> vb_crc16;
    sc_uint<32> vb_rdata;
    sc_uint<8> vb_shiftreg_next;

    v_posedge = 0;
    v_negedge = 0;
    v_txfifo_re = 0;
    v_txfifo_we = 0;
    vb_txfifo_wdata = 0;
    v_rxfifo_re = 0;
    v_inv7 = 0;
    vb_crc7 = 0;
    v_inv16 = 0;
    vb_crc16 = 0;
    vb_rdata = 0;
    vb_shiftreg_next = 0;

    v = r;

    // CRC7 = x^7 + x^3 + 1
    v_inv7 = (r.crc7.read()[6] ^ r.shiftreg.read()[7]);
    vb_crc7[6] = r.crc7.read()[5];
    vb_crc7[5] = r.crc7.read()[4];
    vb_crc7[4] = r.crc7.read()[3];
    vb_crc7[3] = (r.crc7.read()[2] ^ v_inv7);
    vb_crc7[2] = r.crc7.read()[1];
    vb_crc7[1] = r.crc7.read()[0];
    vb_crc7[0] = v_inv7;
    // CRC16 = x^16 + x^12 + x^5 + 1
    v_inv16 = (r.crc16.read()[15] ^ i_dat0.read());
    vb_crc16[15] = r.crc16.read()[14];
    vb_crc16[14] = r.crc16.read()[13];
    vb_crc16[13] = r.crc16.read()[12];
    vb_crc16[12] = (r.crc16.read()[11] ^ v_inv16);
    vb_crc16[11] = r.crc16.read()[10];
    vb_crc16[10] = r.crc16.read()[9];
    vb_crc16[9] = r.crc16.read()[8];
    vb_crc16[8] = r.crc16.read()[7];
    vb_crc16[7] = r.crc16.read()[6];
    vb_crc16[6] = r.crc16.read()[5];
    vb_crc16[5] = (r.crc16.read()[4] ^ v_inv16);
    vb_crc16[4] = r.crc16.read()[3];
    vb_crc16[3] = r.crc16.read()[2];
    vb_crc16[2] = r.crc16.read()[1];
    vb_crc16[1] = r.crc16.read()[0];
    vb_crc16[0] = v_inv16;

    // system bus clock scaler to baudrate:
    if (r.scaler.read().or_reduce() == 1) {
        if (r.scaler_cnt.read() == (r.scaler.read() - 1)) {
            v.scaler_cnt = 0;
            v.level = (!r.level);
            v_posedge = (!r.level);
            v_negedge = r.level;
        } else {
            v.scaler_cnt = (r.scaler_cnt.read() + 1);
        }
    }

    if (r.rx_ena.read() == 0) {
        vb_shiftreg_next = ((r.shiftreg.read()(6, 0) << 1) | 1);
    } else {
        vb_shiftreg_next = (r.shiftreg.read()(6, 0), i_dat0.read());
    }
    if (r.cs.read() == 1) {
        if (((v_negedge == 1) && (r.rx_ena.read() == 0))
                || ((v_posedge == 1) && (r.rx_ena.read() == 1))) {
            v.shiftreg = vb_shiftreg_next;
        }
    }

    if ((v_negedge == 1) && (r.cs.read() == 1)) {
        if (r.bit_cnt.read().or_reduce() == 1) {
            if ((r.rx_ena.read() == 0)
                    || ((r.rx_ena.read() == 1) && (r.rx_synced.read() == 1))) {
                v.bit_cnt = (r.bit_cnt.read() - 1);
            }
        } else {
            v.cs = 0;
        }
    }

    v.rx_ready = 0;
    if (v_posedge == 1) {
        if ((r.cs.read() == 1) && ((r.rx_ena.read() == 0) || (r.rx_synced.read() == 1))) {
            v.crc7 = vb_crc7;
            v.crc16 = vb_crc16;
        }
    }

    // Transmitter's state machine:
    switch (r.state.read()) {
    case idle:
        v.wdog_cnt = r.wdog;
        if (r.ena_byte_cnt.read().or_reduce() == 1) {
            v_txfifo_re = (!r.rx_ena);
            if ((wb_txfifo_count.read().or_reduce() == 0) || (r.rx_ena.read() == 1)) {
                // FIFO is empty or RX is enabled:
                v.tx_val = ~0ull;
            } else {
                v.tx_val = wb_txfifo_rdata;
            }
            v.state = wait_edge;
            v.ena_byte_cnt = (r.ena_byte_cnt.read() - 1);
            v.crc7 = 0;
        } else {
            v.tx_val = ~0ull;
        }
        break;
    case wait_edge:
        if (v_negedge == 1) {
            v.cs = 1;
            v.bit_cnt = 7;
            if (r.rx_ena.read() == 1) {
                v.shiftreg = 0;
                if (r.rx_data_block.read() == 1) {
                    v.state = recv_sync;
                } else {
                    v.state = recv_data;
                }
            } else {
                v.shiftreg = r.tx_val;
                v.state = send_data;
            }
        }
        break;
    case send_data:
        if ((r.bit_cnt.read().or_reduce() == 0) && (v_posedge == 1)) {
            if (r.ena_byte_cnt.read().or_reduce() == 1) {
                v_txfifo_re = 1;
                if (wb_txfifo_count.read().or_reduce() == 0) {
                    // FIFO is empty:
                    v.tx_val = ~0ull;
                } else {
                    v.tx_val = wb_txfifo_rdata;
                }
                v.state = wait_edge;
                v.ena_byte_cnt = (r.ena_byte_cnt.read() - 1);
            } else if (r.generate_crc.read() == 1) {
                v.tx_val = ((vb_crc7 << 1) | 1);
                v.generate_crc = 0;
                v.state = wait_edge;
            } else {
                v.state = ending;
            }
        }
        break;
    case recv_data:
        if (v_posedge == 1) {
            if (r.rx_synced.read() == 0) {
                v.rx_synced = ((r.cs.read() == 1) && (!i_dat0));
                if (r.wdog_cnt.read().or_reduce() == 1) {
                    v.wdog_cnt = (r.wdog_cnt.read() - 1);
                } else if (r.wdog.read().or_reduce() == 0) {
                    // Wait Start bit infinitely
                } else {
                    // Wait Start bit time is out:
                    v.rx_synced = 1;
                }
            }
            // Check RX shift ready
            if (r.bit_cnt.read().or_reduce() == 0) {
                if (r.ena_byte_cnt.read().or_reduce() == 1) {
                    v.state = wait_edge;
                    v.ena_byte_cnt = (r.ena_byte_cnt.read() - 1);
                } else {
                    v.state = ending;
                }
                v.rx_ready = 1;
                v.rx_val = vb_shiftreg_next;
            }
        }
        break;
    case recv_sync:
        if (v_posedge == 1) {
            if ((vb_shiftreg_next == 0xFE)
                    || (r.wdog_cnt.read().or_reduce() == 0)) {
                v.state = ending;
                v.rx_val = vb_shiftreg_next;
                v.rx_ready = 1;
                v.ena_byte_cnt = 0;
                v.bit_cnt = 0;
                v.crc16 = 0;
            } else {
                v.wdog_cnt = (r.wdog_cnt.read() - 1);
            }
        }
        break;
    case ending:
        if (r.cs.read() == 0) {
            v.state = idle;
        }
        break;
    default:
        break;
    }

    // Registers access:
    switch (wb_preq_addr.read()(11, 2)) {
    case 0x0:                                               // 0x00: sckdiv
        vb_rdata = r.scaler;
        if ((w_preq_valid.read() == 1) && (w_preq_write.read() == 1)) {
            v.scaler = wb_preq_wdata.read()(30, 0);
            v.scaler_cnt = 0;
        }
        break;
    case 0x2:                                               // 0x08: reserved (watchdog)
        vb_rdata(15, 0) = r.wdog;
        if ((w_preq_valid.read() == 1) && (w_preq_write.read() == 1)) {
            v.wdog = wb_preq_wdata.read()(15, 0);
        }
        break;
    case 0x11:                                              // 0x44: reserved 4 (txctrl)
        vb_rdata[0] = i_detected.read();                    // [0] sd card inserted
        vb_rdata[1] = i_protect.read();                     // [1] write protect
        vb_rdata[2] = i_dat0.read();                        // [2] miso data bit
        vb_rdata(6, 4) = r.state;                           // [6:4] state machine
        vb_rdata[7] = r.generate_crc.read();                // [7] Compute and generate CRC as the last Tx byte
        vb_rdata[8] = r.rx_ena.read();                      // [8] Receive data and write into FIFO only if rx_synced
        vb_rdata[9] = r.rx_synced.read();                   // [9] rx_ena=1 and start bit received
        vb_rdata[10] = r.rx_data_block.read();              // [10] rx_data_block=1 receive certain template byte
        vb_rdata(31, 16) = r.ena_byte_cnt;                  // [31:16] Number of bytes to transmit
        if ((w_preq_valid.read() == 1) && (w_preq_write.read() == 1)) {
            v.generate_crc = wb_preq_wdata.read()[7];
            v.rx_ena = wb_preq_wdata.read()[8];
            v.rx_synced = wb_preq_wdata.read()[9];
            v.rx_data_block = wb_preq_wdata.read()[10];
            v.ena_byte_cnt = wb_preq_wdata.read()(31, 16);
        }
        break;
    case 0x12:                                              // 0x48: Tx FIFO Data
        vb_rdata[31] = wb_txfifo_count.read().and_reduce();
        if (w_preq_valid.read() == 1) {
            if (w_preq_write.read() == 1) {
                v_txfifo_we = 1;
                vb_txfifo_wdata = wb_preq_wdata.read()(7, 0);
            }
        }
        break;
    case 0x13:                                              // 0x4C: Rx FIFO Data
        vb_rdata(7, 0) = wb_rxfifo_rdata;
        vb_rdata[31] = (!wb_rxfifo_count.read().or_reduce());
        if (w_preq_valid.read() == 1) {
            if (w_preq_write.read() == 1) {
                // do nothing:
            } else {
                v_rxfifo_re = 1;
            }
        }
        break;
    case 0x14:                                              // 0x50: Tx FIFO Watermark
        vb_rdata((log2_fifosz - 1), 0) = r.txmark;
        if (w_preq_valid.read() == 1) {
            if (w_preq_write.read() == 1) {
                v.txmark = wb_preq_wdata.read()((log2_fifosz - 1), 0);
            }
        }
        break;
    case 0x15:                                              // 0x54: Rx FIFO Watermark
        vb_rdata((log2_fifosz - 1), 0) = r.rxmark;
        if (w_preq_valid.read() == 1) {
            if (w_preq_write.read() == 1) {
                v.rxmark = wb_preq_wdata.read()((log2_fifosz - 1), 0);
            }
        }
        break;
    case 0x16:                                              // 0x58: CRC16 value (reserved FU740)
        vb_rdata(15, 0) = r.crc16;
        if (w_preq_valid.read() == 1) {
            if (w_preq_write.read() == 1) {
                v.crc16 = wb_preq_wdata.read()(15, 0);
            }
        }
        break;
    default:
        break;
    }

    w_rxfifo_we = r.rx_ready;
    wb_rxfifo_wdata = r.rx_val;
    w_rxfifo_re = v_rxfifo_re;

    w_txfifo_we = v_txfifo_we;
    wb_txfifo_wdata = vb_txfifo_wdata;
    w_txfifo_re = v_txfifo_re;

    v.presp_valid = w_preq_valid;
    v.presp_rdata = vb_rdata;
    v.presp_err = 0;

    if (!async_reset_ && i_nrst.read() == 0) {
        sdctrl_r_reset(v);
    }

    o_sclk = (r.level.read() & r.cs.read());
    o_cmd = (r.rx_ena || r.shiftreg.read()[7]);
    o_cd_dat3 = (!r.cs);
    // Direction bits:
    o_cmd_dir = 0;
    o_dat0_dir = 1;
    o_dat1_dir = 1;
    o_dat2_dir = 1;
    o_cd_dat3_dir = 0;
    // Memory request:
    w_mem_req_ready = 1;
    w_mem_resp_valid = 1;
    wb_mem_resp_rdata = ~0ull;
    wb_mem_resp_err = 0;
}

template<int log2_fifosz>
void sdctrl<log2_fifosz>::registers() {
    if (async_reset_ && i_nrst.read() == 0) {
        sdctrl_r_reset(r);
    } else {
        r = v;
    }
}

}  // namespace debugger

