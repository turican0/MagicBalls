# Turns the "[mb crash]   #NN module+0xOFFSET" lines of a --debug-log crash (Windows) into function
# names and source lines, with dbghelp and the .pdb next to the DLL.
#   python symbolize.py <debug.log or .err> <EXTENSION-NAME...dll>
import ctypes
import ctypes.wintypes as wt
import os
import re
import sys

log_path, dll_path = sys.argv[1], sys.argv[2]
dll_name = os.path.basename(dll_path).lower()

dbghelp = ctypes.WinDLL("dbghelp.dll")
process = ctypes.windll.kernel32.GetCurrentProcess()
dbghelp.SymSetOptions(0x00000002 | 0x00000010)  # SYMOPT_UNDNAME | SYMOPT_LOAD_LINES
dbghelp.SymInitializeW(process, ctypes.c_wchar_p(os.path.dirname(os.path.abspath(dll_path))), False)
BASE = 0x10000000
dbghelp.SymLoadModuleExW.restype = ctypes.c_uint64
dbghelp.SymLoadModuleExW(process, None, ctypes.c_wchar_p(os.path.abspath(dll_path)), None, ctypes.c_uint64(BASE), 0, None, 0)


class SYMBOL_INFO(ctypes.Structure):
    _fields_ = [("SizeOfStruct", wt.ULONG), ("TypeIndex", wt.ULONG), ("Reserved", ctypes.c_uint64 * 2),
                ("Index", wt.ULONG), ("Size", wt.ULONG), ("ModBase", ctypes.c_uint64), ("Flags", wt.ULONG),
                ("Value", ctypes.c_uint64), ("Address", ctypes.c_uint64), ("Register", wt.ULONG),
                ("Scope", wt.ULONG), ("Tag", wt.ULONG), ("NameLen", wt.ULONG), ("MaxNameLen", wt.ULONG),
                ("Name", ctypes.c_char * 1024)]


class IMAGEHLP_LINE64(ctypes.Structure):
    _fields_ = [("SizeOfStruct", wt.DWORD), ("Key", ctypes.c_void_p), ("LineNumber", wt.DWORD),
                ("FileName", ctypes.c_char_p), ("Address", ctypes.c_uint64)]


for line in open(log_path, encoding="utf-8", errors="replace"):
    m = re.search(r"#(\d+) (\S+)\+0x([0-9a-f]+)", line)
    if not m:
        if "CRASH" in line:
            print(line.rstrip())
        continue
    frame, module, offset = m.group(1), m.group(2), int(m.group(3), 16)
    if module.lower() != dll_name:
        print(f"#{frame} {module}+0x{offset:x}")
        continue
    symbol = SYMBOL_INFO(SizeOfStruct=ctypes.sizeof(SYMBOL_INFO) - 1024, MaxNameLen=1024)
    displacement = ctypes.c_uint64()
    name = "?"
    if dbghelp.SymFromAddr(process, ctypes.c_uint64(BASE + offset), ctypes.byref(displacement), ctypes.byref(symbol)):
        name = symbol.Name.decode() + f"+0x{displacement.value:x}"
    source = IMAGEHLP_LINE64(SizeOfStruct=ctypes.sizeof(IMAGEHLP_LINE64))
    line_displacement = wt.DWORD()
    where = ""
    if dbghelp.SymGetLineFromAddr64(process, ctypes.c_uint64(BASE + offset), ctypes.byref(line_displacement), ctypes.byref(source)):
        where = f"  {os.path.basename(source.FileName.decode())}:{source.LineNumber}"
    print(f"#{frame} {name}{where}")
