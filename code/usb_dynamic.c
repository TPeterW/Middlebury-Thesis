
/*-
 * Copyright (c) 2008 Hans Petter Selasky. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef USB_GLOBAL_INCLUDE_FILE
#include USB_GLOBAL_INCLUDE_FILE
#else
#include <sys/stdint.h>
#include <sys/stddef.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/module.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>
#include <sys/sysctl.h>
#include <sys/sx.h>
#include <sys/unistd.h>
#include <sys/callout.h>
#include <sys/malloc.h>
#include <sys/priv.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_device.h>
#include <dev/usb/usb_dynamic.h>
#include <dev/usb/usb_request.h>
#include <dev/usb/usb_tpw_probe_declare.h>
#endif			


static usb_handle_req_t usb_temp_get_desc_w;
static usb_temp_setup_by_index_t usb_temp_setup_by_index_w;
#if USB_HAVE_COMPAT_LINUX
static usb_linux_free_device_t usb_linux_free_device_w;
#endif
static usb_temp_unsetup_t usb_temp_unsetup_w;
static usb_test_quirk_t usb_test_quirk_w;
static usb_quirk_ioctl_t usb_quirk_ioctl_w;


usb_handle_req_t *usb_temp_get_desc_p = &usb_temp_get_desc_w;
usb_temp_setup_by_index_t *usb_temp_setup_by_index_p = &usb_temp_setup_by_index_w;
#if USB_HAVE_COMPAT_LINUX
usb_linux_free_device_t *usb_linux_free_device_p = &usb_linux_free_device_w;
#endif
usb_temp_unsetup_t *usb_temp_unsetup_p = &usb_temp_unsetup_w;
usb_test_quirk_t *usb_test_quirk_p = &usb_test_quirk_w;
usb_quirk_ioctl_t *usb_quirk_ioctl_p = &usb_quirk_ioctl_w;
devclass_t usb_devclass_ptr;

static usb_error_t
usb_temp_setup_by_index_w(struct usb_device *udev, uint16_t index)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_setup_by_index_w, entry);
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_setup_by_index_w, return);
	return (USB_ERR_INVAL);
}

static uint8_t
usb_test_quirk_w(const struct usbd_lookup_info *info, uint16_t quirk)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_test_quirk_w, entry);
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_test_quirk_w, return);
	return (0);			
}

static int
usb_quirk_ioctl_w(unsigned long cmd, caddr_t data, int fflag, struct thread *td)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_quirk_ioctl_w, entry);
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_quirk_ioctl_w, return);
	return (ENOIOCTL);
}

static usb_error_t
usb_temp_get_desc_w(struct usb_device *udev, struct usb_device_request *req, const void **pPtr, uint16_t *pLength)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_get_desc_w, entry);
	
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_get_desc_w, return);
	return (USB_ERR_STALLED);
}

static void
usb_temp_unsetup_w(struct usb_device *udev)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_unsetup_w, entry);
	usbd_free_config_desc(udev, udev->usb_template_ptr);
	udev->usb_template_ptr = NULL;
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_unsetup_w, return);
}

#if USB_HAVE_COMPAT_LINUX
static void
usb_linux_free_device_w(struct usb_device *udev)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_linux_free_device_w, entry);
	
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_linux_free_device_w, return);
}
#endif

void
usb_quirk_unload(void *arg)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_quirk_unload, entry);
	

	usb_test_quirk_p = &usb_test_quirk_w;
	usb_quirk_ioctl_p = &usb_quirk_ioctl_w;

	

	

	pause("WAIT", hz);
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_quirk_unload, return);
}

void
usb_temp_unload(void *arg)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_unload, entry);
	

	usb_temp_get_desc_p = &usb_temp_get_desc_w;
	usb_temp_setup_by_index_p = &usb_temp_setup_by_index_w;
	usb_temp_unsetup_p = &usb_temp_unsetup_w;

	

	

	pause("WAIT", hz);
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_temp_unload, return);
}

void
usb_bus_unload(void *arg)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_bus_unload, entry);
	

	usb_devclass_ptr = NULL;

	

	

	pause("WAIT", hz);
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_bus_unload, return);
}

#if USB_HAVE_COMPAT_LINUX
void
usb_linux_unload(void *arg)
{
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_linux_unload, entry);
	

	usb_linux_free_device_p = &usb_linux_free_device_w;
  
	

	

	pause("WAIT", hz);
	SDT_PROBE0(tpw, kernel, usb_dynamic_usb_linux_unload, return);
}
#endif
