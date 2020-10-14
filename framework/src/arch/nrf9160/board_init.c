// Note: this originally nRF5SDK160098a08e2/examples/peripheral/serial/main.c
/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define APP_UTIL_PLATFORM_H__

#include "nrf_gpio.h"
#include "nrf_uarte.h"
#include "nrfx_nvmc.h"

#define CONSOLE_BASE        NRF_UARTE0
#define CONSOLE_BAUD        NRF_UARTE_BAUDRATE_115200
#define CONSOLE_PARITY      NRF_UARTE_PARITY_EXCLUDED
#define CONSOLE_FLOWCTRL    NRF_UARTE_HWFC_DISABLED

#define CONSOLE_UARTE_CONFIG_PSEL_TXD   29
#define CONSOLE_UARTE_CONFIG_PSEL_RXD   28

#define MEAS_START 0
#define MEAS_STOP 1

#define CACHE_PROFILING 1

void uarte_putc(char c)
{
  uint32_t intmask = CONSOLE_BASE->INTEN;
  nrf_uarte_int_disable(CONSOLE_BASE, intmask);
  nrf_uarte_event_clear(CONSOLE_BASE, NRF_UARTE_EVENT_ENDTX);
  nrf_uarte_event_clear(CONSOLE_BASE, NRF_UARTE_EVENT_TXSTOPPED);
  nrf_uarte_tx_buffer_set(CONSOLE_BASE, (uint8_t*)&c, sizeof(c));
  nrf_uarte_task_trigger(CONSOLE_BASE, NRF_UARTE_TASK_STARTTX);

  bool endtx;
  bool txstopped;

  do {
    endtx     = nrf_uarte_event_check(CONSOLE_BASE, NRF_UARTE_EVENT_ENDTX);
    txstopped = nrf_uarte_event_check(CONSOLE_BASE, NRF_UARTE_EVENT_TXSTOPPED);
  } while((endtx == false) && (txstopped == false));

  nrf_uarte_int_enable(NRF_UARTE0, intmask);
}

static void uarte_init(void)
{
  nrf_gpio_cfg(CONSOLE_UARTE_CONFIG_PSEL_RXD,
               NRF_GPIO_PIN_DIR_INPUT,
               NRF_GPIO_PIN_INPUT_CONNECT,
               NRF_GPIO_PIN_PULLUP,
               NRF_GPIO_PIN_S0S1,
               NRF_GPIO_PIN_NOSENSE);

  nrf_gpio_cfg(CONSOLE_UARTE_CONFIG_PSEL_TXD,
               NRF_GPIO_PIN_DIR_OUTPUT,
               NRF_GPIO_PIN_INPUT_DISCONNECT,
               NRF_GPIO_PIN_PULLUP,
               NRF_GPIO_PIN_S0S1,
               NRF_GPIO_PIN_NOSENSE);

  nrf_uarte_baudrate_set(CONSOLE_BASE, (nrf_uarte_baudrate_t) CONSOLE_BAUD);
  nrf_uarte_configure(CONSOLE_BASE, (nrf_uarte_parity_t) CONSOLE_PARITY,
                      (nrf_uarte_hwfc_t) CONSOLE_FLOWCTRL);
  nrf_uarte_txrx_pins_set(CONSOLE_BASE, CONSOLE_UARTE_CONFIG_PSEL_TXD, CONSOLE_UARTE_CONFIG_PSEL_RXD);
  nrf_uarte_event_clear(CONSOLE_BASE, NRF_UARTE_EVENT_ENDTX);
  nrf_uarte_event_clear(CONSOLE_BASE, NRF_UARTE_EVENT_TXSTOPPED);
  nrf_uarte_enable(CONSOLE_BASE);

}

void cache_enable(void)
{
  nrfx_nvmc_icache_enable();
}

void icache_reset_misses(){
	nrf_nvmc_icache_hit_miss_reset(NRF_NVMC);
}
unsigned int icache_get_misses(){
	return nrf_nvmc_icache_miss_get(NRF_NVMC);
}


int board_init(void)
{
  int main(void);

  uarte_init();

  // Enable ARM DWT->CYCCNT reading
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  
  print("Pairing benchmark with optimized Cortex-M33 64-bit multiplication. For BN254.\n");
  
#if CACHE_PROFILING
  cache_enable();
  print("Benchmark with instruction cache & profiling enabled.\n");

  if(!main()){
	  print("Done!\n");	  
  }
#else
  print("Benchmark without instrction cache & profiling.\n");
  
  if(!main()){
	  print("Done!\n");	  
  } 
#endif
  
  while(1){
	  // Execution ends up here if succesful..
  }
  
  return 0;
  
}

/** @} */
