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

#include <stdint.h>
#include <drivers/sciclient.h>
#include <drivers/sciclient/include/tisci/security/tisci_keywriter.h>
#include "../../keywr_bin/am62x/keycert.h"
#include "main.h"

/* Keywriter certificate */
const uint8_t gKeyCert[KEYCERT_SIZE_IN_BYTES] __attribute__((aligned(128)))
    = KEYCERT;

void keywriter_processKeyConfig(void)
{
    int32_t status = SystemP_SUCCESS;
    Sciclient_ReqPrm_t reqParam;
    Sciclient_RespPrm_t respParam;
    struct tisci_msg_keywriter_req request;
    struct tisci_msg_keywriter_resp response;

    request.image_addr_lo     = ((uint32_t) gKeyCert & 0xFFFFFFFFU);
    request.image_addr_hi     = ((uint32_t)(((uint64_t) gKeyCert) >> 32) & 0xFFFFFFFFU);

    reqParam.messageType      = (uint16_t) TISCI_MSG_KEY_WRITER;
    reqParam.flags            = (uint32_t) TISCI_MSG_FLAG_AOP;
    reqParam.pReqPayload      = (const uint8_t *) &request;
    reqParam.reqPayloadSize   = (uint32_t) sizeof (request);
    reqParam.timeout          = (uint32_t) SystemP_WAIT_FOREVER;

    /* Populated by the API */
    respParam.flags           = 0U;
    respParam.pRespPayload    = (uint8_t *) &response;
    respParam.respPayloadSize = (uint32_t) sizeof (response);

	DebugP_log("keys Certificate found: 0x%x \r\n", (uint32_t) gKeyCert);

    status = Sciclient_service(&reqParam, &respParam);

	DebugP_log("Keywriter Debug Response:0x%x \r\n", (uint32_t) response.debug_response);

	if (( status == SystemP_SUCCESS ) && ( response.debug_response == 0x0U ))
	{
		DebugP_log("Success Programming Keys \r\n");
	}
	else
	{
		DebugP_log("Error occured... \r\n");
	}

    if (!((status == SystemP_SUCCESS) && ((respParam.flags & TISCI_MSG_FLAG_ACK) == TISCI_MSG_FLAG_ACK)))
    {
        DebugP_assertNoLog(0);
    }
}





