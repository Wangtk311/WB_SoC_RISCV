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

 #pragma once

#include <systemc.h>
#include <string>
#include <iomanip>

namespace debugger {

static const int CFG_BUS_ADDR_WIDTH      = 64;
static const int CFG_LOG2_BUS_DATA_BYTES = 3;
static const int CFG_BUS_ID_BITS         = 5;
static const int CFG_BUS_USER_BITS       = 1;

static const int CFG_SYSBUS_ADDR_BITS = CFG_BUS_ADDR_WIDTH;
static const int CFG_LOG2_SYSBUS_DATA_BYTES = CFG_LOG2_BUS_DATA_BYTES;
static const int CFG_SYSBUS_ID_BITS = CFG_BUS_ID_BITS;
static const int CFG_SYSBUS_USER_BITS = CFG_BUS_USER_BITS;

static const int BUS_DATA_BYTES      = (1 << CFG_LOG2_BUS_DATA_BYTES);
static const int BUS_DATA_WIDTH      = 8 * BUS_DATA_BYTES;


// Burst length size decoder
static const int XSIZE_TOTAL = 8;
// Decoder of the transaction bytes from AXI format to Bytes.
static sc_uint<XSIZE_TOTAL> XSizeToBytes(sc_uint<3> xsize) {
    sc_uint<XSIZE_TOTAL> ret;

    ret = (1 << xsize.to_int());
    return ret;
}


static const unsigned ARCACHE_DEVICE_NON_BUFFERABLE = 0x0;  // 4'b0000
static const unsigned ARCACHE_WRBACK_READ_ALLOCATE  = 0xF;  // 4'b1111

static const unsigned AWCACHE_DEVICE_NON_BUFFERABLE = 0x0;  // 4'b0000
static const unsigned AWCACHE_WRBACK_WRITE_ALLOCATE = 0xF;  // 4'b1111

// see table C3-7 Permitted read address control signal combinations
//
//    read  |  cached  |  unique  |
//     0    |    0     |    *     |    ReadNoSnoop
//     0    |    1     |    0     |    ReadShared
//     0    |    1     |    1     |    ReadMakeUnique
static const int ARSNOOP_READ_NO_SNOOP     = 0x0;
static const int ARSNOOP_READ_SHARED       = 0x1;
static const int ARSNOOP_READ_MAKE_UNIQUE  = 0xC;

// see table C3-8 Permitted read address control signal combinations
//
//   write  |  cached  |  unique  |
//     1    |    0     |    *     |    WriteNoSnoop
//     1    |    1     |    1     |    WriteLineUnique
//     1    |    1     |    0     |    WriteBack
static const int AWSNOOP_WRITE_NO_SNOOP    = 0x0;
static const int AWSNOOP_WRITE_LINE_UNIQUE = 0x1;
static const int AWSNOOP_WRITE_BACK        = 0x3;

// see table C3-19
static const int AC_SNOOP_READ_UNIQUE   = 0x7;
static const int AC_SNOOP_MAKE_INVALID  = 0xD;


// @brief Fixed address burst operation.
// @details The address is the same for every transfer in the burst
//          (FIFO type)
static const uint8_t AXI_BURST_FIXED = 0;
// @brief Burst operation with address increment.
// @details The address for each transfer in the burst is an increment of
//         the address for the previous transfer. The increment value depends
//        on the size of the transfer.
static const uint8_t AXI_BURST_INCR = 1;
// @brief Burst operation with address increment and wrapping.
// @details A wrapping burst is similar to an incrementing burst, except that
//          the address wraps around to a lower address if an upper address
//          limit is reached
static const uint8_t AXI_BURST_WRAP = 2;
// @}


struct axi4_meta_type {
    sc_uint<CFG_BUS_ADDR_WIDTH> addr;
    sc_uint<8> len;              // burst len = len[7:0] + 1
    sc_uint<3> size;             // 0=1B; 1=2B; 2=4B; 3=8B; ...
    sc_uint<2> burst;            // 00=FIXED; 01=INCR; 10=WRAP; 11=reserved
    bool lock;
    sc_uint<4> cache;
    sc_uint<3> prot;
    sc_uint<4> qos;
    sc_uint<4> region;
};

typedef axi4_meta_type axi4_metadata_type;


class axi4_master_in_type {
 public:
    axi4_master_in_type(){}

    inline bool operator == (const axi4_master_in_type &rhs) const {
        return (rhs.aw_ready == aw_ready
             && rhs.w_ready == w_ready
             && rhs.b_valid == b_valid
             && rhs.b_resp == b_resp
             && rhs.b_id == b_id
             && rhs.b_user == b_user
             && rhs.ar_ready == ar_ready
             && rhs.r_valid == r_valid
             && rhs.r_resp == r_resp
             && rhs.r_data == r_data
             && rhs.r_last == r_last
             && rhs.r_id == r_id
             && rhs.r_user == r_user);
    }

    inline axi4_master_in_type& operator = (const axi4_master_in_type &rhs) {
        aw_ready = rhs.aw_ready;
        w_ready = rhs.w_ready;
        b_valid = rhs.b_valid;
        b_resp = rhs.b_resp;
        b_id = rhs.b_id;
        b_user = rhs.b_user;
        ar_ready = rhs.ar_ready;
        r_valid = rhs.r_valid;
        r_resp = rhs.r_resp;
        r_data = rhs.r_data;
        r_last = rhs.r_last;
        r_id = rhs.r_id;
        r_user = rhs.r_user;
        return *this;
    }

    inline friend void sc_trace(sc_trace_file *tf,
                                const axi4_master_in_type &v,
                                const std::string &NAME) {
        sc_trace(tf, v.aw_ready, NAME + "_aw_ready");
        sc_trace(tf, v.w_ready, NAME + "_w_ready");
        sc_trace(tf, v.b_valid, NAME + "_b_valid");
        sc_trace(tf, v.b_resp, NAME + "_b_resp");
        sc_trace(tf, v.b_id, NAME + "_b_id");
        sc_trace(tf, v.b_user, NAME + "_b_user");
        sc_trace(tf, v.ar_ready, NAME + "_ar_ready");
        sc_trace(tf, v.r_valid, NAME + "_r_valid");
        sc_trace(tf, v.r_resp, NAME + "_r_resp");
        sc_trace(tf, v.r_data, NAME + "_r_data");
        sc_trace(tf, v.r_last, NAME + "_r_last");
        sc_trace(tf, v.r_id, NAME + "_r_id");
        sc_trace(tf, v.r_user, NAME + "_r_user");
    }

    inline friend ostream &operator << (ostream &os,
                                        axi4_master_in_type const &v) {
        os << "("
        << v.aw_ready << ","
        << v.w_ready << ","
        << v.b_valid << ","
        << v.b_resp << ","
        << v.b_id << ","
        << v.b_user << ","
        << v.ar_ready << ","
        << v.r_valid << ","
        << v.r_resp << ","
        << v.r_data << ","
        << v.r_last << ","
        << v.r_id << ","
        << v.r_user << ")";
        return os;
    }

 public:
    bool aw_ready;
    bool w_ready;
    bool b_valid;
    sc_uint<2> b_resp;
    sc_uint<CFG_BUS_ID_BITS> b_id;
    sc_uint<CFG_BUS_USER_BITS> b_user;
    bool ar_ready;
    bool r_valid;
    sc_uint<2> r_resp;
    sc_uint<BUS_DATA_WIDTH> r_data;
    bool r_last;
    sc_uint<CFG_BUS_ID_BITS> r_id;
    sc_uint<CFG_BUS_USER_BITS> r_user;
};


class axi4_master_out_type {
 public:
    axi4_master_out_type(){}

    inline bool operator == (const axi4_master_out_type &rhs) const {
        return (rhs.aw_valid == aw_valid
            && rhs.aw_bits.addr == aw_bits.addr
            && rhs.aw_bits.len == aw_bits.len
            && rhs.aw_bits.size == aw_bits.size
            && rhs.aw_bits.burst == aw_bits.burst
            && rhs.aw_bits.lock == aw_bits.lock
            && rhs.aw_bits.cache == aw_bits.cache
            && rhs.aw_bits.prot == aw_bits.prot
            && rhs.aw_bits.qos == aw_bits.qos
            && rhs.aw_bits.region == aw_bits.region
            && rhs.aw_id == aw_id
            && rhs.aw_user == aw_user
            && rhs.w_valid == w_valid
            && rhs.w_data == w_data
            && rhs.w_last == w_last
            && rhs.w_strb == w_strb
            && rhs.w_user == w_user
            && rhs.b_ready == b_ready
            && rhs.ar_valid == ar_valid
            && rhs.ar_bits.addr == ar_bits.addr
            && rhs.ar_bits.len == ar_bits.len
            && rhs.ar_bits.size == ar_bits.size
            && rhs.ar_bits.burst == ar_bits.burst
            && rhs.ar_bits.lock == ar_bits.lock
            && rhs.ar_bits.cache == ar_bits.cache
            && rhs.ar_bits.prot == ar_bits.prot
            && rhs.ar_bits.qos == ar_bits.qos
            && rhs.ar_bits.region == ar_bits.region
            && rhs.ar_id == ar_id
            && rhs.ar_user == ar_user
            && rhs.r_ready == r_ready);
    }

    inline axi4_master_out_type& operator = (const axi4_master_out_type &rhs) {
        aw_valid = rhs.aw_valid;
        aw_bits.addr = rhs.aw_bits.addr;
        aw_bits.len = rhs.aw_bits.len;
        aw_bits.size = rhs.aw_bits.size;
        aw_bits.burst = rhs.aw_bits.burst;
        aw_bits.lock = rhs.aw_bits.lock;
        aw_bits.cache = rhs.aw_bits.cache;
        aw_bits.prot = rhs.aw_bits.prot;
        aw_bits.qos = rhs.aw_bits.qos;
        aw_bits.region = rhs.aw_bits.region;
        aw_id = rhs.aw_id;
        aw_user = rhs.aw_user;
        w_valid = rhs.w_valid;
        w_data = rhs.w_data;
        w_last = rhs.w_last;
        w_strb = rhs.w_strb;
        w_user = rhs.w_user;
        b_ready = rhs.b_ready;
        ar_valid = rhs.ar_valid;
        ar_bits.addr = rhs.ar_bits.addr;
        ar_bits.len = rhs.ar_bits.len;
        ar_bits.size = rhs.ar_bits.size;
        ar_bits.burst = rhs.ar_bits.burst;
        ar_bits.lock = rhs.ar_bits.lock;
        ar_bits.cache = rhs.ar_bits.cache;
        ar_bits.prot = rhs.ar_bits.prot;
        ar_bits.qos = rhs.ar_bits.qos;
        ar_bits.region = rhs.ar_bits.region;
        ar_id = rhs.ar_id;
        ar_user = rhs.ar_user;
        r_ready = rhs.r_ready;
        return *this;
    }

    inline friend void sc_trace(sc_trace_file *tf,
                                const axi4_master_out_type &v,
                                const std::string &NAME) {
        sc_trace(tf, v.aw_valid, NAME + "_aw_valid");
        sc_trace(tf, v.aw_bits.addr, NAME + "_aw_bits_addr");
        sc_trace(tf, v.aw_bits.len, NAME + "_aw_bits_len");
        sc_trace(tf, v.aw_bits.size, NAME + "_aw_bits_size");
        sc_trace(tf, v.aw_bits.burst, NAME + "_aw_bits_burst");
        sc_trace(tf, v.aw_bits.lock, NAME + "_aw_bits_lock");
        sc_trace(tf, v.aw_bits.cache, NAME + "_aw_bits_cache");
        sc_trace(tf, v.aw_bits.prot, NAME + "_aw_bits_prot");
        sc_trace(tf, v.aw_bits.qos, NAME + "_aw_bits_qos");
        sc_trace(tf, v.aw_bits.region, NAME + "_aw_bits_region");
        sc_trace(tf, v.aw_id, NAME + "_aw_id");
        sc_trace(tf, v.aw_user, NAME + "_aw_user");
        sc_trace(tf, v.w_valid, NAME + "_w_valid");
        sc_trace(tf, v.w_data, NAME + "_w_data");
        sc_trace(tf, v.w_last, NAME + "_w_last");
        sc_trace(tf, v.w_strb, NAME + "_w_strb");
        sc_trace(tf, v.w_user, NAME + "_w_user");
        sc_trace(tf, v.b_ready, NAME + "_b_ready");
        sc_trace(tf, v.ar_valid, NAME + "_ar_valid");
        sc_trace(tf, v.ar_bits.addr, NAME + "_ar_bits_addr");
        sc_trace(tf, v.ar_bits.len, NAME + "_ar_bits_len");
        sc_trace(tf, v.ar_bits.size, NAME + "_ar_bits_size");
        sc_trace(tf, v.ar_bits.burst, NAME + "_ar_bits_burst");
        sc_trace(tf, v.ar_bits.lock, NAME + "_ar_bits_lock");
        sc_trace(tf, v.ar_bits.cache, NAME + "_ar_bits_cache");
        sc_trace(tf, v.ar_bits.prot, NAME + "_ar_bits_prot");
        sc_trace(tf, v.ar_bits.qos, NAME + "_ar_bits_qos");
        sc_trace(tf, v.ar_bits.region, NAME + "_ar_bits_region");
        sc_trace(tf, v.ar_id, NAME + "_ar_id");
        sc_trace(tf, v.ar_user, NAME + "_ar_user");
        sc_trace(tf, v.r_ready, NAME + "_r_ready");
    }

    inline friend ostream &operator << (ostream &os,
                                        axi4_master_out_type const &v) {
        os << "("
        << v.aw_valid << ","
        << v.aw_bits.addr << ","
        << v.aw_bits.len << ","
        << v.aw_bits.size << ","
        << v.aw_bits.burst << ","
        << v.aw_bits.lock << ","
        << v.aw_bits.cache << ","
        << v.aw_bits.prot << ","
        << v.aw_bits.qos << ","
        << v.aw_bits.region << ","
        << v.aw_id << ","
        << v.aw_user << ","
        << v.w_valid << ","
        << v.w_data << ","
        << v.w_last << ","
        << v.w_strb << ","
        << v.w_user << ","
        << v.b_ready << ","
        << v.ar_valid << ","
        << v.ar_bits.addr << ","
        << v.ar_bits.len << ","
        << v.ar_bits.size << ","
        << v.ar_bits.burst << ","
        << v.ar_bits.lock << ","
        << v.ar_bits.cache << ","
        << v.ar_bits.prot << ","
        << v.ar_bits.qos << ","
        << v.ar_bits.region << ","
        << v.ar_id << ","
        << v.ar_user << ","
        << v.r_ready
        << ")";
        return os;
    }

 public:
    bool aw_valid;
    axi4_meta_type aw_bits;
    sc_uint<CFG_BUS_ID_BITS> aw_id;
    sc_uint<CFG_BUS_USER_BITS> aw_user;
    bool w_valid;
    sc_uint<BUS_DATA_WIDTH> w_data;
    bool w_last;
    sc_uint<BUS_DATA_BYTES> w_strb;
    sc_uint<CFG_BUS_USER_BITS> w_user;
    bool b_ready;
    bool ar_valid;
    axi4_meta_type ar_bits;
    sc_uint<CFG_BUS_ID_BITS> ar_id;
    sc_uint<CFG_BUS_USER_BITS> ar_user;
    bool r_ready;
};

static const axi4_master_in_type axi4_master_in_none;

}  // namespace debugger

