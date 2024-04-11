# 第三次课后练习

刘蕴皓 2021012884

在uCore中处于内核态时，用read_ebp()获得当前栈底指针信息，偏移两个单位得到最后入栈的参数地址args，然后通过函数调用栈的结构反复前读以获得函数调用栈各个frame的信息

```
void print_stackframe(void)
{
	uint32_t ebp = read_ebp();
	uint32_t eip = read_eip();
	int i,j;
	for(i = 0; i < STACKFRAME_DEPTH && ebp != 0; i++)
	{
		cprintf("ebp = 0x%08x, eip = 0x%08x ", ebp, eip);
		cprintf("Args = ");
		uint32_t *args = (uint32_t *)ebp + 2;
		for(j = 0; j < 4; j++)
			cprintf("0x%08x ", args[j]);
		cprintf("\n");
		print_debuginfo(eip-1);
		eip = ((uint32_t *)ebp + 1)[0];
		ebp = ((uint32_t *)ebp)[0];
	}
}
```

处于用户态时，还需要从用户态切换到内核态，打印出内核独立堆栈中的调用信息