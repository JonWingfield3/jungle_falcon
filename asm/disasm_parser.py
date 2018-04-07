import argparse
"""
  Messy python script which parses a disasm file
  (one created with objdump -D) and corrects for weird
  offsets added by riscv gcc toolchain (probably linker).
  The corrected results are printed to stdout. Thus, for
  longer asm files piping to less or redirecting to a file
  is usually preferable.
"""

if __name__ == '__main__':
  parser = argparse.ArgumentParser("Disassembly parser")
  parser.add_argument("disasm_file", type=str)
  args = parser.parse_args()

  with open(args.disasm_file) as disasm_file:
     for i in range(6):
       print(disasm_file.readline(), end='')

     offset_line = disasm_file.readline()
     print(offset_line, end='')
     overall_offset = int(offset_line.split(' ')[0], 16)
     next_line = disasm_file.readline()
     while next_line != "":
       candidate_num = next_line.split(":")[0]
       if candidate_num != "\n":
         try:
           actual_num = int(candidate_num, 16) - overall_offset
           new_line = "\t{}".format(hex(actual_num)[2:]) + next_line[next_line.find(":"):]
           print(new_line, end='')
         except ValueError:
           print(next_line, end='')
       else: print("")
       next_line = disasm_file.readline()
  print() 
