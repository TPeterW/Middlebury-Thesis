dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:kernel::entry
{
	printf("%s\t%d\t%d\t%s\t%d\t%d\t", execname, pid, ppid, probefunc, arg1, arg2);
}

dtrace:::END
{
	trace("End");
}
