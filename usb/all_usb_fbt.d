dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

fbt:kernel::entry
/probeprov == "fbt" && strstr(probefunc, "usb") != 0/
{
	printf("%s\t %d\t %d\t %s\n", execname, pid, ppid, probefunc);
}

dtrace:::END
{
	trace("End");
}
