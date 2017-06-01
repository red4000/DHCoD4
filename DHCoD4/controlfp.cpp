// TODO: cache control word in each function if it's not cached, and if it is cached then restore it and uncache.

void set24bitpc()
{
	unsigned short cw;
	__asm
	{
		push eax
		fnstcw cw
		fwait
		mov ax, cw
		and ax, ~0x300 
		mov cw, ax
		fldcw cw
		pop eax
	}
}

void set64bitpc()
{
	unsigned short cw;
	__asm
	{
		push eax
		fnstcw cw
		fwait
		mov ax, cw
		or ax, 0x300 
		mov cw, ax
		fldcw cw
		pop eax
	}
}
