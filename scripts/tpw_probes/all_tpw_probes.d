dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:kernel:usb_transfer_usbd_transfer_enqueue:entry,
tpw:kernel:usb_transfer_usbd_transfer_dequeue:entry,
tpw:kernel:usb_transfer_usbd_transfer_enqueue:return,
tpw:kernel:usb_transfer_usbd_transfer_dequeue:return
{
	printf("%s\t%d\t%s\t%p\t%p\t%d\t%d\t", execname, pid, probefunc, arg0, arg1, arg2, arg3);
}

tpw:kernel::
{
	printf("%s\t%d\t%s\t", execname, pid, probefunc);
}

dtrace:::END
{
	trace("End");
}
