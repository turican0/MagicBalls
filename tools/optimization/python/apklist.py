# Lists the files of a zip (APK) on the web without downloading it: reads only the end of central
# directory and the central directory through HTTP range requests (curl, as the company network needs).
#   python apklist.py <url> <out.tsv>
import struct
import subprocess
import sys

url, out = sys.argv[1], sys.argv[2]


def fetch(start, end):
    r = subprocess.run(["curl", "-s", "-L", "--ssl-no-revoke", "-r", f"{start}-{end}", url],
                       capture_output=True, check=True)
    return r.stdout


def size():
    r = subprocess.run(["curl", "-s", "-L", "-I", "--ssl-no-revoke", url], capture_output=True, text=True)
    lengths = [l.split(":")[1].strip() for l in r.stdout.splitlines() if l.lower().startswith("content-length")]
    return int(lengths[-1])


total = size()
tail = fetch(total - 65536, total - 1)
base = total - len(tail)
eocd = tail.rfind(b"PK\x05\x06")
entries, cd_size, cd_offset = struct.unpack("<HII", tail[eocd + 10:eocd + 20])
# zip64: the real values are in the zip64 end record
if cd_offset == 0xFFFFFFFF or entries == 0xFFFF:
    loc = tail.rfind(b"PK\x06\x07")
    z64 = struct.unpack("<Q", tail[loc + 8:loc + 16])[0]
    rec = fetch(z64, z64 + 55)
    entries, cd_size, cd_offset = struct.unpack("<QQQ", rec[32:56])
cd = fetch(cd_offset, cd_offset + cd_size - 1)
rows = []
p = 0
while p < len(cd) and cd[p:p + 4] == b"PK\x01\x02":
    csize, usize = struct.unpack("<II", cd[p + 20:p + 28])
    nlen, xlen, clen = struct.unpack("<HHH", cd[p + 28:p + 34])
    name = cd[p + 46:p + 46 + nlen].decode("utf-8", "replace")
    extra = cd[p + 46 + nlen:p + 46 + nlen + xlen]
    if usize == 0xFFFFFFFF or csize == 0xFFFFFFFF:  # zip64 sizes in the extra field
        q = 0
        while q < len(extra):
            tag, ln = struct.unpack("<HH", extra[q:q + 4])
            if tag == 1:
                vals = struct.unpack("<" + "Q" * (ln // 8), extra[q + 4:q + 4 + ln])
                i = 0
                if usize == 0xFFFFFFFF:
                    usize = vals[i]; i += 1
                if csize == 0xFFFFFFFF:
                    csize = vals[i]
            q += 4 + ln
    rows.append((name, usize, csize))
    p += 46 + nlen + xlen + clen
with open(out, "w", encoding="utf-8") as f:
    for r in rows:
        f.write(f"{r[0]}\t{r[1]}\t{r[2]}\n")
print(f"{total / 1e6:.1f} MB, {len(rows)} entries (directory said {entries})")
