MEMORY
{
	RAM(rw)    : ORIGIN = 0x00000000, LENGTH = 1K
	ROM(rx)	   : ORIGIN = 0x00000400, LENGTH = 4K
}

SECTIONS
{
  .startup :
  {
    *.(.text)
  } > ROM

  .data : 
  {
    *.(.data*)
  } > RAM

}
