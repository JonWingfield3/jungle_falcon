from elftools.elf.elffile import ELFFile
import argparse
import struct

verbosity_level=1

def vlog(v_level, string):
  if v_level <= verbosity_level: print('[VLOG {}]: {}'.format(v_level, string))  

def swap_endianness(byte_arr):
  for ii in range(0, len(byte_arr), 4):
    swp_cpy = byte_arr[ii : ii + 4]
    swp_cpy.reverse()
    byte_arr[ii : ii + 4] = swp_cpy
  return byte_arr

def make_bin(elf_filename, section_names=['.start', '.text']):
  with open(elf_filename, 'rb') as f:
    elffile = ELFFile(f)
    vlog(3, 'HEADER:\n{}\n'.format(elffile.header))

    sections_data = {section_name : bytearray(\
        elffile.get_section_by_name(section_name).data()) for section_name in section_names}

    vlog(5, '.start section:\n{}\n\n'.format(sections_data['.start']))
    vlog(5, '.text section:\n{}\n\n'.format(sections_data['.text']))

    bin_file = sections_data['.start'] + sections_data['.text']

    vlog(4, 'combined sections:\n{}\n\n'.format(bin_file))
    return bin_file

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='RiscV linker built on top of pyelftools')
  parser.add_argument('elf_file', help='input elf file to analyze/create binary image for', type=str)
  parser.add_argument('--verbosity', '-v', type=int, help='set verbosity level [1-5]', default=1)

  args = parser.parse_args()

  verbosity_level = args.verbosity

  elf_filename = args.elf_file
  if '.out' not in elf_filename:
    print('Error! Unrecognized file type: {}'.format(elf_filename))

  bin_filename = elf_filename.replace('.out', '.bin')
  print("Creating {}...".format(bin_filename))
  
  bin_file = make_bin(elf_filename)  

  with open(bin_filename, 'wb') as f:
    f.write(bin_file)
  print("Wrote {} bytes to {}".format(len(bin_file), bin_filename))

