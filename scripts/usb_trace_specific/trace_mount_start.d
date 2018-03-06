#pragma D option flowindent

fbt:kernel::entry
/probefunc == "usbd_transfer_start" && guard++ == 0/
{
	self->traceme = 1;
	printf("fd: %d", arg0);
}

tpw:kernel::,
fbt:kernel::
/self->traceme/
{
	printf("tid: %d", tid);
}

fbt:::return
/self->traceme/
{
	printf("tid: %d", tid);
	self->traceme = 0;
	exit(0);
}
