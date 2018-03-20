#include <sys/sdt.h>
#ifndef USB_TPW_PROBE
SDT_PROVIDER_DEFINE(tpw);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usbd_get_endpoint, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_done, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_iface, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_start_read, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_do_request_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_m_copy_in_cb, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_open, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_raw_write_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_uninit, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_tag_destroy, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_do_request_proc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_open_pipe_write, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_ctrl_write_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_isoc_write_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_byte, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_wr_cs_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_lock_cb, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_msignal, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_switch_rid, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_write_clear_stall_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_byte, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_pc_free_mem, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_rewakeup, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_statestr, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_make_str_desc, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_out, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_rewakeup, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_mwait, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_read_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_frame_zero, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_make_str_desc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_fifo_open, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_get_max_lun, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_lookup_usbd_lookup_id_by_info, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_is_gone, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_item, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_intr_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_close, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_uninit, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_data, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_device_set_usb_desc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_clone_create, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_raw_write_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_device, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_item, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_isoc_write_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_get_page, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_pc_alloc_mem, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_tag_setup, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_attach, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_command_start, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_open, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usbd_get_endpoint, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_uiomove, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_tag_create, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_transfer_setup, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_report_size, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_usb_ref_device, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_process, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_quirk_unload, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_get_desc_w, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_reset_tt_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_xfer, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_open_pipe_read, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_count_active_host_ports, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_aggregate_xferflags, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_start_write, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_command_start, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_free, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_out, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_end_parse, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_detach, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_explore_sub, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_wr_cs_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_detach, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_get_serial, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_frame_zero, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_transfer_start, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_eject, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_get_manufacturer, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_transfer_setup_has_bulk, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_xfer, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_read_clear_stall_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_is_called_from, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub_malloc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_xfertap, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_explore_handle_re_enumerate, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_ctrl_read_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_get_string_any, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_ifname2ubus, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_read_port_status, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_usb_device_20_compatible, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_in, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_handle_set_stall, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_get_dma_delay, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_pause_mtx, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_attach, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_statestr, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_setup_by_index_w, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usb_request_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_update_max_frame_size, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_is_called_from, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_trim_spaces, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_descriptor_from_usb, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_fifo_free, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_get_string_any, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_parse_usb_idesc_foreach, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_free, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_printbcd, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_transfer_setup, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_attach, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usb_request_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_reset_port, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_m_copy_in_cb, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_uiomove, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_warm_reset_port, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_trim_spaces, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_transfer_start, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_read_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_command_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_dymo_eject, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_get_desc_w, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_clone_destroy, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_out_user, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_pause_mtx, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_linux_unload, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_read_port_status, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_detach, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_device, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_is_gone, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_get_debug_bits, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_parse_usb_idesc_foreach, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_linux_unload, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_queue, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_handle_iface_request, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_get_page, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_lookup_usbd_lookup_id_by_uaa, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_start_parse, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_read_10, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_check_alt_setting, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_raw_write, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_get_max_lun, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_end_parse, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_detach, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_clone_match, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_command_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_pc_cpu_invalidate, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_transfer_setup_has_bulk, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_open_pipe_read, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_check_alt_setting, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_unload, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usbd_interface_count, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_switch_rid, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_eject, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_fifo_create, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_test_quirk_w, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_set_config, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_stop_io, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_status_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_write_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_transfer_setup_sub, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_endpoint, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_close, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_get_dma_delay, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_set_config, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_queue, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_rd_cs_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_loc_fill, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_get_product, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_write_10, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_dymo_eject, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_endpoint, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_xfertap, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_fifo_open, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_quirk_ioctl_w, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_is_collection, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_mwait, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_clone_match, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_drain, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_attach, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_test_quirk_w, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_handle_request_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_read_capacity, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_debug_usb_dump_iface, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_lookup_usbd_lookup_id_by_uaa, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_aggregate_status, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_reset_tt_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_aggregate_status, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_in, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_is_keyboard, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_start_parse, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_data_sub, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_get_manufacturer, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_m_copy_in, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_root_intr, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_locate, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_intr_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_root_intr, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_ref_device, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_usbd_req_get_hid_desc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_clear_local, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_usbd_req_get_hid_desc, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_init, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_isoc_read_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usb_do_clear_stall_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_do_request_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_transfer_usbd_update_max_frame_size, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_open_pipe_write, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_get_product, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_process, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_fifo_create, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_usb_printbcd, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_util_device_set_usb_desc, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_warm_reset_port, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_lock_cb, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_tag_destroy, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_unref_device, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_start_read, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_open, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_mbuf_usb_alloc_mbufs, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_ctrl_read_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_mbuf_usb_alloc_mbufs, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_locate, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_descriptor_from_usb, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_get_desc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_msignal, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_get_string_desc, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_pc_buffer_is_aligned, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_data_sub, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_fifo_alloc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_drain, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usb_do_clear_stall_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_get_string_desc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_is_keyboard, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_bus_unload, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_pc_free_mem, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_usb_device_20_compatible, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_read_10, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_unsetup_w, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_tt_buffer_reset_async_locked, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_pc_buffer_is_aligned, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_open, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_unref_device, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_quirk_unload, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_xfer_frame_is_read, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_pc_cpu_invalidate, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_get_debug_bits, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_stop_io, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_read_capacity, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_xfer_precompute_size, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_probe, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usbd_interface_count, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_linux_free_device_w, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_probe, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_parse_usb_desc_foreach, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_error_usbd_errstr, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_read_clear_stall_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_in_user, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_clear_stall_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_write_clear_stall_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_isoc_read_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_setup_by_index_w, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_re_enumerate, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_ioctl, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_rd_cs_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_auto_quirk, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_reset_tt_proc, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_handle_set_config, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_m_copy_in, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_usb_ref_device, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_handle_request_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_reset_tt_proc, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_ctrl_write_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_iface_is_cdrom, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_out_user, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_quirk_ioctl_w, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_aggregate_xferflags, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_explore_handle_re_enumerate, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_init, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usbd_get_ep_by_addr, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_clear_stall_callback, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_is_collection, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_handle_set_config, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_raw_write, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_handle_request_usb_handle_set_stall, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_get_data, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_data_write_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_clone_create, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_unload, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_loc_fill, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_clone_destroy, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_done, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_req_re_enumerate, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_ioctl, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_get_hr_func, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_linux_free_device_w, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_error_usbd_errstr, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_pf_usbpf_ifname2ubus, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_tag_setup, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hub_uhub_tt_buffer_reset_async_locked, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_hid_hid_clear_local, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_generic_ugen_start_write, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usbd_copy_in_user, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_busdma_usb_dma_tag_create, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_auto_quirk, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_bbb_status_callback, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dev_usb_fifo_free, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_bus_unload, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_msc_write_10, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_process_usb_proc_create, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_dynamic_usb_temp_unsetup_w, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_request_usbd_get_hr_func, entry);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_msctest_usb_iface_is_cdrom, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_parse_usb_desc_foreach, return);
	SDT_PROBE_DEFINE0(tpw, kernel, usb_device_usb_get_serial, entry);
#define USB_TPW_PROBE
#endif
