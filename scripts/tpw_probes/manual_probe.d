dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:kernel::entry
{
	trace("Found it! Found it!!!\n");
}

dtrace:::END
{
	trace("End listening...\n");
}
