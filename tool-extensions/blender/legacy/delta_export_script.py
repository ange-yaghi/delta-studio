# Magic Numbers and Constants
FILE_VERSION = 0x0
MAGIC_NUMBER = 0x50F1AA
EDITOR_ID = 0x3
    
def write_32_bit_unsigned(i, f):
    b = i.to_bytes(4, byteorder='little', signed=False)
    f.write(b)
    
def write_header(f):
    write_32_bit_unsigned(MAGIC_NUMBER, f)
    write_32_bit_unsigned(FILE_VERSION, f)
    write_32_bit_unsigned(EDITOR_ID, f)
