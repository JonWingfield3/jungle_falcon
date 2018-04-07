from elftools.elf.elffile import ELFFile
import argparse

def swap_endianness(byte_arr):
  for ii in range(0, len(byte_arr), 4):
    swp_cpy = byte_arr[ii : ii + 4]
    swp_cpy.reverse()
    byte_arr[ii : ii + 4] = swp_cpy
  return byte_arr

def get_bin_image(elf_filename, section_names=['.start', '.text']):
  with open(elf_filename, 'rb') as f:
    elffile = ELFFile(f)

    sections_data = {section_name : bytearray(\
        elffile.get_section_by_name(section_name).data()) for section_name in section_names}

    bin_file = sections_data['.start'] + sections_data['.text']
    return bin_file

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='RISC-V linker tool built using pyelftools')
  parser.add_argument('elf_file', help='input elf file', type=str)
  args = parser.parse_args()

  elf_filename = args.elf_file
  if '.out' not in elf_filename:
    print('Error! Unrecognized file type: {}'.format(elf_filename))
    quit()

  bin_filename = elf_filename.replace('.out', '.bin')
  print("Creating {}...".format(bin_filename))

  image = get_bin_image(elf_filename)

  with open(bin_filename, 'wb') as bin_file:
    bin_file.write(image)
  print("Done. Wrote {} bytes to {}".format(len(image), bin_filename))

