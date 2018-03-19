dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:kernel::entry,
fbt:kernel::entry
/strstr(probefunc, "usb") != 0 || probeprov == "tpw"/
{
	printf("%s\t %d\t %d\t %s\t %d\n", execname, pid, ppid, probeprov, timestamp);
}

dtrace:::END
{
	trace("End");
}
