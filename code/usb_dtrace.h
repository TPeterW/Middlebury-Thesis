#include <sys/sdt.h>

#ifndef _TPW_USB_DTRACE_

#define TPW_DEFINE_PROVIDER(P)			SDT_PROVIDER_DEFINE(P)
#define TPW_DECLARE_PROVIDER(P)			SDT_PROVIDER_DECLARE(P)

#define TPW_DEFINE_PROBE(P, N, A)		SDT_PROBE_DEFINE##N (tpw, kernel, P, A)
#define TPW_DECLARE_PROBE(P, N, A)		  

#define _TPW_USB_DTRACE_
