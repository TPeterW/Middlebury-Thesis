dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:kernel::
{
	printf("%s\t%d\t%d\t%s\t%p\t%p\t", execname, pid, ppid, probefunc, arg0, arg1);
}

dtrace:::END
{
	trace("End");
}
