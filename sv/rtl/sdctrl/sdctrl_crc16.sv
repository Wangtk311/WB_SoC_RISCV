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

`timescale 1ns/10ps

module sdctrl_crc16 #(
    parameter bit async_reset = 1'b0
)
(
    input logic i_clk,                                      // CPU clock
    input logic i_nrst,                                     // Reset: active LOW
    input logic i_clear,                                    // Clear CRC register;
    input logic i_next,                                     // Shift enable strob
    input logic [3:0] i_dat,                                // Input bit
    output logic [15:0] o_crc16                             // Computed value
);

import sdctrl_crc16_pkg::*;

sdctrl_crc16_registers r, rin;

always_comb
begin: comb_proc
    sdctrl_crc16_registers v;
    logic v_inv16_3;
    logic [15:0] vb_crc16_3;
    logic v_inv16_2;
    logic [15:0] vb_crc16_2;
    logic v_inv16_1;
    logic [15:0] vb_crc16_1;
    logic v_inv16_0;
    logic [15:0] vb_crc16_0;

    v_inv16_3 = 0;
    vb_crc16_3 = 0;
    v_inv16_2 = 0;
    vb_crc16_2 = 0;
    v_inv16_1 = 0;
    vb_crc16_1 = 0;
    v_inv16_0 = 0;
    vb_crc16_0 = 0;

    v = r;

    // CRC16 = x^16 + x^12 + x^5 + 1
    v_inv16_3 = (r.crc16[15] ^ i_dat[3]);
    vb_crc16_3[15] = r.crc16[14];
    vb_crc16_3[14] = r.crc16[13];
    vb_crc16_3[13] = r.crc16[12];
    vb_crc16_3[12] = (r.crc16[11] ^ v_inv16_3);
    vb_crc16_3[11] = r.crc16[10];
    vb_crc16_3[10] = r.crc16[9];
    vb_crc16_3[9] = r.crc16[8];
    vb_crc16_3[8] = r.crc16[7];
    vb_crc16_3[7] = r.crc16[6];
    vb_crc16_3[6] = r.crc16[5];
    vb_crc16_3[5] = (r.crc16[4] ^ v_inv16_3);
    vb_crc16_3[4] = r.crc16[3];
    vb_crc16_3[3] = r.crc16[2];
    vb_crc16_3[2] = r.crc16[1];
    vb_crc16_3[1] = r.crc16[0];
    vb_crc16_3[0] = v_inv16_3;

    v_inv16_2 = (vb_crc16_3[15] ^ i_dat[2]);
    vb_crc16_2[15] = vb_crc16_3[14];
    vb_crc16_2[14] = vb_crc16_3[13];
    vb_crc16_2[13] = vb_crc16_3[12];
    vb_crc16_2[12] = (vb_crc16_3[11] ^ v_inv16_2);
    vb_crc16_2[11] = vb_crc16_3[10];
    vb_crc16_2[10] = vb_crc16_3[9];
    vb_crc16_2[9] = vb_crc16_3[8];
    vb_crc16_2[8] = vb_crc16_3[7];
    vb_crc16_2[7] = vb_crc16_3[6];
    vb_crc16_2[6] = vb_crc16_3[5];
    vb_crc16_2[5] = (vb_crc16_3[4] ^ v_inv16_2);
    vb_crc16_2[4] = vb_crc16_3[3];
    vb_crc16_2[3] = vb_crc16_3[2];
    vb_crc16_2[2] = vb_crc16_3[1];
    vb_crc16_2[1] = vb_crc16_3[0];
    vb_crc16_2[0] = v_inv16_2;

    v_inv16_1 = (vb_crc16_2[15] ^ i_dat[1]);
    vb_crc16_1[15] = vb_crc16_2[14];
    vb_crc16_1[14] = vb_crc16_2[13];
    vb_crc16_1[13] = vb_crc16_2[12];
    vb_crc16_1[12] = (vb_crc16_2[11] ^ v_inv16_1);
    vb_crc16_1[11] = vb_crc16_2[10];
    vb_crc16_1[10] = vb_crc16_2[9];
    vb_crc16_1[9] = vb_crc16_2[8];
    vb_crc16_1[8] = vb_crc16_2[7];
    vb_crc16_1[7] = vb_crc16_2[6];
    vb_crc16_1[6] = vb_crc16_2[5];
    vb_crc16_1[5] = (vb_crc16_2[4] ^ v_inv16_1);
    vb_crc16_1[4] = vb_crc16_2[3];
    vb_crc16_1[3] = vb_crc16_2[2];
    vb_crc16_1[2] = vb_crc16_2[1];
    vb_crc16_1[1] = vb_crc16_2[0];
    vb_crc16_1[0] = v_inv16_1;

    v_inv16_0 = (vb_crc16_1[15] ^ i_dat[0]);
    vb_crc16_0[15] = vb_crc16_1[14];
    vb_crc16_0[14] = vb_crc16_1[13];
    vb_crc16_0[13] = vb_crc16_1[12];
    vb_crc16_0[12] = (vb_crc16_1[11] ^ v_inv16_0);
    vb_crc16_0[11] = vb_crc16_1[10];
    vb_crc16_0[10] = vb_crc16_1[9];
    vb_crc16_0[9] = vb_crc16_1[8];
    vb_crc16_0[8] = vb_crc16_1[7];
    vb_crc16_0[7] = vb_crc16_1[6];
    vb_crc16_0[6] = vb_crc16_1[5];
    vb_crc16_0[5] = (vb_crc16_1[4] ^ v_inv16_0);
    vb_crc16_0[4] = vb_crc16_1[3];
    vb_crc16_0[3] = vb_crc16_1[2];
    vb_crc16_0[2] = vb_crc16_1[1];
    vb_crc16_0[1] = vb_crc16_1[0];
    vb_crc16_0[0] = v_inv16_0;

    if (i_clear == 1'b1) begin
        v.crc16 = '0;
    end else if (i_next == 1'b1) begin
        v.crc16 = vb_crc16_0;
    end

    if (~async_reset && i_nrst == 1'b0) begin
        v = sdctrl_crc16_r_reset;
    end

    o_crc16 = r.crc16;

    rin = v;
end: comb_proc

generate
    if (async_reset) begin: async_rst_gen

        always_ff @(posedge i_clk, negedge i_nrst) begin: rg_proc
            if (i_nrst == 1'b0) begin
                r <= sdctrl_crc16_r_reset;
            end else begin
                r <= rin;
            end
        end: rg_proc


    end: async_rst_gen
    else begin: no_rst_gen

        always_ff @(posedge i_clk) begin: rg_proc
            r <= rin;
        end: rg_proc

    end: no_rst_gen
endgenerate

endmodule: sdctrl_crc16
