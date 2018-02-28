#pragma D option flowindent

fbt:::entry
/strstr(probefunc, "usb") != 0 && guard++ == 0/
{
	self->traceme = 1;
	printf("fd: %d", arg0);
}

fbt:::
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
