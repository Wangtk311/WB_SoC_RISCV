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

#include <string.h>
#include "axi_maps.h"
#include "encoding.h"
#include "fw_api.h"

void isr_uart0_tx();

void allocate_exception_table(void);
void allocate_interrupt_table(void);
void test_l2coherence(void);
void test_plic(void);
void test_fpu(void);
void test_swirq(void);
void test_mtimer(void);
void test_missaccess(void);
void test_stackprotect(void);
void test_spiflash(uint64_t bar);
void test_gnss_ss(uint64_t bar);
int test_pmp();
int test_mmu();
int test_ddr();
int test_spi();
void print_pnp(void);
int hwthread1(void);
int hwthread2(void);
int hwthread3(void);


int main() {
    uint32_t cfg;
    pnp_map *pnp = (pnp_map *)ADDR_BUS0_XSLV_PNP;
    uart_map *uart = (uart_map *)ADDR_BUS0_XSLV_UART0;
    gpio_map *gpio = (gpio_map *)ADDR_BUS0_XSLV_GPIO;
    uint64_t bar;
    uint32_t cpu_max;

    switch (fw_get_cpuid()) {
    case 0:
        break;
    case 1:
        hwthread1();
        break;
    case 2:
        hwthread2();
        break;
    case 3:
        hwthread3();
        break;
    default:
        while (1) {}
    }

    pnp->fwid = 0x20220116;
    gpio->input_en = 0x000f;
    gpio->output_en = 0xfff0;
    fw_malloc_init();
    
    allocate_exception_table();
    allocate_interrupt_table();

    uart_isr_init();   // enable printf_uart function and Tx irq=1
 
    led_set(0x01);

#if 1
    test_spi();
#endif

    cpu_max = pnp->cfg >> 28;

    printf_uart("HARTID . . . . .%d\r\n", fw_get_cpuid());
    printf_uart("HARTS. . . . . .%d\r\n", cpu_max);
    printf_uart("PLIC_IRQS  . . .%d\r\n", (pnp->cfg & 0xFF));
    printf_uart("HWID . . . . . .0x%08x\r\n", pnp->hwid);
    printf_uart("FWID . . . . . .0x%08x\r\n", pnp->fwid);

    led_set(0x02);

    test_plic();
    test_mtimer();
    test_swirq();
    test_l2coherence();
    test_pmp();
    test_mmu();

    led_set(0x03);
    test_fpu();

    led_set(0x04);
    test_missaccess();

    led_set(0x05);
    test_stackprotect();


    led_set(0x55);
    print_pnp();

    led_set(0x1F);

    test_ddr();

    // TODO: implement test console
    clint_map *clint = (clint_map *)ADDR_BUS0_XSLV_CLINT;
    uint64_t t_start = clint->mtime;

    while (1) {
        // temporary put it here, while PLIC not fully ready
        isr_uart0_tx();

        // 1 sec output
        if ((clint->mtime - t_start) > SYS_HZ) {
            t_start = clint->mtime;

            // GPIO[11:8] = output LED[7:4] switching ***1
            // GPIO[7:4]  = output LED[3:0] status of DIP
            // GPIO[3:0]  = input DIP
            uint32_t outval = gpio->output_val >> 8;
            outval = (outval << 1) & 0xF;
            if (outval == 0) {
                outval = 0x1;
            }
            outval = (outval << 8) | (gpio->input_val << 4);
            gpio->output_val = outval;
        }
    }

    // Run BSL and Linux from DDR:
    set_csr(mstatus, MSTATUS_MPP_M);          // run bbl-q and riscv-tests in machine mode
    write_csr(mepc, ADDR_BUS0_XSLV_DDR);      // jump to ddr (bbl-q should be init)

    // a0 = hart id
    // a1 = fdt header
    __asm__("csrr a0, mhartid");
    __asm__("la a1, dtb_start");
    __asm__("mret");

#if 0
    // NEVER REACH THIS POINT
    // jump to entry point in SRAM = 0x10000000
    //     'meps' - Machine Exception Program Coutner
    __asm__("lui t0, 0x10000");
    __asm__("csrw mepc, t0");
    __asm__("mret");
#endif

    return 0;
}
