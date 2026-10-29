/*
 * Copyright (C) 2023-2024 Texas Instruments Incorporated
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "main.h"
#include "drivers/bootloader.h"

#define WAIT_FOR_R5_UART_LOGS   (50U)
#define WAIT_FOR_VPP_RAMP       (1U)

/* call this API to stop the booting process and spin, do that you can connect
 * debugger, load symbols and then make the 'loop' variable as 0 to continue execution
 * with debugger connected.
 */
void loop_forever(void)
{
    volatile uint32_t loop = 1U;
    while(loop == 1U)
    {
        /*Let the cpu spin*/
    }
}

int32_t main(void)
{
    int32_t status = SystemP_SUCCESS;

    status = Bootloader_socWaitForFWBoot();
	if (status != SystemP_SUCCESS)
	{
		DebugP_assertNoLog(0);
	}

    System_init();
    Drivers_open();

    /* 
     * Conservative delay of 50 usec added to avoid partial UART logs
    */
    ClockP_usleep(WAIT_FOR_R5_UART_LOGS);

    DebugP_log("\r\n");
    DebugP_log("Starting Keywriting\r\n");

	keywriter_setVpp();
    DebugP_log("Enabled VPP\r\n");

    /* 
     * Conservatively wait for 1 second to make sure the VPP signal ramp is fully done 
     * before starting the keywriting operation.
     * This is for the TI EVM.
     * For custom boards, the delay can be optimized as per the evaluation done for the 
     * time it takes for VPP to stable after enabling it.
    */
    ClockP_sleep(WAIT_FOR_VPP_RAMP);

    status = Sciclient_getVersionCheck(1);

    if (status == SystemP_SUCCESS)
    {
        keywriter_processKeyConfig();
    }

    Drivers_close();
    System_deinit();

	loop_forever();

    return 0;
}


