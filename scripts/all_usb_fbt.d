dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:kernel::entry,
fbt:kernel::entry
/strstr(probefunc, "usb") != 0/
{
	printf("%s\t %d\t %d\t %s\t %d\t %d\n", execname, pid, ppid, probefunc, arg0, arg1);
}

dtrace:::END
{
	trace("End");
}
