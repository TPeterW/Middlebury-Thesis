dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:kernel::
{
	printf("%s\t%d\t%d\t%s\t%d\t", execname, pid, ppid, probefunc, timestamp);
}

dtrace:::END
{
	trace("End");
}
