/*
 * Count off and report the number of seconds elapsed
 */
dtrace:::BEGIN
{
	i = 0;
}

profile:::tick-1sec
{
	trace(++i);
}

dtrace:::END
{
	trace(i);
}
