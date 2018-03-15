OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv",
	      "elf32-littleriscv")

OUTPUT_ARCH(riscv)

ENTRY(_start)

SECTIONS
{
  . = 0x0;
  .start      : 
  { 
    *(.start) 
  }
  . = 0xc;
  .text       : 
  { 
    *(.text) 
  }

  /DISCARD/ :
  {
    *(.comments)
  }

}
