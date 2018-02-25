dtrace:::BEGIN
{
	i = 0;
}

syscall::read:entry,
syscall::write:entry
/pid == 2219/
{
	trace(++i);
}
