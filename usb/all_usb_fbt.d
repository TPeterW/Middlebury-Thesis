dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

fbt:kernel::entry
/ (id >= 11321 && id <= 11355) || (id >= 11421 && id <= 11494) || (id >= 11526 && id <= 11530) || (id >= 11556 && id <= 11563) || (id >= 11581 && id <= 11604) /
{
	printf("%s\t %s\t %d\t %d\n", probefunc, execname, pid, ppid);
}

fbt:::return
{
	
}

dtrace:::END
{
	trace("End");
}
