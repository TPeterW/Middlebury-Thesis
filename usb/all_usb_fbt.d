dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

fbt:kernel::entry
/strstr(probefunc, "usb") != 0/
{
	
	printf("%s\t %s\t %d\t %d\n", probefunc, execname, pid, ppid);
}

dtrace:::END
{
	trace("End");
}
