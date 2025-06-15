import ctypes

class kv(ctypes.Structure):
    _fields_ = [
        ("header", ctypes.c_char_p),
        ("numEntries", ctypes.c_size_t),
        ("entries", ctypes.POINTER(ctypes.c_char_p))
    ]

class CSV(ctypes.Structure):
    _fields_ = [
        ("data", ctypes.POINTER(kv)),
        ("numEntries", ctypes.c_size_t)
    ]

lib = ctypes.CDLL("./main.dll")

lib.parse_csv.restype = CSV
lib.parse_csv.argtypes = [ctypes.c_char_p, ctypes.c_long]

def parse_csv(content: bytes):
    return lib.parse_csv(content, len(content))

table = parse_csv(open("customers-100.csv", "rb").read())

for i in range(table.numEntries):
    print(table.data[i].header.decode("utf-8", errors="replace"))

    for j in range(table.data[i].numEntries):
        print(table.data[i].entries[j].decode("utf-8", errors="replace"))