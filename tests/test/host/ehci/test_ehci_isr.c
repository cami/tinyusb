/*
 * test_ehci_isr.c
 *
 *  Created on: Mar 12, 2013
 *      Author: hathach
 */

/*
 * Software License Agreement (BSD License)
 * Copyright (c) 2012, hathach (tinyusb.net)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the tiny usb stack.
 */

#include "unity.h"
#include "tusb_option.h"
#include "errors.h"
#include "binary.h"

#include "hal.h"
#include "mock_osal.h"
#include "hcd.h"
#include "mock_usbh_hcd.h"
#include "ehci.h"
#include "ehci_controller.h"

usbh_device_info_t usbh_devices[TUSB_CFG_HOST_DEVICE_MAX+1];

uint8_t hostid;
ehci_registers_t * regs;
void setUp(void)
{
  memclr_(&lpc_usb0, sizeof(LPC_USB0_Type));
  memclr_(&lpc_usb1, sizeof(LPC_USB1_Type));

  hostid = RANDOM(CONTROLLER_HOST_NUMBER) + TEST_CONTROLLER_HOST_START_INDEX;
  regs = get_operational_register(hostid);

  hcd_init();
  regs->usb_sts = 0; // hcd_init clear usb_sts by writing 1s
}

void tearDown(void)
{
}

void test_isr_device_connect_highspeed(void)
{
  ehci_controller_device_plug(hostid, TUSB_SPEED_HIGH);

  usbh_device_plugged_isr_Expect(hostid, TUSB_SPEED_HIGH);

  //------------- Code Under Test -------------//
  hcd_isr(hostid);
}

void test_isr_device_connect_fullspeed(void)
{
  ehci_controller_device_plug(hostid, TUSB_SPEED_FULL);

  usbh_device_plugged_isr_Expect(hostid, TUSB_SPEED_FULL);

  //------------- Code Under Test -------------//
  hcd_isr(hostid);
}

void test_isr_device_connect_slowspeed(void)
{
  ehci_controller_device_plug(hostid, TUSB_SPEED_LOW);

  usbh_device_plugged_isr_Expect(hostid, TUSB_SPEED_LOW);

  //------------- Code Under Test -------------//
  hcd_isr(hostid);
}

void test_isr_device_disconnect(void)
{
  ehci_controller_device_unplug(hostid);
  usbh_device_unplugged_isr_Expect(hostid);

  //------------- Code Under Test -------------//
  hcd_isr(hostid);

  TEST_ASSERT(regs->usb_cmd_bit.advacne_async);
}