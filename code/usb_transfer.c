
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
#include <sys/proc.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdi_util.h>

#define	USB_DEBUG_VAR usb_debug

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_busdma.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_transfer.h>
#include <dev/usb/usb_device.h>
#include <dev/usb/usb_debug.h>
#include <dev/usb/usb_util.h>

#include <dev/usb/usb_controller.h>
#include <dev/usb/usb_bus.h>
#include <dev/usb/usb_pf.h>
#include <dev/usb/usb_tpw_probe_declare.h>
#endif			

struct usb_std_packet_size {
	struct {
		uint16_t min;		
		uint16_t max;		
	}	range;

	uint16_t fixed[4];
};

static usb_callback_t usb_request_callback;

static const struct usb_config usb_control_ep_cfg[USB_CTRL_XFER_MAX] = {

	

	[0] = {
		.type = UE_CONTROL,
		.endpoint = 0x00,	
		.direction = UE_DIR_ANY,
		.bufsize = USB_EP0_BUFSIZE,	
		.flags = {.proxy_buffer = 1,},
		.callback = &usb_request_callback,
		.usb_mode = USB_MODE_DUAL,	
	},

	

	[1] = {
		.type = UE_CONTROL,
		.endpoint = 0x00,	
		.direction = UE_DIR_ANY,
		.bufsize = sizeof(struct usb_device_request),
		.callback = &usb_do_clear_stall_callback,
		.timeout = 1000,	
		.interval = 50,	
		.usb_mode = USB_MODE_HOST,
	},
};



static void	usbd_update_max_frame_size(struct usb_xfer *);
static void	usbd_transfer_unsetup_sub(struct usb_xfer_root *, uint8_t);
static void	usbd_control_transfer_init(struct usb_xfer *);
static int	usbd_setup_ctrl_transfer(struct usb_xfer *);
static void	usb_callback_proc(struct usb_proc_msg *);
static void	usbd_callback_ss_done_defer(struct usb_xfer *);
static void	usbd_callback_wrapper(struct usb_xfer_queue *);
static void	usbd_transfer_start_cb(void *);
static uint8_t	usbd_callback_wrapper_sub(struct usb_xfer *);
static void	usbd_get_std_packet_size(struct usb_std_packet_size *ptr, 
		    uint8_t type, enum usb_dev_speed speed);

/*------------------------------------------------------------------------*
 *	usb_request_callback
 *------------------------------------------------------------------------*/
static void
usb_request_callback(struct usb_xfer *xfer, usb_error_t error)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_request_callback, entry);
	if (xfer->flags_int.usb_mode == USB_MODE_DEVICE)
{
		usb_handle_request_callback(xfer, error);
}
	else
		usbd_do_request_callback(xfer, error);
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_request_callback, return);
}

/*------------------------------------------------------------------------*
 *	usbd_update_max_frame_size
 *
 * This function updates the maximum frame size, hence high speed USB
 * can transfer multiple consecutive packets.
 *------------------------------------------------------------------------*/
static void
usbd_update_max_frame_size(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_update_max_frame_size, entry);
	
	
	

	xfer->max_frame_size = xfer->max_packet_size * xfer->max_packet_count;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_update_max_frame_size, return);
}

/*------------------------------------------------------------------------*
 *	usbd_get_dma_delay
 *
 * The following function is called when we need to
 * synchronize with DMA hardware.
 *
 * Returns:
 *    0: no DMA delay required
 * Else: milliseconds of DMA delay
 *------------------------------------------------------------------------*/
usb_timeout_t
usbd_get_dma_delay(struct usb_device *udev)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_get_dma_delay, entry);
	const struct usb_bus_methods *mtod;
	uint32_t temp;

	mtod = udev->bus->methods;
	temp = 0;

	if (mtod->get_dma_delay) {
		(mtod->get_dma_delay) (udev, &temp);
		/*
		 * Round up and convert to milliseconds. Note that we use
		 * 1024 milliseconds per second. to save a division.
		 */
		temp += 0x3FF;
		temp /= 0x400;
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_get_dma_delay, return);
	return (temp);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_setup_sub_malloc
 *
 * This function will allocate one or more DMA'able memory chunks
 * according to "size", "align" and "count" arguments. "ppc" is
 * pointed to a linear array of USB page caches afterwards.
 *
 * If the "align" argument is equal to "1" a non-contiguous allocation
 * can happen. Else if the "align" argument is greater than "1", the
 * allocation will always be contiguous in memory.
 *
 * Returns:
 *    0: Success
 * Else: Failure
 *------------------------------------------------------------------------*/
#if USB_HAVE_BUSDMA
uint8_t
usbd_transfer_setup_sub_malloc(struct usb_setup_params *parm,
    struct usb_page_cache **ppc, usb_size_t size, usb_size_t align,
    usb_size_t count)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, entry);
	struct usb_page_cache *pc;
	struct usb_page *pg;
	void *buf;
	usb_size_t n_dma_pc;
	usb_size_t n_dma_pg;
	usb_size_t n_obj;
	usb_size_t x;
	usb_size_t y;
	usb_size_t r;
	usb_size_t z;

	USB_ASSERT(align > 0, ("Invalid alignment, 0x%08x\n",
	    align));
	USB_ASSERT(size > 0, ("Invalid size = 0\n"));

	if (count == 0) {
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
		return (0);		
	}
	/*
	 * Make sure that the size is aligned properly.
	 */
	size = -((-size) & (-align));

	/*
	 * Try multi-allocation chunks to reduce the number of DMA
	 * allocations, hence DMA allocations are slow.
	 */
	if (align == 1) {
		
		n_dma_pc = count;
		n_dma_pg = (2 + (size / USB_PAGE_SIZE));
		n_obj = 1;
	} else if (size >= USB_PAGE_SIZE) {
		n_dma_pc = count;
		n_dma_pg = 1;
		n_obj = 1;
	} else {
		
#ifdef USB_DMA_SINGLE_ALLOC
		n_obj = 1;
#else
		n_obj = (USB_PAGE_SIZE / size);
#endif
		/*
		 * Compute number of DMA chunks, rounded up
		 * to nearest one:
		 */
		n_dma_pc = howmany(count, n_obj);
		n_dma_pg = 1;
	}

	/*
	 * DMA memory is allocated once, but mapped twice. That's why
	 * there is one list for auto-free and another list for
	 * non-auto-free which only holds the mapping and not the
	 * allocation.
	 */
	if (parm->buf == NULL) {
		
		parm->dma_page_ptr += n_dma_pc * n_dma_pg;
		parm->dma_page_cache_ptr += n_dma_pc;

		
		parm->dma_page_ptr += count * n_dma_pg;
		parm->xfer_page_cache_ptr += count;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
		return (0);
	}
	for (x = 0; x != n_dma_pc; x++) {
		
		parm->dma_page_cache_ptr[x].tag_parent =
		    &parm->curr_xfer->xroot->dma_parent_tag;
	}
	for (x = 0; x != count; x++) {
		
		parm->xfer_page_cache_ptr[x].tag_parent =
		    &parm->curr_xfer->xroot->dma_parent_tag;
	}

	if (ppc != NULL) {
		if (n_obj != 1)
{
			*ppc = parm->xfer_page_cache_ptr;
}
		else
			*ppc = parm->dma_page_cache_ptr;
	}
	r = count;			
	z = n_obj * size;		
	pc = parm->xfer_page_cache_ptr;
	pg = parm->dma_page_ptr;

	if (n_obj == 1) {
	    /*
	     * Avoid mapping memory twice if only a single object
	     * should be allocated per page cache:
	     */
	    for (x = 0; x != n_dma_pc; x++) {
		if (usb_pc_alloc_mem(parm->dma_page_cache_ptr,
		    pg, z, align)) {
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
			return (1);	
		}
		
		parm->dma_page_cache_ptr++;
		pg += n_dma_pg;
	    }
	} else {
	    for (x = 0; x != n_dma_pc; x++) {

		if (r < n_obj) {
			
			z = r * size;
			n_obj = r;
		}
		if (usb_pc_alloc_mem(parm->dma_page_cache_ptr,
		    pg, z, align)) {
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
			return (1);	
		}
		
		buf = parm->dma_page_cache_ptr->buffer;
		
		parm->dma_page_cache_ptr++;
		pg += n_dma_pg;

		for (y = 0; (y != n_obj); y++, r--, pc++, pg += n_dma_pg) {

			
			if (usb_pc_dmamap_create(pc, size)) {
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
				return (1);	
			}
			pc->buffer = USB_ADD_BYTES(buf, y * size);
			pc->page_start = pg;

			mtx_lock(pc->tag_parent->mtx);
			if (usb_pc_load_mem(pc, size, 1 /* synchronous */ )) {
				mtx_unlock(pc->tag_parent->mtx);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
				return (1);	
			}
			mtx_unlock(pc->tag_parent->mtx);
		}
	    }
	}

	parm->xfer_page_cache_ptr = pc;
	parm->dma_page_ptr = pg;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
	return (0);
}
#endif

/*------------------------------------------------------------------------*
 *	usbd_transfer_setup_sub - transfer setup subroutine
 *
 * This function must be called from the "xfer_setup" callback of the
 * USB Host or Device controller driver when setting up an USB
 * transfer. This function will setup correct packet sizes, buffer
 * sizes, flags and more, that are stored in the "usb_xfer"
 * structure.
 *------------------------------------------------------------------------*/
void
usbd_transfer_setup_sub(struct usb_setup_params *parm)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub, entry);
	enum {
		REQ_SIZE = 8,
		MIN_PKT = 8,
	};
	struct usb_xfer *xfer = parm->curr_xfer;
	const struct usb_config *setup = parm->curr_setup;
	struct usb_endpoint_ss_comp_descriptor *ecomp;
	struct usb_endpoint_descriptor *edesc;
	struct usb_std_packet_size std_size;
	usb_frcount_t n_frlengths;
	usb_frcount_t n_frbuffers;
	usb_frcount_t x;
	uint16_t maxp_old;
	uint8_t type;
	uint8_t zmps;

	/*
	 * Sanity check. The following parameters must be initialized before
	 * calling this function.
	 */
	if ((parm->hc_max_packet_size == 0) ||
	    (parm->hc_max_packet_count == 0) ||
	    (parm->hc_max_frame_size == 0)) {
		parm->err = USB_ERR_INVAL;
		goto done;
	}
	edesc = xfer->endpoint->edesc;
	ecomp = xfer->endpoint->ecomp;

	type = (edesc->bmAttributes & UE_XFERTYPE);

	xfer->flags = setup->flags;
	xfer->nframes = setup->frames;
	xfer->timeout = setup->timeout;
	xfer->callback = setup->callback;
	xfer->interval = setup->interval;
	xfer->endpointno = edesc->bEndpointAddress;
	xfer->max_packet_size = UGETW(edesc->wMaxPacketSize);
	xfer->max_packet_count = 1;
	
	xfer->flags_int.usb_mode = parm->udev->flags.usb_mode;

	parm->bufsize = setup->bufsize;

	switch (parm->speed) {
	case USB_SPEED_HIGH:
		switch (type) {
		case UE_ISOCHRONOUS:
		case UE_INTERRUPT:
			xfer->max_packet_count +=
			    (xfer->max_packet_size >> 11) & 3;

			
			if (xfer->max_packet_count > 3)
{
				xfer->max_packet_count = 3;
}
			break;
		default:
			break;
		}
		xfer->max_packet_size &= 0x7FF;
		break;
	case USB_SPEED_SUPER:
		xfer->max_packet_count += (xfer->max_packet_size >> 11) & 3;

		if (ecomp != NULL)
{
			xfer->max_packet_count += ecomp->bMaxBurst;
}

		if ((xfer->max_packet_count == 0) || 
		    (xfer->max_packet_count > 16))
{
			xfer->max_packet_count = 16;
}

		switch (type) {
		case UE_CONTROL:
			xfer->max_packet_count = 1;
			break;
		case UE_ISOCHRONOUS:
			if (ecomp != NULL) {
				uint8_t mult;

				mult = UE_GET_SS_ISO_MULT(
				    ecomp->bmAttributes) + 1;
				if (mult > 3)
{
					mult = 3;
}

				xfer->max_packet_count *= mult;
			}
			break;
		default:
			break;
		}
		xfer->max_packet_size &= 0x7FF;
		break;
	default:
		break;
	}
	

	if (xfer->max_packet_count > parm->hc_max_packet_count) {
		xfer->max_packet_count = parm->hc_max_packet_count;
	}

	

	maxp_old = xfer->max_packet_size;

	

	if ((xfer->max_packet_size > parm->hc_max_packet_size) ||
	    (xfer->max_packet_size == 0)) {
		xfer->max_packet_size = parm->hc_max_packet_size;
	}
	

	usbd_get_std_packet_size(&std_size, type, parm->speed);

	if (std_size.range.min || std_size.range.max) {

		if (xfer->max_packet_size < std_size.range.min) {
			xfer->max_packet_size = std_size.range.min;
		}
		if (xfer->max_packet_size > std_size.range.max) {
			xfer->max_packet_size = std_size.range.max;
		}
	} else {

		if (xfer->max_packet_size >= std_size.fixed[3]) {
			xfer->max_packet_size = std_size.fixed[3];
		} else if (xfer->max_packet_size >= std_size.fixed[2]) {
			xfer->max_packet_size = std_size.fixed[2];
		} else if (xfer->max_packet_size >= std_size.fixed[1]) {
			xfer->max_packet_size = std_size.fixed[1];
		} else {
			
			xfer->max_packet_size = std_size.fixed[0];
		}
	}

	/*
	 * Check if the max packet size was outside its allowed range
	 * and clamped to a valid value:
	 */
	if (maxp_old != xfer->max_packet_size)
{
		xfer->flags_int.maxp_was_clamped = 1;
}
	
	

	usbd_update_max_frame_size(xfer);

	

	if (type == UE_ISOCHRONOUS) {

		uint16_t frame_limit;

		xfer->interval = 0;	
		xfer->flags_int.isochronous_xfr = 1;	

		if (xfer->timeout == 0) {
			/*
			 * set a default timeout in
			 * case something goes wrong!
			 */
			xfer->timeout = 1000 / 4;
		}
		switch (parm->speed) {
		case USB_SPEED_LOW:
		case USB_SPEED_FULL:
			frame_limit = USB_MAX_FS_ISOC_FRAMES_PER_XFER;
			xfer->fps_shift = 0;
			break;
		default:
			frame_limit = USB_MAX_HS_ISOC_FRAMES_PER_XFER;
			xfer->fps_shift = edesc->bInterval;
			if (xfer->fps_shift > 0)
{
				xfer->fps_shift--;
}
			if (xfer->fps_shift > 3)
{
				xfer->fps_shift = 3;
}
			if (xfer->flags.pre_scale_frames != 0)
{
				xfer->nframes <<= (3 - xfer->fps_shift);
}
			break;
		}

		if (xfer->nframes > frame_limit) {
			/*
			 * this is not going to work
			 * cross hardware
			 */
			parm->err = USB_ERR_INVAL;
			goto done;
		}
		if (xfer->nframes == 0) {
			/*
			 * this is not a valid value
			 */
			parm->err = USB_ERR_ZERO_NFRAMES;
			goto done;
		}
	} else {

		/*
		 * If a value is specified use that else check the
		 * endpoint descriptor!
		 */
		if (type == UE_INTERRUPT) {

			uint32_t temp;

			if (xfer->interval == 0) {

				xfer->interval = edesc->bInterval;

				switch (parm->speed) {
				case USB_SPEED_LOW:
				case USB_SPEED_FULL:
					break;
				default:
					
					if (xfer->interval < 4)
{
						xfer->interval = 1;
}
					else if (xfer->interval > 16)
{
						xfer->interval = (1 << (16 - 4));
}
					else
						xfer->interval = 
						    (1 << (xfer->interval - 4));
					break;
				}
			}

			if (xfer->interval == 0) {
				/*
				 * One millisecond is the smallest
				 * interval we support:
				 */
				xfer->interval = 1;
			}

			xfer->fps_shift = 0;
			temp = 1;

			while ((temp != 0) && (temp < xfer->interval)) {
				xfer->fps_shift++;
				temp *= 2;
			}

			switch (parm->speed) {
			case USB_SPEED_LOW:
			case USB_SPEED_FULL:
				break;
			default:
				xfer->fps_shift += 3;
				break;
			}
		}
	}

	/*
	 * NOTE: we do not allow "max_packet_size" or "max_frame_size"
	 * to be equal to zero when setting up USB transfers, hence
	 * this leads to a lot of extra code in the USB kernel.
	 */

	if ((xfer->max_frame_size == 0) ||
	    (xfer->max_packet_size == 0)) {

		zmps = 1;

		if ((parm->bufsize <= MIN_PKT) &&
		    (type != UE_CONTROL) &&
		    (type != UE_BULK)) {

			
			xfer->max_packet_size = MIN_PKT;
			xfer->max_packet_count = 1;
			parm->bufsize = 0;	
			usbd_update_max_frame_size(xfer);

		} else {
			parm->err = USB_ERR_ZERO_MAXP;
			goto done;
		}

	} else {
		zmps = 0;
	}

	/*
	 * check if we should setup a default
	 * length:
	 */

	if (parm->bufsize == 0) {

		parm->bufsize = xfer->max_frame_size;

		if (type == UE_ISOCHRONOUS) {
			parm->bufsize *= xfer->nframes;
		}
	}
	/*
	 * check if we are about to setup a proxy
	 * type of buffer:
	 */

	if (xfer->flags.proxy_buffer) {

		

		parm->bufsize += (xfer->max_frame_size - 1);

		if (parm->bufsize < xfer->max_frame_size) {
			
			parm->err = USB_ERR_INVAL;
			goto done;
		}
		

		parm->bufsize -= (parm->bufsize % xfer->max_frame_size);

		

		if (type == UE_CONTROL) {
			parm->bufsize += REQ_SIZE;	
		}
	}
	xfer->max_data_length = parm->bufsize;

	

	if (type == UE_ISOCHRONOUS) {
		n_frlengths = xfer->nframes;
		n_frbuffers = 1;
	} else {

		if (type == UE_CONTROL) {
			xfer->flags_int.control_xfr = 1;
			if (xfer->nframes == 0) {
				if (parm->bufsize <= REQ_SIZE) {
					/*
					 * there will never be any data
					 * stage
					 */
					xfer->nframes = 1;
				} else {
					xfer->nframes = 2;
				}
			}
		} else {
			if (xfer->nframes == 0) {
				xfer->nframes = 1;
			}
		}

		n_frlengths = xfer->nframes;
		n_frbuffers = xfer->nframes;
	}

	/*
	 * check if we have room for the
	 * USB device request structure:
	 */

	if (type == UE_CONTROL) {

		if (xfer->max_data_length < REQ_SIZE) {
			
			parm->err = USB_ERR_INVAL;
			goto done;
		}
		xfer->max_data_length -= REQ_SIZE;
	}
	/*
	 * Setup "frlengths" and shadow "frlengths" for keeping the
	 * initial frame lengths when a USB transfer is complete. This
	 * information is useful when computing isochronous offsets.
	 */
	xfer->frlengths = parm->xfer_length_ptr;
	parm->xfer_length_ptr += 2 * n_frlengths;

	
	xfer->frbuffers = parm->xfer_page_cache_ptr;
	parm->xfer_page_cache_ptr += n_frbuffers;

	
	xfer->max_frame_count = xfer->nframes;

	/*
	 * check if we need to setup
	 * a local buffer:
	 */

	if (!xfer->flags.ext_buffer) {
#if USB_HAVE_BUSDMA
		struct usb_page_search page_info;
		struct usb_page_cache *pc;

		if (usbd_transfer_setup_sub_malloc(parm,
		    &pc, parm->bufsize, 1, 1)) {
			parm->err = USB_ERR_NOMEM;
		} else if (parm->buf != NULL) {

			usbd_get_page(pc, 0, &page_info);

			xfer->local_buffer = page_info.buffer;

			usbd_xfer_set_frame_offset(xfer, 0, 0);

			if ((type == UE_CONTROL) && (n_frbuffers > 1)) {
				usbd_xfer_set_frame_offset(xfer, REQ_SIZE, 1);
			}
		}
#else
		
		parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));

		if (parm->buf != NULL) {
			xfer->local_buffer =
			    USB_ADD_BYTES(parm->buf, parm->size[0]);

			usbd_xfer_set_frame_offset(xfer, 0, 0);

			if ((type == UE_CONTROL) && (n_frbuffers > 1)) {
				usbd_xfer_set_frame_offset(xfer, REQ_SIZE, 1);
			}
		}
		parm->size[0] += parm->bufsize;

		
		parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));
#endif
	}
	/*
	 * Compute maximum buffer size
	 */

	if (parm->bufsize_max < parm->bufsize) {
		parm->bufsize_max = parm->bufsize;
	}
#if USB_HAVE_BUSDMA
	if (xfer->flags_int.bdma_enable) {
		/*
		 * Setup "dma_page_ptr".
		 *
		 * Proof for formula below:
		 *
		 * Assume there are three USB frames having length "a", "b" and
		 * "c". These USB frames will at maximum need "z"
		 * "usb_page" structures. "z" is given by:
		 *
		 * z = ((a / USB_PAGE_SIZE) + 2) + ((b / USB_PAGE_SIZE) + 2) +
		 * ((c / USB_PAGE_SIZE) + 2);
		 *
		 * Constraining "a", "b" and "c" like this:
		 *
		 * (a + b + c) <= parm->bufsize
		 *
		 * We know that:
		 *
		 * z <= ((parm->bufsize / USB_PAGE_SIZE) + (3*2));
		 *
		 * Here is the general formula:
		 */
		xfer->dma_page_ptr = parm->dma_page_ptr;
		parm->dma_page_ptr += (2 * n_frbuffers);
		parm->dma_page_ptr += (parm->bufsize / USB_PAGE_SIZE);
	}
#endif
	if (zmps) {
		
		xfer->max_data_length = 0;
	}
	

	xfer->max_hc_frame_size =
	    (parm->hc_max_frame_size -
	    (parm->hc_max_frame_size % xfer->max_frame_size));

	if (xfer->max_hc_frame_size == 0) {
		parm->err = USB_ERR_INVAL;
		goto done;
	}

	

	if (parm->buf) {
		for (x = 0; x != n_frbuffers; x++) {
			xfer->frbuffers[x].tag_parent =
			    &xfer->xroot->dma_parent_tag;
#if USB_HAVE_BUSDMA
			if (xfer->flags_int.bdma_enable &&
			    (parm->bufsize_max > 0)) {

				if (usb_pc_dmamap_create(
				    xfer->frbuffers + x,
				    parm->bufsize_max)) {
					parm->err = USB_ERR_NOMEM;
					goto done;
				}
			}
#endif
		}
	}
done:
	if (parm->err) {
		/*
		 * Set some dummy values so that we avoid division by zero:
		 */
		xfer->max_hc_frame_size = 1;
		xfer->max_frame_size = 1;
		xfer->max_packet_size = 1;
		xfer->max_data_length = 0;
		xfer->nframes = 0;
		xfer->max_frame_count = 0;
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub, return);
}

static uint8_t
usbd_transfer_setup_has_bulk(const struct usb_config *setup_start,
    uint16_t n_setup)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_has_bulk, entry);
	while (n_setup--) {
		uint8_t type = setup_start[n_setup].type;
		if (type == UE_BULK || type == UE_BULK_INTR ||
		    type == UE_TYPE_ANY)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_has_bulk, return);
			return (1);
}
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup_has_bulk, return);
	return (0);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_setup - setup an array of USB transfers
 *
 * NOTE: You must always call "usbd_transfer_unsetup" after calling
 * "usbd_transfer_setup" if success was returned.
 *
 * The idea is that the USB device driver should pre-allocate all its
 * transfers by one call to this function.
 *
 * Return values:
 *    0: Success
 * Else: Failure
 *------------------------------------------------------------------------*/
usb_error_t
usbd_transfer_setup(struct usb_device *udev,
    const uint8_t *ifaces, struct usb_xfer **ppxfer,
    const struct usb_config *setup_start, uint16_t n_setup,
    void *priv_sc, struct mtx *xfer_mtx)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup, entry);
	const struct usb_config *setup_end = setup_start + n_setup;
	const struct usb_config *setup;
	struct usb_setup_params *parm;
	struct usb_endpoint *ep;
	struct usb_xfer_root *info;
	struct usb_xfer *xfer;
	void *buf = NULL;
	usb_error_t error = 0;
	uint16_t n;
	uint16_t refcount;
	uint8_t do_unlock;

	WITNESS_WARN(WARN_GIANTOK | WARN_SLEEPOK, NULL,
	    "usbd_transfer_setup can sleep!");

	

	if (n_setup == 0) {
		DPRINTFN(6, "setup array has zero length!\n");
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup, return);
		return (USB_ERR_INVAL);
	}
	if (ifaces == NULL) {
		DPRINTFN(6, "ifaces array is NULL!\n");
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup, return);
		return (USB_ERR_INVAL);
	}
	if (xfer_mtx == NULL) {
		DPRINTFN(6, "using global lock\n");
		xfer_mtx = &Giant;
	}

	

	for (setup = setup_start, n = 0;
	    setup != setup_end; setup++, n++) {
		if (setup->bufsize == (usb_frlength_t)-1) {
			error = USB_ERR_BAD_BUFSIZE;
			DPRINTF("invalid bufsize\n");
		}
		if (setup->callback == NULL) {
			error = USB_ERR_NO_CALLBACK;
			DPRINTF("no callback\n");
		}
		ppxfer[n] = NULL;
	}

	if (error)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup, return);
		return (error);
}

	
	do_unlock = usbd_ctrl_lock(udev);

	refcount = 0;
	info = NULL;

	parm = &udev->scratch.xfer_setup[0].parm;
	memset(parm, 0, sizeof(*parm));

	parm->udev = udev;
	parm->speed = usbd_get_speed(udev);
	parm->hc_max_packet_count = 1;

	if (parm->speed >= USB_SPEED_MAX) {
		parm->err = USB_ERR_INVAL;
		goto done;
	}
	

	while (1) {

		if (buf) {
			/*
			 * Initialize the "usb_xfer_root" structure,
			 * which is common for all our USB transfers.
			 */
			info = USB_ADD_BYTES(buf, 0);

			info->memory_base = buf;
			info->memory_size = parm->size[0];

#if USB_HAVE_BUSDMA
			info->dma_page_cache_start = USB_ADD_BYTES(buf, parm->size[4]);
			info->dma_page_cache_end = USB_ADD_BYTES(buf, parm->size[5]);
#endif
			info->xfer_page_cache_start = USB_ADD_BYTES(buf, parm->size[5]);
			info->xfer_page_cache_end = USB_ADD_BYTES(buf, parm->size[2]);

			cv_init(&info->cv_drain, "WDRAIN");

			info->xfer_mtx = xfer_mtx;
#if USB_HAVE_BUSDMA
			usb_dma_tag_setup(&info->dma_parent_tag,
			    parm->dma_tag_p, udev->bus->dma_parent_tag[0].tag,
			    xfer_mtx, &usb_bdma_done_event, udev->bus->dma_bits,
			    parm->dma_tag_max);
#endif

			info->bus = udev->bus;
			info->udev = udev;

			TAILQ_INIT(&info->done_q.head);
			info->done_q.command = &usbd_callback_wrapper;
#if USB_HAVE_BUSDMA
			TAILQ_INIT(&info->dma_q.head);
			info->dma_q.command = &usb_bdma_work_loop;
#endif
			info->done_m[0].hdr.pm_callback = &usb_callback_proc;
			info->done_m[0].xroot = info;
			info->done_m[1].hdr.pm_callback = &usb_callback_proc;
			info->done_m[1].xroot = info;

			/* 
			 * In device side mode control endpoint
			 * requests need to run from a separate
			 * context, else there is a chance of
			 * deadlock!
			 */
			if (setup_start == usb_control_ep_cfg)
{
				info->done_p =
				    USB_BUS_CONTROL_XFER_PROC(udev->bus);
}
			else if (xfer_mtx == &Giant)
{
				info->done_p =
				    USB_BUS_GIANT_PROC(udev->bus);
}
			else if (usbd_transfer_setup_has_bulk(setup_start, n_setup))
{
				info->done_p =
				    USB_BUS_NON_GIANT_BULK_PROC(udev->bus);
}
			else
				info->done_p =
				    USB_BUS_NON_GIANT_ISOC_PROC(udev->bus);
		}
		

		parm->size[0] = 0;
		parm->buf = buf;
		parm->size[0] += sizeof(info[0]);

		for (setup = setup_start, n = 0;
		    setup != setup_end; setup++, n++) {

			
			if (setup->callback == NULL) {
				continue;
			}
			
			ep = usbd_get_endpoint(udev,
			    ifaces[setup->if_index], setup);

			/*
			 * Check that the USB PIPE is valid and that
			 * the endpoint mode is proper.
			 *
			 * Make sure we don't allocate a streams
			 * transfer when such a combination is not
			 * valid.
			 */
			if ((ep == NULL) || (ep->methods == NULL) ||
			    ((ep->ep_mode != USB_EP_MODE_STREAMS) &&
			    (ep->ep_mode != USB_EP_MODE_DEFAULT)) ||
			    (setup->stream_id != 0 &&
			    (setup->stream_id >= USB_MAX_EP_STREAMS ||
			    (ep->ep_mode != USB_EP_MODE_STREAMS)))) {
				if (setup->flags.no_pipe_ok)
{
					continue;
}
				if ((setup->usb_mode != USB_MODE_DUAL) &&
				    (setup->usb_mode != udev->flags.usb_mode))
{
					continue;
}
				parm->err = USB_ERR_NO_PIPE;
				goto done;
			}

			
			parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));

			
			parm->curr_setup = setup;

			if (buf) {
				/*
				 * Common initialization of the
				 * "usb_xfer" structure.
				 */
				xfer = USB_ADD_BYTES(buf, parm->size[0]);
				xfer->address = udev->address;
				xfer->priv_sc = priv_sc;
				xfer->xroot = info;

				usb_callout_init_mtx(&xfer->timeout_handle,
				    &udev->bus->bus_mtx, 0);
			} else {
				/*
				 * Setup a dummy xfer, hence we are
				 * writing to the "usb_xfer"
				 * structure pointed to by "xfer"
				 * before we have allocated any
				 * memory:
				 */
				xfer = &udev->scratch.xfer_setup[0].dummy;
				memset(xfer, 0, sizeof(*xfer));
				refcount++;
			}

			
			xfer->endpoint = ep;

			
			xfer->stream_id = setup->stream_id;

			parm->size[0] += sizeof(xfer[0]);
			parm->methods = xfer->endpoint->methods;
			parm->curr_xfer = xfer;

			/*
			 * Call the Host or Device controller transfer
			 * setup routine:
			 */
			(udev->bus->methods->xfer_setup) (parm);

			
			if (parm->err)
{
				goto done;
}

			if (buf) {
				/*
				 * Increment the endpoint refcount. This
				 * basically prevents setting a new
				 * configuration and alternate setting
				 * when USB transfers are in use on
				 * the given interface. Search the USB
				 * code for "endpoint->refcount_alloc" if you
				 * want more information.
				 */
				USB_BUS_LOCK(info->bus);
				if (xfer->endpoint->refcount_alloc >= USB_EP_REF_MAX)
{
					parm->err = USB_ERR_INVAL;
}

				xfer->endpoint->refcount_alloc++;

				if (xfer->endpoint->refcount_alloc == 0)
{
					panic("usbd_transfer_setup(): Refcount wrapped to zero\n");
}
				USB_BUS_UNLOCK(info->bus);

				/*
				 * Whenever we set ppxfer[] then we
				 * also need to increment the
				 * "setup_refcount":
				 */
				info->setup_refcount++;

				/*
				 * Transfer is successfully setup and
				 * can be used:
				 */
				ppxfer[n] = xfer;
			}

			
			if (parm->err)
{
				goto done;
}
		}

		if (buf != NULL || parm->err != 0)
{
			goto done;
}

		
		if (refcount == 0)
{
			goto done;
}

		
		parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));

		
		parm->size[1] = parm->size[0];

		/*
		 * The number of DMA tags required depends on
		 * the number of endpoints. The current estimate
		 * for maximum number of DMA tags per endpoint
		 * is three:
		 * 1) for loading memory
		 * 2) for allocating memory
		 * 3) for fixing memory [UHCI]
		 */
		parm->dma_tag_max += 3 * MIN(n_setup, USB_EP_MAX);

		/*
		 * DMA tags for QH, TD, Data and more.
		 */
		parm->dma_tag_max += 8;

		parm->dma_tag_p += parm->dma_tag_max;

		parm->size[0] += ((uint8_t *)parm->dma_tag_p) -
		    ((uint8_t *)0);

		
		parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));

		
		parm->size[3] = parm->size[0];

		parm->size[0] += ((uint8_t *)parm->dma_page_ptr) -
		    ((uint8_t *)0);

		
		parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));

		
		parm->size[4] = parm->size[0];

		parm->size[0] += ((uint8_t *)parm->dma_page_cache_ptr) -
		    ((uint8_t *)0);

		
		parm->size[5] = parm->size[0];

		parm->size[0] += ((uint8_t *)parm->xfer_page_cache_ptr) -
		    ((uint8_t *)0);

		

		parm->size[2] = parm->size[0];

		
		parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));

		parm->size[6] = parm->size[0];

		parm->size[0] += ((uint8_t *)parm->xfer_length_ptr) -
		    ((uint8_t *)0);

		
		parm->size[0] += ((-parm->size[0]) & (USB_HOST_ALIGN - 1));

		
		buf = malloc(parm->size[0], M_USB, M_WAITOK | M_ZERO);

		if (buf == NULL) {
			parm->err = USB_ERR_NOMEM;
			DPRINTFN(0, "cannot allocate memory block for "
			    "configuration (%d bytes)\n",
			    parm->size[0]);
			goto done;
		}
		parm->dma_tag_p = USB_ADD_BYTES(buf, parm->size[1]);
		parm->dma_page_ptr = USB_ADD_BYTES(buf, parm->size[3]);
		parm->dma_page_cache_ptr = USB_ADD_BYTES(buf, parm->size[4]);
		parm->xfer_page_cache_ptr = USB_ADD_BYTES(buf, parm->size[5]);
		parm->xfer_length_ptr = USB_ADD_BYTES(buf, parm->size[6]);
	}

done:
	if (buf) {
		if (info->setup_refcount == 0) {
			/*
			 * "usbd_transfer_unsetup_sub" will unlock
			 * the bus mutex before returning !
			 */
			USB_BUS_LOCK(info->bus);

			
			usbd_transfer_unsetup_sub(info, 0);
		}
	}

	
	if (parm->err)
{
		usbd_transfer_unsetup(ppxfer, n_setup);
}

	error = parm->err;

	if (do_unlock)
{
		usbd_ctrl_unlock(udev);
}

	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_setup, return);
	return (error);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_unsetup_sub - factored out code
 *------------------------------------------------------------------------*/
static void
usbd_transfer_unsetup_sub(struct usb_xfer_root *info, uint8_t needs_delay)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_unsetup_sub, entry);
#if USB_HAVE_BUSDMA
	struct usb_page_cache *pc;
#endif

	USB_BUS_LOCK_ASSERT(info->bus, MA_OWNED);

	

	if (needs_delay) {
		usb_timeout_t temp;
		temp = usbd_get_dma_delay(info->udev);
		if (temp != 0) {
			usb_pause_mtx(&info->bus->bus_mtx,
			    USB_MS_TO_TICKS(temp));
		}
	}

	
	usb_proc_mwait(info->done_p, &info->done_m[0], &info->done_m[1]);

	USB_BUS_UNLOCK(info->bus);

#if USB_HAVE_BUSDMA
	
	pc = info->dma_page_cache_start;
	while (pc != info->dma_page_cache_end) {
		usb_pc_free_mem(pc);
		pc++;
	}

	
	pc = info->xfer_page_cache_start;
	while (pc != info->xfer_page_cache_end) {
		usb_pc_dmamap_destroy(pc);
		pc++;
	}

	
	usb_dma_tag_unsetup(&info->dma_parent_tag);
#endif

	cv_destroy(&info->cv_drain);

	/*
	 * free the "memory_base" last, hence the "info" structure is
	 * contained within the "memory_base"!
	 */
	free(info->memory_base, M_USB);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_unsetup_sub, return);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_unsetup - unsetup/free an array of USB transfers
 *
 * NOTE: All USB transfers in progress will get called back passing
 * the error code "USB_ERR_CANCELLED" before this function
 * returns.
 *------------------------------------------------------------------------*/
void
usbd_transfer_unsetup(struct usb_xfer **pxfer, uint16_t n_setup)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_unsetup, entry);
	struct usb_xfer *xfer;
	struct usb_xfer_root *info;
	uint8_t needs_delay = 0;

	WITNESS_WARN(WARN_GIANTOK | WARN_SLEEPOK, NULL,
	    "usbd_transfer_unsetup can sleep!");

	while (n_setup--) {
		xfer = pxfer[n_setup];

		if (xfer == NULL)
{
			continue;
}

		info = xfer->xroot;

		USB_XFER_LOCK(xfer);
		USB_BUS_LOCK(info->bus);

		/*
		 * HINT: when you start/stop a transfer, it might be a
		 * good idea to directly use the "pxfer[]" structure:
		 *
		 * usbd_transfer_start(sc->pxfer[0]);
		 * usbd_transfer_stop(sc->pxfer[0]);
		 *
		 * That way, if your code has many parts that will not
		 * stop running under the same lock, in other words
		 * "xfer_mtx", the usbd_transfer_start and
		 * usbd_transfer_stop functions will simply return
		 * when they detect a NULL pointer argument.
		 *
		 * To avoid any races we clear the "pxfer[]" pointer
		 * while holding the private mutex of the driver:
		 */
		pxfer[n_setup] = NULL;

		USB_BUS_UNLOCK(info->bus);
		USB_XFER_UNLOCK(xfer);

		usbd_transfer_drain(xfer);

#if USB_HAVE_BUSDMA
		if (xfer->flags_int.bdma_enable)
{
			needs_delay = 1;
}
#endif
		/*
		 * NOTE: default endpoint does not have an
		 * interface, even if endpoint->iface_index == 0
		 */
		USB_BUS_LOCK(info->bus);
		xfer->endpoint->refcount_alloc--;
		USB_BUS_UNLOCK(info->bus);

		usb_callout_drain(&xfer->timeout_handle);

		USB_BUS_LOCK(info->bus);

		USB_ASSERT(info->setup_refcount != 0, ("Invalid setup "
		    "reference count\n"));

		info->setup_refcount--;

		if (info->setup_refcount == 0) {
			usbd_transfer_unsetup_sub(info,
			    needs_delay);
		} else {
			USB_BUS_UNLOCK(info->bus);
		}
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_unsetup, return);
}

/*------------------------------------------------------------------------*
 *	usbd_control_transfer_init - factored out code
 *
 * In USB Device Mode we have to wait for the SETUP packet which
 * containst the "struct usb_device_request" structure, before we can
 * transfer any data. In USB Host Mode we already have the SETUP
 * packet at the moment the USB transfer is started. This leads us to
 * having to setup the USB transfer at two different places in
 * time. This function just contains factored out control transfer
 * initialisation code, so that we don't duplicate the code.
 *------------------------------------------------------------------------*/
static void
usbd_control_transfer_init(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_control_transfer_init, entry);
	struct usb_device_request req;

	

	usbd_copy_out(xfer->frbuffers, 0, &req, sizeof(req));

	

	xfer->flags_int.control_rem = UGETW(req.wLength);

	

	xfer->endpointno &= ~(UE_DIR_IN | UE_DIR_OUT);
	xfer->endpointno |=
	    (req.bmRequestType & UT_READ) ? UE_DIR_IN : UE_DIR_OUT;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_control_transfer_init, return);
}

/*------------------------------------------------------------------------*
 *	usbd_control_transfer_did_data
 *
 * This function returns non-zero if a control endpoint has
 * transferred the first DATA packet after the SETUP packet.
 * Else it returns zero.
 *------------------------------------------------------------------------*/
static uint8_t
usbd_control_transfer_did_data(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_control_transfer_did_data, entry);
	struct usb_device_request req;

	
	if (xfer->flags_int.control_hdr != 0)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_control_transfer_did_data, return);
		return (0);
}

	
	usbd_copy_out(xfer->frbuffers, 0, &req, sizeof(req));

	
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_control_transfer_did_data, return);
	return (xfer->flags_int.control_rem != UGETW(req.wLength));
}

/*------------------------------------------------------------------------*
 *	usbd_setup_ctrl_transfer
 *
 * This function handles initialisation of control transfers. Control
 * transfers are special in that regard that they can both transmit
 * and receive data.
 *
 * Return values:
 *    0: Success
 * Else: Failure
 *------------------------------------------------------------------------*/
static int
usbd_setup_ctrl_transfer(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_setup_ctrl_transfer, entry);
	usb_frlength_t len;

	
	if (xfer->flags.stall_pipe && xfer->flags_int.control_act) {
		
		xfer->flags_int.control_stall = 1;
		xfer->flags_int.control_act = 0;
	} else {
		
		xfer->flags_int.control_stall = 0;
	}

	
	if (xfer->nframes > 2) {
		/*
		 * If you need to split a control transfer, you
		 * have to do one part at a time. Only with
		 * non-control transfers you can do multiple
		 * parts a time.
		 */
		DPRINTFN(0, "Too many frames: %u\n",
		    (unsigned int)xfer->nframes);
		goto error;
	}

	/*
         * Check if there is a control
         * transfer in progress:
         */
	if (xfer->flags_int.control_act) {

		if (xfer->flags_int.control_hdr) {

			

			xfer->flags_int.control_hdr = 0;

			
			if (xfer->flags_int.usb_mode == USB_MODE_DEVICE) {
				usbd_control_transfer_init(xfer);
			}
		}
		

		len = xfer->sumlen;

	} else {

		

		if (xfer->frlengths[0] != sizeof(struct usb_device_request)) {
			DPRINTFN(0, "Wrong framelength %u != %zu\n",
			    xfer->frlengths[0], sizeof(struct
			    usb_device_request));
			goto error;
		}
		
		if (xfer->flags_int.usb_mode == USB_MODE_DEVICE) {

			
			if (xfer->nframes != 1) {
				/*
			         * We need to receive the setup
			         * message first so that we know the
			         * data direction!
			         */
				DPRINTF("Misconfigured transfer\n");
				goto error;
			}
			/*
			 * Set a dummy "control_rem" value.  This
			 * variable will be overwritten later by a
			 * call to "usbd_control_transfer_init()" !
			 */
			xfer->flags_int.control_rem = 0xFFFF;
		} else {

			

			usbd_control_transfer_init(xfer);
		}

		

		xfer->flags_int.control_hdr = 1;

		

		len = (xfer->sumlen - sizeof(struct usb_device_request));
	}

	

	xfer->flags_int.control_did_data =
	    usbd_control_transfer_did_data(xfer);

	

	if (len > xfer->flags_int.control_rem) {
		DPRINTFN(0, "Length (%d) greater than "
		    "remaining length (%d)\n", len,
		    xfer->flags_int.control_rem);
		goto error;
	}
	

	if (xfer->flags.force_short_xfer) {
		xfer->flags_int.control_rem = 0;
	} else {
		if ((len != xfer->max_data_length) &&
		    (len != xfer->flags_int.control_rem) &&
		    (xfer->nframes != 1)) {
			DPRINTFN(0, "Short control transfer without "
			    "force_short_xfer set\n");
			goto error;
		}
		xfer->flags_int.control_rem -= len;
	}

	

	if ((xfer->flags_int.control_rem > 0) ||
	    (xfer->flags.manual_status)) {
		
		xfer->flags_int.control_act = 1;

		
		if ((!xfer->flags_int.control_hdr) &&
		    (xfer->nframes == 1)) {
			/*
		         * This is not a valid operation!
		         */
			DPRINTFN(0, "Invalid parameter "
			    "combination\n");
			goto error;
		}
	} else {
		
		xfer->flags_int.control_act = 0;
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_setup_ctrl_transfer, return);
	return (0);			

error:
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_setup_ctrl_transfer, return);
	return (1);			
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_submit - start USB hardware for the given transfer
 *
 * This function should only be called from the USB callback.
 *------------------------------------------------------------------------*/
void
usbd_transfer_submit(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, entry);
	struct usb_xfer_root *info;
	struct usb_bus *bus;
	usb_frcount_t x;

	info = xfer->xroot;
	bus = info->bus;

	DPRINTF("xfer=%p, endpoint=%p, nframes=%d, dir=%s\n",
	    xfer, xfer->endpoint, xfer->nframes, USB_GET_DATA_ISREAD(xfer) ?
	    "read" : "write");

#ifdef USB_DEBUG
	if (USB_DEBUG_VAR > 0) {
		USB_BUS_LOCK(bus);

		usb_dump_endpoint(xfer->endpoint);

		USB_BUS_UNLOCK(bus);
	}
#endif

	USB_XFER_LOCK_ASSERT(xfer, MA_OWNED);
	USB_BUS_LOCK_ASSERT(bus, MA_NOTOWNED);

	
	if (!xfer->flags_int.open) {
		xfer->flags_int.open = 1;

		DPRINTF("open\n");

		USB_BUS_LOCK(bus);
		(xfer->endpoint->methods->open) (xfer);
		USB_BUS_UNLOCK(bus);
	}
	
	xfer->flags_int.transferring = 1;

#if USB_HAVE_POWERD
	
	usbd_transfer_power_ref(xfer, 1);
#endif
	/*
	 * Check if the transfer is waiting on a queue, most
	 * frequently the "done_q":
	 */
	if (xfer->wait_queue) {
		USB_BUS_LOCK(bus);
		usbd_transfer_dequeue(xfer);
		USB_BUS_UNLOCK(bus);
	}
	
	xfer->flags_int.did_dma_delay = 0;

	
	xfer->flags_int.did_close = 0;

#if USB_HAVE_BUSDMA
	
	xfer->flags_int.bdma_setup = 0;
#endif
	
	xfer->flags_int.can_cancel_immed = 0;

	
	xfer->sumlen = 0;
	xfer->actlen = 0;
	xfer->aframes = 0;

	
	xfer->error = 0;

	
	if (info->udev->state < USB_STATE_POWERED) {
		USB_BUS_LOCK(bus);
		/*
		 * Must return cancelled error code else
		 * device drivers can hang.
		 */
		usbd_transfer_done(xfer, USB_ERR_CANCELLED);
		USB_BUS_UNLOCK(bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, return);
		return;
	}

	
	if (xfer->nframes == 0) {
		if (xfer->flags.stall_pipe) {
			/*
			 * Special case - want to stall without transferring
			 * any data:
			 */
			DPRINTF("xfer=%p nframes=0: stall "
			    "or clear stall!\n", xfer);
			USB_BUS_LOCK(bus);
			xfer->flags_int.can_cancel_immed = 1;
			
			usb_command_wrapper(&xfer->endpoint->
			    endpoint_q[xfer->stream_id], xfer);
			USB_BUS_UNLOCK(bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, return);
			return;
		}
		USB_BUS_LOCK(bus);
		usbd_transfer_done(xfer, USB_ERR_INVAL);
		USB_BUS_UNLOCK(bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, return);
		return;
	}
	

	for (x = 0; x != xfer->nframes; x++) {
		
		xfer->frlengths[x + xfer->max_frame_count] = xfer->frlengths[x];
		
		xfer->sumlen += xfer->frlengths[x];
		if (xfer->sumlen < xfer->frlengths[x]) {
			
			USB_BUS_LOCK(bus);
			usbd_transfer_done(xfer, USB_ERR_INVAL);
			USB_BUS_UNLOCK(bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, return);
			return;
		}
	}

	

	xfer->flags_int.short_xfer_ok = 0;
	xfer->flags_int.short_frames_ok = 0;

	

	if (xfer->flags_int.control_xfr) {

		if (usbd_setup_ctrl_transfer(xfer)) {
			USB_BUS_LOCK(bus);
			usbd_transfer_done(xfer, USB_ERR_STALLED);
			USB_BUS_UNLOCK(bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, return);
			return;
		}
	}
	/*
	 * Setup filtered version of some transfer flags,
	 * in case of data read direction
	 */
	if (USB_GET_DATA_ISREAD(xfer)) {

		if (xfer->flags.short_frames_ok) {
			xfer->flags_int.short_xfer_ok = 1;
			xfer->flags_int.short_frames_ok = 1;
		} else if (xfer->flags.short_xfer_ok) {
			xfer->flags_int.short_xfer_ok = 1;

			
			if (xfer->flags_int.control_xfr) {
				/*
				 * 1) Control transfers do not support
				 * reception of multiple short USB
				 * frames in host mode and device side
				 * mode, with exception of:
				 *
				 * 2) Due to sometimes buggy device
				 * side firmware we need to do a
				 * STATUS stage in case of short
				 * control transfers in USB host mode.
				 * The STATUS stage then becomes the
				 * "alt_next" to the DATA stage.
				 */
				xfer->flags_int.short_frames_ok = 1;
			}
		}
	}
	/*
	 * Check if BUS-DMA support is enabled and try to load virtual
	 * buffers into DMA, if any:
	 */
#if USB_HAVE_BUSDMA
	if (xfer->flags_int.bdma_enable) {
		
		usb_command_wrapper(&xfer->xroot->dma_q, xfer);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, return);
		return;
	}
#endif
	/*
	 * Enter the USB transfer into the Host Controller or
	 * Device Controller schedule:
	 */
	usbd_pipe_enter(xfer);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_submit, return);
}

/*------------------------------------------------------------------------*
 *	usbd_pipe_enter - factored out code
 *------------------------------------------------------------------------*/
void
usbd_pipe_enter(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_enter, entry);
	struct usb_endpoint *ep;

	USB_XFER_LOCK_ASSERT(xfer, MA_OWNED);

	USB_BUS_LOCK(xfer->xroot->bus);

	ep = xfer->endpoint;

	DPRINTF("enter\n");

	
	xfer->flags_int.can_cancel_immed = 1;

	
	(ep->methods->enter) (xfer);

	
	if (xfer->error) {
		
		usbd_transfer_done(xfer, 0);
		USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_enter, return);
		return;
	}

	
	usb_command_wrapper(&ep->endpoint_q[xfer->stream_id], xfer);
	USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_enter, return);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_start - start an USB transfer
 *
 * NOTE: Calling this function more than one time will only
 *       result in a single transfer start, until the USB transfer
 *       completes.
 *------------------------------------------------------------------------*/
void
usbd_transfer_start(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_start, entry);
	if (xfer == NULL) {
		
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_start, return);
		return;
	}
	USB_XFER_LOCK_ASSERT(xfer, MA_OWNED);

	

	if (!xfer->flags_int.started) {
		
		USB_BUS_LOCK(xfer->xroot->bus);
		xfer->flags_int.started = 1;
		USB_BUS_UNLOCK(xfer->xroot->bus);
	}
	

	if (xfer->flags_int.transferring) {
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_start, return);
		return;
	}
	USB_BUS_LOCK(xfer->xroot->bus);
	
	usbd_callback_ss_done_defer(xfer);
	USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_start, return);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_stop - stop an USB transfer
 *
 * NOTE: Calling this function more than one time will only
 *       result in a single transfer stop.
 * NOTE: When this function returns it is not safe to free nor
 *       reuse any DMA buffers. See "usbd_transfer_drain()".
 *------------------------------------------------------------------------*/
void
usbd_transfer_stop(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_stop, entry);
	struct usb_endpoint *ep;

	if (xfer == NULL) {
		
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_stop, return);
		return;
	}
	USB_XFER_LOCK_ASSERT(xfer, MA_OWNED);

	

	if (!xfer->flags_int.open) {
		if (xfer->flags_int.started) {
			
			
			USB_BUS_LOCK(xfer->xroot->bus);
			xfer->flags_int.started = 0;
			USB_BUS_UNLOCK(xfer->xroot->bus);
		}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_stop, return);
		return;
	}
	

	USB_BUS_LOCK(xfer->xroot->bus);
	
	xfer->error = USB_ERR_CANCELLED;

	/*
	 * Clear "open" and "started" when both private and USB lock
	 * is locked so that we don't get a race updating "flags_int"
	 */
	xfer->flags_int.open = 0;
	xfer->flags_int.started = 0;

	/*
	 * Check if we can cancel the USB transfer immediately.
	 */
	if (xfer->flags_int.transferring) {
		if (xfer->flags_int.can_cancel_immed &&
		    (!xfer->flags_int.did_close)) {
			DPRINTF("close\n");
			/*
			 * The following will lead to an USB_ERR_CANCELLED
			 * error code being passed to the USB callback.
			 */
			(xfer->endpoint->methods->close) (xfer);
			
			xfer->flags_int.did_close = 1;
		} else {
			
		}
	} else {
		DPRINTF("close\n");

		
		(xfer->endpoint->methods->close) (xfer);

		/*
		 * Any additional DMA delay is done by
		 * "usbd_transfer_unsetup()".
		 */

		/*
		 * Special case. Check if we need to restart a blocked
		 * endpoint.
		 */
		ep = xfer->endpoint;

		/*
		 * If the current USB transfer is completing we need
		 * to start the next one:
		 */
		if (ep->endpoint_q[xfer->stream_id].curr == xfer) {
			usb_command_wrapper(
			    &ep->endpoint_q[xfer->stream_id], NULL);
		}
	}

	USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_stop, return);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_pending
 *
 * This function will check if an USB transfer is pending which is a
 * little bit complicated!
 * Return values:
 * 0: Not pending
 * 1: Pending: The USB transfer will receive a callback in the future.
 *------------------------------------------------------------------------*/
uint8_t
usbd_transfer_pending(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_pending, entry);
	struct usb_xfer_root *info;
	struct usb_xfer_queue *pq;

	if (xfer == NULL) {
		
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_pending, return);
		return (0);
	}
	USB_XFER_LOCK_ASSERT(xfer, MA_OWNED);

	if (xfer->flags_int.transferring) {
		
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_pending, return);
		return (1);
	}
	USB_BUS_LOCK(xfer->xroot->bus);
	if (xfer->wait_queue) {
		
		USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_pending, return);
		return (1);
	}
	info = xfer->xroot;
	pq = &info->done_q;

	if (pq->curr == xfer) {
		
		USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_pending, return);
		return (1);
	}
	
	USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_pending, return);
	return (0);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_drain
 *
 * This function will stop the USB transfer and wait for any
 * additional BUS-DMA and HW-DMA operations to complete. Buffers that
 * are loaded into DMA can safely be freed or reused after that this
 * function has returned.
 *------------------------------------------------------------------------*/
void
usbd_transfer_drain(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_drain, entry);
	WITNESS_WARN(WARN_GIANTOK | WARN_SLEEPOK, NULL,
	    "usbd_transfer_drain can sleep!");

	if (xfer == NULL) {
		
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_drain, return);
		return;
	}
	if (xfer->xroot->xfer_mtx != &Giant) {
		USB_XFER_LOCK_ASSERT(xfer, MA_NOTOWNED);
	}
	USB_XFER_LOCK(xfer);

	usbd_transfer_stop(xfer);

	while (usbd_transfer_pending(xfer) || 
	    xfer->flags_int.doing_callback) {

		/* 
		 * It is allowed that the callback can drop its
		 * transfer mutex. In that case checking only
		 * "usbd_transfer_pending()" is not enough to tell if
		 * the USB transfer is fully drained. We also need to
		 * check the internal "doing_callback" flag.
		 */
		xfer->flags_int.draining = 1;

		/*
		 * Wait until the current outstanding USB
		 * transfer is complete !
		 */
		cv_wait(&xfer->xroot->cv_drain, xfer->xroot->xfer_mtx);
	}
	USB_XFER_UNLOCK(xfer);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_drain, return);
}

struct usb_page_cache *
usbd_xfer_get_frame(struct usb_xfer *xfer, usb_frcount_t frindex)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_get_frame, entry);
	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_get_frame, return);
	return (&xfer->frbuffers[frindex]);
}

void *
usbd_xfer_get_frame_buffer(struct usb_xfer *xfer, usb_frcount_t frindex)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_get_frame_buffer, entry);
	struct usb_page_search page_info;

	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	usbd_get_page(&xfer->frbuffers[frindex], 0, &page_info);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_get_frame_buffer, return);
	return (page_info.buffer);
}

/*------------------------------------------------------------------------*
 *	usbd_xfer_get_fps_shift
 *
 * The following function is only useful for isochronous transfers. It
 * returns how many times the frame execution rate has been shifted
 * down.
 *
 * Return value:
 * Success: 0..3
 * Failure: 0
 *------------------------------------------------------------------------*/
uint8_t
usbd_xfer_get_fps_shift(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_get_fps_shift, entry);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_get_fps_shift, return);
	return (xfer->fps_shift);
}

usb_frlength_t
usbd_xfer_frame_len(struct usb_xfer *xfer, usb_frcount_t frindex)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_frame_len, entry);
	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_frame_len, return);
	return (xfer->frlengths[frindex]);
}

/*------------------------------------------------------------------------*
 *	usbd_xfer_set_frame_data
 *
 * This function sets the pointer of the buffer that should
 * loaded directly into DMA for the given USB frame. Passing "ptr"
 * equal to NULL while the corresponding "frlength" is greater
 * than zero gives undefined results!
 *------------------------------------------------------------------------*/
void
usbd_xfer_set_frame_data(struct usb_xfer *xfer, usb_frcount_t frindex,
    void *ptr, usb_frlength_t len)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frame_data, entry);
	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	
	xfer->frbuffers[frindex].buffer = ptr;
	usbd_xfer_set_frame_len(xfer, frindex, len);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frame_data, return);
}

void
usbd_xfer_frame_data(struct usb_xfer *xfer, usb_frcount_t frindex,
    void **ptr, int *len)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_frame_data, entry);
	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	if (ptr != NULL)
{
		*ptr = xfer->frbuffers[frindex].buffer;
}
	if (len != NULL)
{
		*len = xfer->frlengths[frindex];
}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_frame_data, return);
}

/*------------------------------------------------------------------------*
 *	usbd_xfer_old_frame_length
 *
 * This function returns the framelength of the given frame at the
 * time the transfer was submitted. This function can be used to
 * compute the starting data pointer of the next isochronous frame
 * when an isochronous transfer has completed.
 *------------------------------------------------------------------------*/
usb_frlength_t
usbd_xfer_old_frame_length(struct usb_xfer *xfer, usb_frcount_t frindex)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_old_frame_length, entry);
	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_old_frame_length, return);
	return (xfer->frlengths[frindex + xfer->max_frame_count]);
}

void
usbd_xfer_status(struct usb_xfer *xfer, int *actlen, int *sumlen, int *aframes,
    int *nframes)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_status, entry);
	if (actlen != NULL)
{
		*actlen = xfer->actlen;
}
	if (sumlen != NULL)
{
		*sumlen = xfer->sumlen;
}
	if (aframes != NULL)
{
		*aframes = xfer->aframes;
}
	if (nframes != NULL)
{
		*nframes = xfer->nframes;
}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_status, return);
}

/*------------------------------------------------------------------------*
 *	usbd_xfer_set_frame_offset
 *
 * This function sets the frame data buffer offset relative to the beginning
 * of the USB DMA buffer allocated for this USB transfer.
 *------------------------------------------------------------------------*/
void
usbd_xfer_set_frame_offset(struct usb_xfer *xfer, usb_frlength_t offset,
    usb_frcount_t frindex)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frame_offset, entry);
	KASSERT(!xfer->flags.ext_buffer, ("Cannot offset data frame "
	    "when the USB buffer is external\n"));
	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	
	xfer->frbuffers[frindex].buffer =
	    USB_ADD_BYTES(xfer->local_buffer, offset);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frame_offset, return);
}

void
usbd_xfer_set_interval(struct usb_xfer *xfer, int i)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_interval, entry);
	xfer->interval = i;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_interval, return);
}

void
usbd_xfer_set_timeout(struct usb_xfer *xfer, int t)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_timeout, entry);
	xfer->timeout = t;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_timeout, return);
}

void
usbd_xfer_set_frames(struct usb_xfer *xfer, usb_frcount_t n)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frames, entry);
	xfer->nframes = n;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frames, return);
}

usb_frcount_t
usbd_xfer_max_frames(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_max_frames, entry);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_max_frames, return);
	return (xfer->max_frame_count);
}

usb_frlength_t
usbd_xfer_max_len(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_max_len, entry);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_max_len, return);
	return (xfer->max_data_length);
}

usb_frlength_t
usbd_xfer_max_framelen(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_max_framelen, entry);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_max_framelen, return);
	return (xfer->max_frame_size);
}

void
usbd_xfer_set_frame_len(struct usb_xfer *xfer, usb_frcount_t frindex,
    usb_frlength_t len)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frame_len, entry);
	KASSERT(frindex < xfer->max_frame_count, ("frame index overflow"));

	xfer->frlengths[frindex] = len;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_frame_len, return);
}

/*------------------------------------------------------------------------*
 *	usb_callback_proc - factored out code
 *
 * This function performs USB callbacks.
 *------------------------------------------------------------------------*/
static void
usb_callback_proc(struct usb_proc_msg *_pm)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_callback_proc, entry);
	struct usb_done_msg *pm = (void *)_pm;
	struct usb_xfer_root *info = pm->xroot;

	
	USB_BUS_UNLOCK(info->bus);

	/*
	 * We exploit the fact that the mutex is the same for all
	 * callbacks that will be called from this thread:
	 */
	mtx_lock(info->xfer_mtx);
	USB_BUS_LOCK(info->bus);

	
	usb_command_wrapper(&info->done_q,
	    info->done_q.curr);

	mtx_unlock(info->xfer_mtx);
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_callback_proc, return);
}

/*------------------------------------------------------------------------*
 *	usbd_callback_ss_done_defer
 *
 * This function will defer the start, stop and done callback to the
 * correct thread.
 *------------------------------------------------------------------------*/
static void
usbd_callback_ss_done_defer(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_ss_done_defer, entry);
	struct usb_xfer_root *info = xfer->xroot;
	struct usb_xfer_queue *pq = &info->done_q;

	USB_BUS_LOCK_ASSERT(xfer->xroot->bus, MA_OWNED);

	if (pq->curr != xfer) {
		usbd_transfer_enqueue(pq, xfer);
	}
	if (!pq->recurse_1) {

		/*
	         * We have to postpone the callback due to the fact we
	         * will have a Lock Order Reversal, LOR, if we try to
	         * proceed !
	         */
		(void) usb_proc_msignal(info->done_p,
		    &info->done_m[0], &info->done_m[1]);
	} else {
		
		pq->recurse_2 = 0;
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_ss_done_defer, return);
	return;

	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_ss_done_defer, return);
}

/*------------------------------------------------------------------------*
 *	usbd_callback_wrapper
 *
 * This is a wrapper for USB callbacks. This wrapper does some
 * auto-magic things like figuring out if we can call the callback
 * directly from the current context or if we need to wakeup the
 * interrupt process.
 *------------------------------------------------------------------------*/
static void
usbd_callback_wrapper(struct usb_xfer_queue *pq)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper, entry);
	struct usb_xfer *xfer = pq->curr;
	struct usb_xfer_root *info = xfer->xroot;

	USB_BUS_LOCK_ASSERT(info->bus, MA_OWNED);
	if ((pq->recurse_3 != 0 || mtx_owned(info->xfer_mtx) == 0) &&
	    SCHEDULER_STOPPED() == 0) {
		/*
	       	 * Cases that end up here:
		 *
		 * 5) HW interrupt done callback or other source.
		 * 6) HW completed transfer during callback
		 */
		DPRINTFN(3, "case 5 and 6\n");

		/*
	         * We have to postpone the callback due to the fact we
	         * will have a Lock Order Reversal, LOR, if we try to
	         * proceed!
		 *
		 * Postponing the callback also ensures that other USB
		 * transfer queues get a chance.
	         */
		(void) usb_proc_msignal(info->done_p,
		    &info->done_m[0], &info->done_m[1]);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper, return);
		return;
	}
	/*
	 * Cases that end up here:
	 *
	 * 1) We are starting a transfer
	 * 2) We are prematurely calling back a transfer
	 * 3) We are stopping a transfer
	 * 4) We are doing an ordinary callback
	 */
	DPRINTFN(3, "case 1-4\n");
	
	info->done_q.curr = NULL;

	
	xfer->flags_int.doing_callback = 1;

	USB_BUS_UNLOCK(info->bus);
	USB_BUS_LOCK_ASSERT(info->bus, MA_NOTOWNED);

	
	if (!xfer->flags_int.transferring) {
		xfer->usb_state = USB_ST_SETUP;
		if (!xfer->flags_int.started) {
			
			USB_BUS_LOCK(info->bus);
			goto done;
		}
	} else {

		if (usbd_callback_wrapper_sub(xfer)) {
			
			USB_BUS_LOCK(info->bus);
			goto done;
		}
#if USB_HAVE_POWERD
		
		usbd_transfer_power_ref(xfer, -1);
#endif
		xfer->flags_int.transferring = 0;

		if (xfer->error) {
			xfer->usb_state = USB_ST_ERROR;
		} else {
			
			xfer->usb_state = USB_ST_TRANSFERRED;
#if USB_HAVE_BUSDMA
			
			if (xfer->flags_int.bdma_enable &&
			    (!xfer->flags_int.bdma_no_post_sync)) {
				usb_bdma_post_sync(xfer);
			}
#endif
		}
	}

#if USB_HAVE_PF
	if (xfer->usb_state != USB_ST_SETUP) {
		USB_BUS_LOCK(info->bus);
		usbpf_xfertap(xfer, USBPF_XFERTAP_DONE);
		USB_BUS_UNLOCK(info->bus);
	}
#endif
	
	(xfer->callback) (xfer, xfer->error);

	
	USB_BUS_LOCK(info->bus);

	/*
	 * Check if we got started after that we got cancelled, but
	 * before we managed to do the callback.
	 */
	if ((!xfer->flags_int.open) &&
	    (xfer->flags_int.started) &&
	    (xfer->usb_state == USB_ST_ERROR)) {
		
		xfer->flags_int.doing_callback = 0;
		
		usb_command_wrapper(&info->done_q, xfer);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper, return);
		return;
	}

done:
	
	xfer->flags_int.doing_callback = 0;

	/*
	 * Check if we are draining.
	 */
	if (xfer->flags_int.draining &&
	    (!xfer->flags_int.transferring)) {
		
		xfer->flags_int.draining = 0;
		cv_broadcast(&info->cv_drain);
	}

	
	usb_command_wrapper(&info->done_q,
	    info->done_q.curr);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper, return);
}

/*------------------------------------------------------------------------*
 *	usb_dma_delay_done_cb
 *
 * This function is called when the DMA delay has been exectuded, and
 * will make sure that the callback is called to complete the USB
 * transfer. This code path is usually only used when there is an USB
 * error like USB_ERR_CANCELLED.
 *------------------------------------------------------------------------*/
void
usb_dma_delay_done_cb(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_dma_delay_done_cb, entry);
	USB_BUS_LOCK_ASSERT(xfer->xroot->bus, MA_OWNED);

	DPRINTFN(3, "Completed %p\n", xfer);

	
	usbd_transfer_done(xfer, 0);
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_dma_delay_done_cb, return);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_dequeue
 *
 *  - This function is used to remove an USB transfer from a USB
 *  transfer queue.
 *
 *  - This function can be called multiple times in a row.
 *------------------------------------------------------------------------*/
void
usbd_transfer_dequeue(struct usb_xfer *xfer)
{
	struct usb_xfer_queue *pq;

	pq = xfer->wait_queue;

	SDT_PROBE4(tpw, kernel, usb_transfer_usbd_transfer_dequeue, entry, xfer, pq, xfer->endpointno, xfer->address);

	if (pq) {
		TAILQ_REMOVE(&pq->head, xfer, wait_entry);
		xfer->wait_queue = NULL;
	}
	SDT_PROBE4(tpw, kernel, usb_transfer_usbd_transfer_dequeue, return, xfer, pq, xfer->endpointno, xfer->address);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_enqueue
 *
 *  - This function is used to insert an USB transfer into a USB *
 *  transfer queue.
 *
 *  - This function can be called multiple times in a row.
 *------------------------------------------------------------------------*/
void
usbd_transfer_enqueue(struct usb_xfer_queue *pq, struct usb_xfer *xfer)
{
	SDT_PROBE4(tpw, kernel, usb_transfer_usbd_transfer_enqueue, entry, xfer, pq, xfer->endpointno, xfer->address);
	/*
	 * Insert the USB transfer into the queue, if it is not
	 * already on a USB transfer queue:
	 */
	if (xfer->wait_queue == NULL) {
		xfer->wait_queue = pq;
		TAILQ_INSERT_TAIL(&pq->head, xfer, wait_entry);
	}
	SDT_PROBE4(tpw, kernel, usb_transfer_usbd_transfer_enqueue, return, xfer, pq, xfer->endpointno, xfer->address);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_done
 *
 *  - This function is used to remove an USB transfer from the busdma,
 *  pipe or interrupt queue.
 *
 *  - This function is used to queue the USB transfer on the done
 *  queue.
 *
 *  - This function is used to stop any USB transfer timeouts.
 *------------------------------------------------------------------------*/
void
usbd_transfer_done(struct usb_xfer *xfer, usb_error_t error)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_done, entry);
	struct usb_xfer_root *info = xfer->xroot;

	USB_BUS_LOCK_ASSERT(info->bus, MA_OWNED);

	DPRINTF("err=%s\n", usbd_errstr(error));

	/*
	 * If we are not transferring then just return.
	 * This can happen during transfer cancel.
	 */
	if (!xfer->flags_int.transferring) {
		DPRINTF("not transferring\n");
		
		xfer->flags_int.control_act = 0;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_done, return);
		return;
	}
	
	if (xfer->error == USB_ERR_NORMAL_COMPLETION)
{
		xfer->error = error;
}

	
	usb_callout_stop(&xfer->timeout_handle);

	/*
	 * If we are waiting on a queue, just remove the USB transfer
	 * from the queue, if any. We should have the required locks
	 * locked to do the remove when this function is called.
	 */
	usbd_transfer_dequeue(xfer);

#if USB_HAVE_BUSDMA
	if (mtx_owned(info->xfer_mtx)) {
		struct usb_xfer_queue *pq;

		/*
		 * If the private USB lock is not locked, then we assume
		 * that the BUS-DMA load stage has been passed:
		 */
		pq = &info->dma_q;

		if (pq->curr == xfer) {
			
			usb_command_wrapper(pq, NULL);
		}
	}
#endif
	
	if (xfer->error) {
		info->bus->stats_err.uds_requests
		    [xfer->endpoint->edesc->bmAttributes & UE_XFERTYPE]++;
	} else {
		info->bus->stats_ok.uds_requests
		    [xfer->endpoint->edesc->bmAttributes & UE_XFERTYPE]++;
	}

	
	usbd_callback_ss_done_defer(xfer);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_done, return);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_start_cb
 *
 * This function is called to start the USB transfer when
 * "xfer->interval" is greater than zero, and and the endpoint type is
 * BULK or CONTROL.
 *------------------------------------------------------------------------*/
static void
usbd_transfer_start_cb(void *arg)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_start_cb, entry);
	struct usb_xfer *xfer = arg;
	struct usb_endpoint *ep = xfer->endpoint;

	USB_BUS_LOCK_ASSERT(xfer->xroot->bus, MA_OWNED);

	DPRINTF("start\n");

#if USB_HAVE_PF
	usbpf_xfertap(xfer, USBPF_XFERTAP_SUBMIT);
#endif

	
	xfer->flags_int.can_cancel_immed = 1;

	
	if (xfer->error == 0)
{
		(ep->methods->start) (xfer);
}

	
	if (xfer->error) {
		
		usbd_transfer_done(xfer, 0);
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_start_cb, return);
}

/*------------------------------------------------------------------------*
 *	usbd_xfer_set_stall
 *
 * This function is used to set the stall flag outside the
 * callback. This function is NULL safe.
 *------------------------------------------------------------------------*/
void
usbd_xfer_set_stall(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_stall, entry);
	if (xfer == NULL) {
		
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_stall, return);
		return;
	}
	USB_XFER_LOCK_ASSERT(xfer, MA_OWNED);

	
	USB_BUS_LOCK(xfer->xroot->bus);
	xfer->flags.stall_pipe = 1;
	USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_set_stall, return);
}

int
usbd_xfer_is_stalled(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_is_stalled, entry);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_xfer_is_stalled, return);
	return (xfer->endpoint->is_stalled);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_clear_stall
 *
 * This function is used to clear the stall flag outside the
 * callback. This function is NULL safe.
 *------------------------------------------------------------------------*/
void
usbd_transfer_clear_stall(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_clear_stall, entry);
	if (xfer == NULL) {
		
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_clear_stall, return);
		return;
	}
	USB_XFER_LOCK_ASSERT(xfer, MA_OWNED);

	
	USB_BUS_LOCK(xfer->xroot->bus);

	xfer->flags.stall_pipe = 0;

	USB_BUS_UNLOCK(xfer->xroot->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_clear_stall, return);
}

/*------------------------------------------------------------------------*
 *	usbd_pipe_start
 *
 * This function is used to add an USB transfer to the pipe transfer list.
 *------------------------------------------------------------------------*/
void
usbd_pipe_start(struct usb_xfer_queue *pq)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_start, entry);
	struct usb_endpoint *ep;
	struct usb_xfer *xfer;
	uint8_t type;

	xfer = pq->curr;
	ep = xfer->endpoint;

	USB_BUS_LOCK_ASSERT(xfer->xroot->bus, MA_OWNED);

	/*
	 * If the endpoint is already stalled we do nothing !
	 */
	if (ep->is_stalled) {
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_start, return);
		return;
	}
	/*
	 * Check if we are supposed to stall the endpoint:
	 */
	if (xfer->flags.stall_pipe) {
		struct usb_device *udev;
		struct usb_xfer_root *info;

		
		xfer->flags.stall_pipe = 0;

		
		info = xfer->xroot;
		udev = info->udev;

		/*
		 * Only stall BULK and INTERRUPT endpoints.
		 */
		type = (ep->edesc->bmAttributes & UE_XFERTYPE);
		if ((type == UE_BULK) ||
		    (type == UE_INTERRUPT)) {
			uint8_t did_stall;

			did_stall = 1;

			if (udev->flags.usb_mode == USB_MODE_DEVICE) {
				(udev->bus->methods->set_stall) (
				    udev, ep, &did_stall);
			} else if (udev->ctrl_xfer[1]) {
				info = udev->ctrl_xfer[1]->xroot;
				usb_proc_msignal(
				    USB_BUS_CS_PROC(info->bus),
				    &udev->cs_msg[0], &udev->cs_msg[1]);
			} else {
				
				DPRINTFN(0, "No stall handler\n");
			}
			/*
			 * Check if we should stall. Some USB hardware
			 * handles set- and clear-stall in hardware.
			 */
			if (did_stall) {
				/*
				 * The transfer will be continued when
				 * the clear-stall control endpoint
				 * message is received.
				 */
				ep->is_stalled = 1;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_start, return);
				return;
			}
		} else if (type == UE_ISOCHRONOUS) {

			/* 
			 * Make sure any FIFO overflow or other FIFO
			 * error conditions go away by resetting the
			 * endpoint FIFO through the clear stall
			 * method.
			 */
			if (udev->flags.usb_mode == USB_MODE_DEVICE) {
				(udev->bus->methods->clear_stall) (udev, ep);
			}
		}
	}
	
	if (xfer->nframes == 0) {
		
		xfer->aframes = 0;
		usbd_transfer_done(xfer, 0);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_start, return);
		return;
	}
	/*
	 * Handled cases:
	 *
	 * 1) Start the first transfer queued.
	 *
	 * 2) Re-start the current USB transfer.
	 */
	/*
	 * Check if there should be any
	 * pre transfer start delay:
	 */
	if (xfer->interval > 0) {
		type = (ep->edesc->bmAttributes & UE_XFERTYPE);
		if ((type == UE_BULK) ||
		    (type == UE_CONTROL)) {
			usbd_transfer_timeout_ms(xfer,
			    &usbd_transfer_start_cb,
			    xfer->interval);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_start, return);
			return;
		}
	}
	DPRINTF("start\n");

#if USB_HAVE_PF
	usbpf_xfertap(xfer, USBPF_XFERTAP_SUBMIT);
#endif
	
	xfer->flags_int.can_cancel_immed = 1;

	
	if (xfer->error == 0)
{
		(ep->methods->start) (xfer);
}

	
	if (xfer->error) {
		
		usbd_transfer_done(xfer, 0);
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_pipe_start, return);
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_timeout_ms
 *
 * This function is used to setup a timeout on the given USB
 * transfer. If the timeout has been deferred the callback given by
 * "cb" will get called after "ms" milliseconds.
 *------------------------------------------------------------------------*/
void
usbd_transfer_timeout_ms(struct usb_xfer *xfer,
    void (*cb) (void *arg), usb_timeout_t ms)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_timeout_ms, entry);
	USB_BUS_LOCK_ASSERT(xfer->xroot->bus, MA_OWNED);

	
	usb_callout_reset(&xfer->timeout_handle,
	    USB_MS_TO_TICKS(ms) + USB_CALLOUT_ZERO_TICKS, cb, xfer);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_transfer_timeout_ms, return);
}

/*------------------------------------------------------------------------*
 *	usbd_callback_wrapper_sub
 *
 *  - This function will update variables in an USB transfer after
 *  that the USB transfer is complete.
 *
 *  - This function is used to start the next USB transfer on the
 *  ep transfer queue, if any.
 *
 * NOTE: In some special cases the USB transfer will not be removed from
 * the pipe queue, but remain first. To enforce USB transfer removal call
 * this function passing the error code "USB_ERR_CANCELLED".
 *
 * Return values:
 * 0: Success.
 * Else: The callback has been deferred.
 *------------------------------------------------------------------------*/
static uint8_t
usbd_callback_wrapper_sub(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper_sub, entry);
	struct usb_endpoint *ep;
	struct usb_bus *bus;
	usb_frcount_t x;

	bus = xfer->xroot->bus;

	if ((!xfer->flags_int.open) &&
	    (!xfer->flags_int.did_close)) {
		DPRINTF("close\n");
		USB_BUS_LOCK(bus);
		(xfer->endpoint->methods->close) (xfer);
		USB_BUS_UNLOCK(bus);
		
		xfer->flags_int.did_close = 1;
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper_sub, return);
		return (1);		
	}
	/*
	 * If we have a non-hardware induced error we
	 * need to do the DMA delay!
	 */
	if (xfer->error != 0 && !xfer->flags_int.did_dma_delay &&
	    (xfer->error == USB_ERR_CANCELLED ||
	    xfer->error == USB_ERR_TIMEOUT ||
	    bus->methods->start_dma_delay != NULL)) {

		usb_timeout_t temp;

		
		xfer->flags_int.did_dma_delay = 1;

		
		xfer->flags_int.can_cancel_immed = 0;

		temp = usbd_get_dma_delay(xfer->xroot->udev);

		DPRINTFN(3, "DMA delay, %u ms, "
		    "on %p\n", temp, xfer);

		if (temp != 0) {
			USB_BUS_LOCK(bus);
			/*
			 * Some hardware solutions have dedicated
			 * events when it is safe to free DMA'ed
			 * memory. For the other hardware platforms we
			 * use a static delay.
			 */
			if (bus->methods->start_dma_delay != NULL) {
				(bus->methods->start_dma_delay) (xfer);
			} else {
				usbd_transfer_timeout_ms(xfer,
				    (void (*)(void *))&usb_dma_delay_done_cb,
				    temp);
			}
			USB_BUS_UNLOCK(bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper_sub, return);
			return (1);	
		}
	}
	
	if (xfer->aframes > xfer->nframes) {
		if (xfer->error == 0) {
			panic("%s: actual number of frames, %d, is "
			    "greater than initial number of frames, %d\n",
			    __FUNCTION__, xfer->aframes, xfer->nframes);
		} else {
			
			xfer->aframes = xfer->nframes;
		}
	}
	
	xfer->actlen = 0;

	for (x = 0; x != xfer->aframes; x++) {
		xfer->actlen += xfer->frlengths[x];
	}

	/*
	 * Frames that were not transferred get zero actual length in
	 * case the USB device driver does not check the actual number
	 * of frames transferred, "xfer->aframes":
	 */
	for (; x < xfer->nframes; x++) {
		usbd_xfer_set_frame_len(xfer, x, 0);
	}

	
	if (xfer->actlen > xfer->sumlen) {
		if (xfer->error == 0) {
			panic("%s: actual length, %d, is greater than "
			    "initial length, %d\n",
			    __FUNCTION__, xfer->actlen, xfer->sumlen);
		} else {
			
			xfer->actlen = xfer->sumlen;
		}
	}
	DPRINTFN(1, "xfer=%p endpoint=%p sts=%d alen=%d, slen=%d, afrm=%d, nfrm=%d\n",
	    xfer, xfer->endpoint, xfer->error, xfer->actlen, xfer->sumlen,
	    xfer->aframes, xfer->nframes);

	if (xfer->error) {
		
		xfer->flags_int.control_act = 0;

#if USB_HAVE_TT_SUPPORT
		switch (xfer->error) {
		case USB_ERR_NORMAL_COMPLETION:
		case USB_ERR_SHORT_XFER:
		case USB_ERR_STALLED:
		case USB_ERR_CANCELLED:
			
			break;
		default:
			
			USB_BUS_LOCK(bus);
			uhub_tt_buffer_reset_async_locked(xfer->xroot->udev, xfer->endpoint);
			USB_BUS_UNLOCK(bus);
			break;
		}
#endif
		
		if ((xfer->error != USB_ERR_CANCELLED) &&
		    (xfer->flags.pipe_bof)) {
			DPRINTFN(2, "xfer=%p: Block On Failure "
			    "on endpoint=%p\n", xfer, xfer->endpoint);
			goto done;
		}
	} else {
		
		if (xfer->actlen < xfer->sumlen) {

			
			xfer->flags_int.control_act = 0;

			if (!xfer->flags_int.short_xfer_ok) {
				xfer->error = USB_ERR_SHORT_XFER;
				if (xfer->flags.pipe_bof) {
					DPRINTFN(2, "xfer=%p: Block On Failure on "
					    "Short Transfer on endpoint %p.\n",
					    xfer, xfer->endpoint);
					goto done;
				}
			}
		} else {
			/*
			 * Check if we are in the middle of a
			 * control transfer:
			 */
			if (xfer->flags_int.control_act) {
				DPRINTFN(5, "xfer=%p: Control transfer "
				    "active on endpoint=%p\n", xfer, xfer->endpoint);
				goto done;
			}
		}
	}

	ep = xfer->endpoint;

	/*
	 * If the current USB transfer is completing we need to start the
	 * next one:
	 */
	USB_BUS_LOCK(bus);
	if (ep->endpoint_q[xfer->stream_id].curr == xfer) {
		usb_command_wrapper(&ep->endpoint_q[xfer->stream_id], NULL);

		if (ep->endpoint_q[xfer->stream_id].curr != NULL ||
		    TAILQ_FIRST(&ep->endpoint_q[xfer->stream_id].head) != NULL) {
			
		} else {
			
			
			xfer->endpoint->is_synced = 0;
		}
	}
	USB_BUS_UNLOCK(bus);
done:
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_callback_wrapper_sub, return);
	return (0);
}

/*------------------------------------------------------------------------*
 *	usb_command_wrapper
 *
 * This function is used to execute commands non-recursivly on an USB
 * transfer.
 *------------------------------------------------------------------------*/
void
usb_command_wrapper(struct usb_xfer_queue *pq, struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_command_wrapper, entry);
	if (xfer) {
		/*
		 * If the transfer is not already processing,
		 * queue it!
		 */
		if (pq->curr != xfer) {
			usbd_transfer_enqueue(pq, xfer);
			if (pq->curr != NULL) {
				
				DPRINTFN(6, "busy %p\n", pq->curr);
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_command_wrapper, return);
				return;
			}
		}
	} else {
		
		pq->curr = NULL;
	}

	if (!pq->recurse_1) {

		
		pq->recurse_3 = 0;

		do {
			
			pq->recurse_1 = 1;
			pq->recurse_2 = 1;

			if (pq->curr == NULL) {
				xfer = TAILQ_FIRST(&pq->head);
				if (xfer) {
					TAILQ_REMOVE(&pq->head, xfer,
					    wait_entry);
					xfer->wait_queue = NULL;
					pq->curr = xfer;
				} else {
					break;
				}
			}
			DPRINTFN(6, "cb %p (enter)\n", pq->curr);
			(pq->command) (pq);
			DPRINTFN(6, "cb %p (leave)\n", pq->curr);

			/*
			 * Set third recurse flag to indicate
			 * recursion happened:
			 */
			pq->recurse_3 = 1;

		} while (!pq->recurse_2);

		
		pq->recurse_1 = 0;

	} else {
		
		pq->recurse_2 = 0;
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usb_command_wrapper, return);
}

/*------------------------------------------------------------------------*
 *	usbd_ctrl_transfer_setup
 *
 * This function is used to setup the default USB control endpoint
 * transfer.
 *------------------------------------------------------------------------*/
void
usbd_ctrl_transfer_setup(struct usb_device *udev)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_ctrl_transfer_setup, entry);
	struct usb_xfer *xfer;
	uint8_t no_resetup;
	uint8_t iface_index;

	
	if (udev->parent_hub == NULL)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_ctrl_transfer_setup, return);
		return;
}
repeat:

	xfer = udev->ctrl_xfer[0];
	if (xfer) {
		USB_XFER_LOCK(xfer);
		no_resetup =
		    ((xfer->address == udev->address) &&
		    (udev->ctrl_ep_desc.wMaxPacketSize[0] ==
		    udev->ddesc.bMaxPacketSize));
		if (udev->flags.usb_mode == USB_MODE_DEVICE) {
			if (no_resetup) {
				/*
				 * NOTE: checking "xfer->address" and
				 * starting the USB transfer must be
				 * atomic!
				 */
				usbd_transfer_start(xfer);
			}
		}
		USB_XFER_UNLOCK(xfer);
	} else {
		no_resetup = 0;
	}

	if (no_resetup) {
		/*
	         * All parameters are exactly the same like before.
	         * Just return.
	         */
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_ctrl_transfer_setup, return);
		return;
	}
	/*
	 * Update wMaxPacketSize for the default control endpoint:
	 */
	udev->ctrl_ep_desc.wMaxPacketSize[0] =
	    udev->ddesc.bMaxPacketSize;

	/*
	 * Unsetup any existing USB transfer:
	 */
	usbd_transfer_unsetup(udev->ctrl_xfer, USB_CTRL_XFER_MAX);

	/*
	 * Reset clear stall error counter.
	 */
	udev->clear_stall_errors = 0;

	/*
	 * Try to setup a new USB transfer for the
	 * default control endpoint:
	 */
	iface_index = 0;
	if (usbd_transfer_setup(udev, &iface_index,
	    udev->ctrl_xfer, usb_control_ep_cfg, USB_CTRL_XFER_MAX, NULL,
	    &udev->device_mtx)) {
		DPRINTFN(0, "could not setup default "
		    "USB transfer\n");
	} else {
		goto repeat;
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_ctrl_transfer_setup, return);
}

/*------------------------------------------------------------------------*
 *	usbd_clear_data_toggle - factored out code
 *
 * NOTE: the intention of this function is not to reset the hardware
 * data toggle.
 *------------------------------------------------------------------------*/
void
usbd_clear_stall_locked(struct usb_device *udev, struct usb_endpoint *ep)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_locked, entry);
	USB_BUS_LOCK_ASSERT(udev->bus, MA_OWNED);

	
	if (udev->flags.usb_mode == USB_MODE_HOST &&
	    udev->parent_hub != NULL &&
	    udev->bus->methods->clear_stall != NULL &&
	    ep->methods != NULL) {
		(udev->bus->methods->clear_stall) (udev, ep);
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_locked, return);
}

/*------------------------------------------------------------------------*
 *	usbd_clear_data_toggle - factored out code
 *
 * NOTE: the intention of this function is not to reset the hardware
 * data toggle on the USB device side.
 *------------------------------------------------------------------------*/
void
usbd_clear_data_toggle(struct usb_device *udev, struct usb_endpoint *ep)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_data_toggle, entry);
	DPRINTFN(5, "udev=%p endpoint=%p\n", udev, ep);

	USB_BUS_LOCK(udev->bus);
	ep->toggle_next = 0;
	
	usbd_clear_stall_locked(udev, ep);
	USB_BUS_UNLOCK(udev->bus);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_data_toggle, return);
}

/*------------------------------------------------------------------------*
 *	usbd_clear_stall_callback - factored out clear stall callback
 *
 * Input parameters:
 *  xfer1: Clear Stall Control Transfer
 *  xfer2: Stalled USB Transfer
 *
 * This function is NULL safe.
 *
 * Return values:
 *   0: In progress
 *   Else: Finished
 *
 * Clear stall config example:
 *
 * static const struct usb_config my_clearstall =  {
 *	.type = UE_CONTROL,
 *	.endpoint = 0,
 *	.direction = UE_DIR_ANY,
 *	.interval = 50, //50 milliseconds
 *	.bufsize = sizeof(struct usb_device_request),
 *	.timeout = 1000, //1.000 seconds
 *	.callback = &my_clear_stall_callback, // **
 *	.usb_mode = USB_MODE_HOST,
 * };
 *
 * ** "my_clear_stall_callback" calls "usbd_clear_stall_callback"
 * passing the correct parameters.
 *------------------------------------------------------------------------*/
uint8_t
usbd_clear_stall_callback(struct usb_xfer *xfer1,
    struct usb_xfer *xfer2)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, entry);
	struct usb_device_request req;

	if (xfer2 == NULL) {
		
		DPRINTF("NULL input parameter\n");
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
		return (0);
	}
	USB_XFER_LOCK_ASSERT(xfer1, MA_OWNED);
	USB_XFER_LOCK_ASSERT(xfer2, MA_OWNED);

	switch (USB_GET_STATE(xfer1)) {
	case USB_ST_SETUP:

		/*
		 * pre-clear the data toggle to DATA0 ("umass.c" and
		 * "ata-usb.c" depends on this)
		 */

		usbd_clear_data_toggle(xfer2->xroot->udev, xfer2->endpoint);

		

		req.bmRequestType = UT_WRITE_ENDPOINT;
		req.bRequest = UR_CLEAR_FEATURE;
		USETW(req.wValue, UF_ENDPOINT_HALT);
		req.wIndex[0] = xfer2->endpoint->edesc->bEndpointAddress;
		req.wIndex[1] = 0;
		USETW(req.wLength, 0);

		/*
		 * "usbd_transfer_setup_sub()" will ensure that
		 * we have sufficient room in the buffer for
		 * the request structure!
		 */

		

		usbd_copy_in(xfer1->frbuffers, 0, &req, sizeof(req));

		
		xfer1->frlengths[0] = sizeof(req);
		xfer1->nframes = 1;

		usbd_transfer_submit(xfer1);
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
		return (0);

	case USB_ST_TRANSFERRED:
		break;

	default:			
		if (xfer1->error == USB_ERR_CANCELLED) {
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
			return (0);
		}
		break;
	}
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
	return (1);			
}

/*------------------------------------------------------------------------*
 *	usbd_transfer_poll
 *
 * The following function gets called from the USB keyboard driver and
 * UMASS when the system has paniced.
 *
 * NOTE: It is currently not possible to resume normal operation on
 * the USB controller which has been polled, due to clearing of the
 * "up_dsleep" and "up_msleep" flags.
 *------------------------------------------------------------------------*/
void
usbd_transfer_poll(struct usb_xfer **ppxfer, uint16_t max)
{
	struct usb_xfer *xfer;
	struct usb_xfer_root *xroot;
	struct usb_device *udev;
	struct usb_proc_msg *pm;
	uint16_t n;
	uint16_t drop_bus;
	uint16_t drop_xfer;

	for (n = 0; n != max; n++) {
		
		xfer = ppxfer[n];
		if (xfer == NULL)
{
			continue;	
}
		xroot = xfer->xroot;
		if (xroot == NULL)
{
			continue;	
}
		udev = xroot->udev;
		if (udev == NULL)
{
			continue;	
}
		if (udev->bus == NULL)
{
			continue;	
}
		if (udev->bus->methods == NULL)
{
			continue;	
}
		if (udev->bus->methods->xfer_poll == NULL)
{
			continue;	
}

		
		drop_bus = 0;
		while (mtx_owned(&xroot->udev->bus->bus_mtx) && !SCHEDULER_STOPPED()) {
			mtx_unlock(&xroot->udev->bus->bus_mtx);
			drop_bus++;
		}

		
		drop_xfer = 0;
		while (mtx_owned(xroot->xfer_mtx) && !SCHEDULER_STOPPED()) {
			mtx_unlock(xroot->xfer_mtx);
			drop_xfer++;
		}

		
		USB_BUS_CONTROL_XFER_PROC(udev->bus)->up_msleep = 0;
		USB_BUS_EXPLORE_PROC(udev->bus)->up_msleep = 0;
		USB_BUS_GIANT_PROC(udev->bus)->up_msleep = 0;
		USB_BUS_NON_GIANT_ISOC_PROC(udev->bus)->up_msleep = 0;
		USB_BUS_NON_GIANT_BULK_PROC(udev->bus)->up_msleep = 0;

		
		(udev->bus->methods->xfer_poll) (udev->bus);

		USB_BUS_LOCK(xroot->bus);

		
		if (udev->ctrl_xfer[1] != NULL) {

			
			pm = &udev->cs_msg[0].hdr;
			(pm->pm_callback) (pm);
			
			pm = &udev->ctrl_xfer[1]->
			    xroot->done_m[0].hdr;
			(pm->pm_callback) (pm);
		}

		
		pm = &xroot->done_m[0].hdr;
		(pm->pm_callback) (pm);

		USB_BUS_UNLOCK(xroot->bus);

		
		while (drop_xfer--)
			mtx_lock(xroot->xfer_mtx);

		
		while (drop_bus--)
			mtx_lock(&xroot->udev->bus->bus_mtx);
	}
}

static void
usbd_get_std_packet_size(struct usb_std_packet_size *ptr,
    uint8_t type, enum usb_dev_speed speed)
{
	static const uint16_t intr_range_max[USB_SPEED_MAX] = {
		[USB_SPEED_LOW] = 8,
		[USB_SPEED_FULL] = 64,
		[USB_SPEED_HIGH] = 1024,
		[USB_SPEED_VARIABLE] = 1024,
		[USB_SPEED_SUPER] = 1024,
	};

	static const uint16_t isoc_range_max[USB_SPEED_MAX] = {
		[USB_SPEED_LOW] = 0,	
		[USB_SPEED_FULL] = 1023,
		[USB_SPEED_HIGH] = 1024,
		[USB_SPEED_VARIABLE] = 3584,
		[USB_SPEED_SUPER] = 1024,
	};

	static const uint16_t control_min[USB_SPEED_MAX] = {
		[USB_SPEED_LOW] = 8,
		[USB_SPEED_FULL] = 8,
		[USB_SPEED_HIGH] = 64,
		[USB_SPEED_VARIABLE] = 512,
		[USB_SPEED_SUPER] = 512,
	};

	static const uint16_t bulk_min[USB_SPEED_MAX] = {
		[USB_SPEED_LOW] = 8,
		[USB_SPEED_FULL] = 8,
		[USB_SPEED_HIGH] = 512,
		[USB_SPEED_VARIABLE] = 512,
		[USB_SPEED_SUPER] = 1024,
	};

	uint16_t temp;

	memset(ptr, 0, sizeof(*ptr));

	switch (type) {
	case UE_INTERRUPT:
		ptr->range.max = intr_range_max[speed];
		break;
	case UE_ISOCHRONOUS:
		ptr->range.max = isoc_range_max[speed];
		break;
	default:
		if (type == UE_BULK)
{
			temp = bulk_min[speed];
}
		else 
			temp = control_min[speed];

		
		ptr->fixed[0] = temp;
		ptr->fixed[1] = temp;
		ptr->fixed[2] = temp;
		ptr->fixed[3] = temp;

		if (speed == USB_SPEED_FULL) {
			
			ptr->fixed[1] = 16;
			ptr->fixed[2] = 32;
			ptr->fixed[3] = 64;
		}
		if ((speed == USB_SPEED_VARIABLE) &&
		    (type == UE_BULK)) {
			
			ptr->fixed[2] = 1024;
			ptr->fixed[3] = 1536;
		}
		break;
	}
}

void	*
usbd_xfer_softc(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
	return (xfer->priv_sc);
}

void *
usbd_xfer_get_priv(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
	return (xfer->priv_fifo);
}

void
usbd_xfer_set_priv(struct usb_xfer *xfer, void *ptr)
{
	xfer->priv_fifo = ptr;
}

uint8_t
usbd_xfer_state(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
	return (xfer->usb_state);
}

void
usbd_xfer_set_flag(struct usb_xfer *xfer, int flag)
{
	switch (flag) {
		case USB_FORCE_SHORT_XFER:
			xfer->flags.force_short_xfer = 1;
			break;
		case USB_SHORT_XFER_OK:
			xfer->flags.short_xfer_ok = 1;
			break;
		case USB_MULTI_SHORT_OK:
			xfer->flags.short_frames_ok = 1;
			break;
		case USB_MANUAL_STATUS:
			xfer->flags.manual_status = 1;
			break;
	}
}

void
usbd_xfer_clr_flag(struct usb_xfer *xfer, int flag)
{
	switch (flag) {
		case USB_FORCE_SHORT_XFER:
			xfer->flags.force_short_xfer = 0;
			break;
		case USB_SHORT_XFER_OK:
			xfer->flags.short_xfer_ok = 0;
			break;
		case USB_MULTI_SHORT_OK:
			xfer->flags.short_frames_ok = 0;
			break;
		case USB_MANUAL_STATUS:
			xfer->flags.manual_status = 0;
			break;
	}
}

/*
 * The following function returns in milliseconds when the isochronous
 * transfer was completed by the hardware. The returned value wraps
 * around 65536 milliseconds.
 */
uint16_t
usbd_xfer_get_timestamp(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
	return (xfer->isoc_time_complete);
}

/*
 * The following function returns non-zero if the max packet size
 * field was clamped to a valid value. Else it returns zero.
 */
uint8_t
usbd_xfer_maxp_was_clamped(struct usb_xfer *xfer)
{
	SDT_PROBE0(tpw, kernel, usb_transfer_usbd_clear_stall_callback, return);
	return (xfer->flags_int.maxp_was_clamped);
}
