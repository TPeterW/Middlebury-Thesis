#include <sys/sdt.h>

#ifndef USB_TPW_PROBE

// SDT_PROVIDER_DEFINE(tpw);
// SDT_PROBE_DEFINE2(tpw, kernel, usb_ethernet, entry, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, usb_ethernet, return, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, if_smsc, entry, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, if_smsc, return, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, if_cdce, entry, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, if_cdce, return, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, if_urndis, entry, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, if_urndis, return, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, usb_transfer_enqueue, entry, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, usb_transfer_enqueue, return, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, usb_transfer_dequeue, entry, "int", "int");
// SDT_PROBE_DEFINE2(tpw, kernel, usb_transfer_dequeue, return, "int", "int");


#define USB_TPW_PROBE
#endif