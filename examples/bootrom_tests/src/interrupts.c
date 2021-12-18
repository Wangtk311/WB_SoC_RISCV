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

#include <string.h>
#include "axi_maps.h"
#include "encoding.h"
#include "fw_api.h"

typedef union csr_mcause_type {
    struct bits_type {
        uint64_t code   : 63;   // 11 - Machine external interrupt
        uint64_t irq    : 1;
    } bits;
    uint64_t value;
} csr_mcause_type;

/** Fatal Error Handler can be used to transmit dump image
 *  or trigger watchdog
 */
void fatal_error() {
    printf_uart("fatal_error()\r\n");
    while (1) {}
}

uint32_t plic_claim(int ctxid) {
    plic_map *p = (plic_map *)ADDR_BUS0_XSLV_PLIC;
    return p->ctx_prio[ctxid].claim_complete;
}

void plic_complete(int ctxid, int irqid) {
    plic_map *p = (plic_map *)ADDR_BUS0_XSLV_PLIC;
    p->ctx_prio[ctxid].claim_complete = irqid;
}

long interrupt_handler_c(long cause, long epc, long long regs[32]) {
    /**
     * Pending interrupt bit is cleared in the crt.S file by calling:
     *      csrc mip, MIP_MSIP
     * If we woudn't do it the interrupt handler will be called infinitly
     *
     * Rise interrupt from the software maybe done sending a self-IPI:
     *      csrwi mipi, 0
     */
    irqctrl_map *p_irqctrl = (irqctrl_map *)ADDR_BUS0_XSLV_IRQCTRL;
    IRQ_HANDLER *isr_table = (IRQ_HANDLER *)p_irqctrl->isr_table;
    uint32_t pending;
    csr_mcause_type mcause;

    mcause.value = cause;
    p_irqctrl->dbg_cause = cause;
    p_irqctrl->dbg_epc = epc;

    p_irqctrl->irq_lock = 1;
    pending = p_irqctrl->irq_pending;
    p_irqctrl->irq_clear = pending;
    p_irqctrl->irq_lock = 0;

    if (mcause.bits.irq == 0x1 && mcause.bits.code == 11) {
        plic_map *p = (plic_map *)ADDR_BUS0_XSLV_PLIC;
        uint32_t irqid = plic_claim(CTX_CPU0_M_MODE);
        if (isr_table[irqid]) {
            isr_table[irqid]();
        } else {
            print_uart("err: no_handler\r\n", 17);
        }
        plic_complete(CTX_CPU0_M_MODE, irqid);
    } else {
       print_uart("mcause:", 7);
       print_uart_hex(cause);
       print_uart(",mepc:", 6);
       print_uart_hex(epc);
       print_uart("\r\n", 2);
       /// Exception trap
       led_set(0xF0);
       while (1) {}
    }

    return epc;
}
