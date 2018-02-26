dtrace:::BEGIN
{
	trace("Begin listening...\n");
}

tpw:::entry
{
	trace("Found it! Found it!!!\n");
}

dtrace:::END
{
	trace("End listening...\n");
}
