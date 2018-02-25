dtrace:::BEGIN
{
	i = 10;
}

profile:::tick-1sec
/i > 0/
{
	trace(i--);
}

profile:::tick-1sec
/i == 0/
{
	trace("Blastoff!");
	exit(0);
}
