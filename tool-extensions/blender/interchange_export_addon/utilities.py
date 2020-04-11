import struct

class BitBool(object):
    TRUE = 0x1
    FALSE = 0x0
    
    @staticmethod
    def convert(b):
        if b:
            return BitBool.TRUE
        else:
            return BitBool.FALSE

def write_32_bit_unsigned(i, f):
    b = struct.pack('I', i)
    f.write(b)
    
    
def write_32_bit_signed(i, f):
    b = struct.pack('i', i)
    f.write(b)
    
    
def write_8_bit_unsigned(i ,f):
    b = struct.pack('B', i)
    f.write(b)
    
    
def write_8_bit_bool(b, f):
    write_8_bit_unsigned(BitBool.convert(b), f)
        

def write_32_bit_bool(b, f):
    write_32_bit_unsigned(BitBool.convert(b), f)
    

def write_32_bit_float(fl, f):
    b = struct.pack('f', fl)
    f.write(b)
    
    
def write_padding(bytes, f):
    PADDING = 0xFE
    
    for i in range(bytes):
        write_8_bit_unsigned(PADDING, f)
        

def write_string(s, f, length=256):
    output = struct.pack("{}s".format(length), s.encode('UTF-8'))
    f.write(output)
        
        
class Vector2(object):
    def __init__(self, x=0.0, y=0.0):
        self.x = x
        self.y = y
    
    @staticmethod
    def from_bvec(bvec):
        return Vector2(
            bvec.x,
            bvec.y
        )
        
    def write(self, f):
        write_32_bit_float(self.x, f)
        write_32_bit_float(self.y, f)


class Vector3(object):
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = x
        self.y = y
        self.z = z
    
    @staticmethod
    def from_bvec(bvec):
        return Vector3(
            bvec.x,
            bvec.y,
            bvec.z
        )
        
    def write(self, f):
        write_32_bit_float(self.x, f)
        write_32_bit_float(self.y, f)
        write_32_bit_float(self.z, f)
        
        
class Vector4(object):
    def __init__(self, x=0.0, y=0.0, z=0.0, w=1.0):
        self.x = x
        self.y = y
        self.z = z
        self.w = w
        
    @staticmethod
    def from_bvec(bvec):
        return Vector4(
            bvec.x,
            bvec.y,
            bvec.z,
            1.0
        )
        
    def write(self, f):
        write_32_bit_float(self.x, f)
        write_32_bit_float(self.y, f)
        write_32_bit_float(self.z, f)
        write_32_bit_float(self.w, f)
        
        
class Quaternion(object):
    def __init__(self, w=0.0, x=0.0, y=0.0, z=0.0):
        self.w = w
        self.x = x
        self.y = y
        self.z = z
        
    @staticmethod
    def from_bquat(bquat):
        return Quaternion(
            bquat.w,
            bquat.x,
            bquat.y,
            bquat.z
        )
        
    def write(self, f):
        write_32_bit_float(self.w, f) 
        write_32_bit_float(self.x, f)
        write_32_bit_float(self.y, f)
        write_32_bit_float(self.z, f) 
