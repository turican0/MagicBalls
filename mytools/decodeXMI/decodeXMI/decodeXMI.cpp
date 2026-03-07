// xmidump.cpp — XMI event dumper
// File/chunk loading: based on xmi_dump.cpp reference code
// EVNT parsing:      own implementation (Sound.cpp AIL engine logic)
//
// Build (MSVC): cl xmidump.cpp /Fe:xmidump.exe
// Build (GCC):  g++ -std=c++17 -O2 -o xmidump xmidump.cpp
// Usage: xmidump <file.xmi> [track] [--xmi-only]
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <vector>

// ============================================================================
// Helpers
// ============================================================================

static uint32_t read_be32(const uint8_t* p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
        | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}
static uint16_t read_be16(const uint8_t* p) {
    return (uint16_t)((p[0] << 8) | p[1]);
}

// Standard MIDI VLQ (high-bit continuation, value concatenated)
static uint32_t read_vlq(const uint8_t*& p, const uint8_t* end) {
    uint32_t val = 0;
    while (p < end) {
        uint8_t b = *p++;
        val = (val << 7) | (b & 0x7F);
        if (!(b & 0x80)) break;
    }
    return val;
}

// XMI delta-time: sequence of bytes with high-bit=0, values are SUMMED (not concatenated)
// High-bit=1 marks start of next event status byte
static uint32_t read_xmi_delta(const uint8_t*& p, const uint8_t* end) {
    uint32_t delta = 0;
    while (p < end && !(*p & 0x80)) {
        delta += *p++;
    }
    return delta;
}

static const char* note_name(uint8_t note) {
    static const char* names[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
    static char buf[8];
    snprintf(buf, sizeof(buf), "%s%d", names[note % 12], (note / 12) - 1);
    return buf;
}

// ============================================================================
// RBRN / TIMB structures (from reference code)
// ============================================================================

struct RbrnMarker { uint16_t id; uint32_t offset; };

struct RbrnChunk {
    int count;
    RbrnMarker markers[64];
};

struct TimbChunk {
    int count;
    struct { int patch, bank; } timbres[128];
};

static void parse_RBRN(const uint8_t* data, uint32_t size, RbrnChunk* out) {
    out->count = 0;
    if (size < 2) return;
    uint16_t count = read_be16(data);
    out->count = count;
    for (int i = 0; i < count && i < 64; i++) {
        const uint8_t* e = data + 2 + i * 6;
        out->markers[i].id = read_be16(e);
        out->markers[i].offset = read_be32(e + 2);
    }
}

static void parse_TIMB(const uint8_t* data, uint32_t size, TimbChunk* out) {
    out->count = 0;
    if (size < 2) return;
    uint16_t count = read_be16(data);
    if (2u + (uint32_t)count * 2u > size) count = (uint16_t)((size - 2) / 2);
    out->count = count;
    for (int i = 0; i < count && i < 128; i++) {
        out->timbres[i].patch = data[2 + i * 2];
        out->timbres[i].bank = data[3 + i * 2];
    }
}

// ============================================================================
// XMI controller names (from sub_A5850 in Sound.cpp)
// ============================================================================

static const char* cc_name(uint8_t cc) {
    switch (cc) {
    case   0: return "Bank Select MSB";
    case   1: return "Modulation";
    case   2: return "Breath Controller";
    case   4: return "Foot Controller";
    case   5: return "Portamento Time";
    case   6: return "Data Entry MSB";
    case   7: return "Volume";
    case   8: return "Balance";
    case  10: return "Pan";
    case  11: return "Expression";
    case  12: return "Effect Control 1";
    case  13: return "Effect Control 2";
    case  32: return "Bank Select LSB";
    case  38: return "Data Entry LSB";
    case  64: return "Sustain Pedal";
    case  65: return "Portamento On/Off";
    case  66: return "Sostenuto";
    case  67: return "Soft Pedal";
    case  71: return "Resonance";
    case  72: return "Release Time";
    case  73: return "Attack Time";
    case  74: return "Brightness";
    case  91: return "Reverb Level";
    case  93: return "Chorus Level";
    case 100: return "RPN LSB";
    case 101: return "RPN MSB";
    case 121: return "Reset All Controllers";
    case 123: return "All Notes Off";
        // XMI-specific (sub_A5850)
    case 0x6C: return "XMI Prefix Callback";
    case 0x6D: return "XMI Branch Index (RBRN)";
    case 0x6E: return "XMI Channel Lock";
    case 0x6F: return "XMI Channel Protect";
    case 0x73: return "XMI Patch Bank Select";
    case 0x74: return "XMI FOR Loop Start";
    case 0x75: return "XMI FOR Loop Next";
    case 0x76: return "XMI Beat/Measure Reset";
    case 0x77: return "XMI Trigger Callback";
    default:   return nullptr;
    }
}

static void print_xmi_cc_note(uint8_t cc, uint8_t val, int* loop_depth) {
    switch (cc) {
    case 0x74: // FOR Loop Start — saves EVNT_ptr + repeat count (FOR_loop_count_33)
        (*loop_depth)++;
        if (val == 0) printf("  -> infinite loop (depth %d)", *loop_depth);
        else          printf("  -> repeat %d times (depth %d)", val, *loop_depth);
        break;
    case 0x75: // FOR Loop Next — jumps EVNT_ptr back or exits
        if (val >= 64) printf("  -> conditional jump back (depth %d)", *loop_depth);
        else           printf("  -> unconditional, ignored");
        if (*loop_depth > 0) (*loop_depth)--;
        break;
    case 0x6D: // Branch — searches RBRN table, resets FOR_loop_count_33[]
        printf("  -> jump to RBRN marker ID=%d", val);
        break;
    case 0x6E:
        printf("  -> %s", val >= 64 ? "lock channel" : "unlock channel");
        break;
    case 0x6F:
        printf("  -> protect level %s", val >= 64 ? "2 (protected)" : "0 (free)");
        break;
    case 0x76:
        printf("  -> reset beat/measure counters + beat_callback");
        break;
    case 0x77: // Trigger — calls trigger_callback_8_32 (registered as sub_8E0D0)
        printf("  -> calls trigger_callback (sub_8E0D0), val=%d", val);
        break;
    default: break;
    }
}

// ============================================================================
// EVNT chunk parser
// ============================================================================

static void analyze_EVNT(const uint8_t* evnt, uint32_t evnt_size,
    const RbrnChunk* rbrn, bool xmi_only)
{
    const uint8_t* p = evnt;
    const uint8_t* end = evnt + evnt_size;

    uint32_t total_ticks = 0;
    uint32_t tempo = 500000;   // default 120 BPM
    uint32_t ev_num = 0;
    int      loop_depth = 0;

    if (xmi_only)
        printf("    (* = XMI-specific controller)\n\n");

    printf("    %-6s  %-8s  %-6s  %-20s  %s\n",
        "Evt#", "Ticks", "Offset", "Type", "Details");
    printf("    %-6s  %-8s  %-6s  %-20s  %s\n",
        "------", "--------", "------", "--------------------",
        "-------------------------------------------");

    while (p < end) {
        uint32_t ev_offset = (uint32_t)(p - evnt);

        // Check RBRN markers at this offset
        for (int i = 0; i < rbrn->count; i++) {
            if (rbrn->markers[i].offset == ev_offset) {
                printf("    %6s  %8u  0x%04X  %-20s  ID=%d\n",
                    ">>>", total_ticks, ev_offset,
                    ">>> BRANCH POINT <<<", rbrn->markers[i].id);
            }
        }

        // XMI delta time: sum of bytes with high-bit=0
        uint32_t delta = read_xmi_delta(p, end);
        total_ticks += delta;

        if (p >= end) break;

        ev_offset = (uint32_t)(p - evnt);
        uint8_t status = *p++;
        uint8_t type = status & 0xF0;
        uint8_t ch = status & 0x0F;

        // ---- Meta ----
        if (status == 0xFF) {
            if (p >= end) break;
            uint8_t mtype = *p++;
            uint32_t mlen = read_vlq(p, end);   // standard VLQ
            const uint8_t* mdata = p;
            p += mlen;

            if (!xmi_only) {
                if (mtype == 0x51 && mlen >= 3) {
                    tempo = ((uint32_t)mdata[0] << 16) | ((uint32_t)mdata[1] << 8) | mdata[2];
                    printf("    %-6u  %-8u  0x%04X  %-20s  %u us/beat = %.1f BPM\n",
                        ev_num, total_ticks, ev_offset, "Tempo",
                        tempo, 60000000.0 / tempo);
                }
                else if (mtype == 0x58 && mlen >= 4) {
                    printf("    %-6u  %-8u  0x%04X  %-20s  %d/%d clocks=%d 32nds=%d\n",
                        ev_num, total_ticks, ev_offset, "Time Signature",
                        mdata[0], 1 << mdata[1], mdata[2], mdata[3]);
                }
                else if (mtype == 0x2F) {
                    printf("    %-6u  %-8u  0x%04X  %-20s\n",
                        ev_num, total_ticks, ev_offset, "End of Track");
                    ev_num++;
                    break;
                }
                else if (mtype >= 0x01 && mtype <= 0x07 && mlen > 0) {
                    printf("    %-6u  %-8u  0x%04X  %-20s  \"",
                        ev_num, total_ticks, ev_offset,
                        mtype == 0x03 ? "Track Name" :
                        mtype == 0x06 ? "Marker" : "Text");
                    for (uint32_t i = 0; i < mlen && i < 64; i++)
                        putchar(mdata[i] >= 32 ? mdata[i] : '?');
                    printf("\"\n");
                }
                else {
                    printf("    %-6u  %-8u  0x%04X  %-20s  type=0x%02X len=%u\n",
                        ev_num, total_ticks, ev_offset, "Meta", mtype, mlen);
                }
            }
            ev_num++;
            continue;
        }

        // ---- SysEx ----
        if (status == 0xF0 || status == 0xF7) {
            uint32_t slen = read_vlq(p, end);
            if (!xmi_only)
                printf("    %-6u  %-8u  0x%04X  %-20s  len=%u\n",
                    ev_num, total_ticks, ev_offset, "SysEx", slen);
            p += slen;
            ev_num++;
            continue;
        }

        // ---- Note Off (0x80) ----
        if (type == 0x80) {
            if (p + 1 > end) break;
            uint8_t note = *p++;
            uint8_t vel = *p++;
            if (!xmi_only)
                printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  note=%-4s vel=%d\n",
                    ev_num, total_ticks, ev_offset, "Note Off",
                    ch, note_name(note), vel);
            ev_num++;
            continue;
        }

        // ---- Note On (0x90) — XMI adds duration VLQ after velocity ----
        if (type == 0x90) {
            if (p + 1 > end) break;
            uint8_t note = *p++;
            uint8_t vel = *p++;
            // XMI-specific: duration follows as standard VLQ
            uint32_t dur = read_vlq(p, end);
            if (!xmi_only) {
                if (vel == 0)
                    printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  note=%-4s (vel=0 -> off)\n",
                        ev_num, total_ticks, ev_offset, "Note On",
                        ch, note_name(note));
                else
                    printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  note=%-4s vel=%d dur=%u\n",
                        ev_num, total_ticks, ev_offset, "Note On",
                        ch, note_name(note), vel, dur);
            }
            ev_num++;
            continue;
        }

        // ---- Poly Pressure (0xA0) ----
        if (type == 0xA0) {
            if (p + 1 > end) break;
            uint8_t note = *p++;
            uint8_t val = *p++;
            if (!xmi_only)
                printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  note=%-4s val=%d\n",
                    ev_num, total_ticks, ev_offset, "Poly Pressure",
                    ch, note_name(note), val);
            ev_num++;
            continue;
        }

        // ---- Control Change (0xB0) ----
        if (type == 0xB0) {
            if (p + 1 > end) break;
            uint8_t cc = *p++;
            uint8_t val = *p++;

            bool is_xmi = (cc >= 0x6C && cc <= 0x77);

            if (!xmi_only || is_xmi) {
                const char* name = cc_name(cc);
                char ccbuf[24];
                if (!name) { snprintf(ccbuf, sizeof(ccbuf), "CC#%d", cc); name = ccbuf; }

                printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  cc=%3d val=%3d  %s",
                    ev_num, total_ticks, ev_offset,
                    is_xmi ? "Control Change *" : "Control Change",
                    ch, cc, val, name);

                if (is_xmi) print_xmi_cc_note(cc, val, &loop_depth);
                printf("\n");
            }
            ev_num++;
            continue;
        }

        // ---- Program Change (0xC0) ----
        if (type == 0xC0) {
            if (p > end) break;
            uint8_t prog = *p++;
            if (!xmi_only)
                printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  program=%d\n",
                    ev_num, total_ticks, ev_offset, "Program Change", ch, prog);
            ev_num++;
            continue;
        }

        // ---- Channel Pressure (0xD0) ----
        if (type == 0xD0) {
            if (p > end) break;
            uint8_t pres = *p++;
            if (!xmi_only)
                printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  pressure=%d\n",
                    ev_num, total_ticks, ev_offset, "Channel Pressure", ch, pres);
            ev_num++;
            continue;
        }

        // ---- Pitch Bend (0xE0) ----
        if (type == 0xE0) {
            if (p + 1 > end) break;
            uint8_t lo = *p++;
            uint8_t hi = *p++;
            int bend = ((hi << 7) | lo) - 8192;
            if (!xmi_only)
                printf("    %-6u  %-8u  0x%04X  %-20s  ch%d  val=%d\n",
                    ev_num, total_ticks, ev_offset, "Pitch Bend", ch, bend);
            ev_num++;
            continue;
        }

        printf("    [unknown status 0x%02X at EVNT+0x%04X, stopping]\n",
            status, ev_offset);
        break;
    }

    double beat_len = tempo / 1000000.0;
    double total_sec = (double)total_ticks / 120.0 * beat_len;
    printf("\n    Total events : %u\n", ev_num);
    printf("    Total ticks  : %u\n", total_ticks);
    printf("    Approx time  : %.2f s  (%.2f min)  at final tempo %.1f BPM\n",
        total_sec, total_sec / 60.0, 60000000.0 / tempo);
}

// ============================================================================
// Sequence parser — given the interior of a FORM XMID block
// (same layout as parse_xmi_sequence in reference code)
// ============================================================================

static void parse_sequence(const uint8_t* data, uint32_t size,
    int seq_idx, bool xmi_only)
{
    printf("\n+==========================================+\n");
    printf("|  Sequence #%d\n", seq_idx);
    printf("+==========================================+\n");

    const uint8_t* p = data;
    const uint8_t* end = data + size;

    const uint8_t* timb_data = nullptr; uint32_t timb_size = 0;
    const uint8_t* rbrn_data = nullptr; uint32_t rbrn_size = 0;
    const uint8_t* evnt_data = nullptr; uint32_t evnt_size = 0;

    while (p + 8 <= end) {
        char tag[5]; memcpy(tag, p, 4); tag[4] = 0;
        uint32_t csz = read_be32(p + 4);
        const uint8_t* cd = p + 8;
        p += 8 + csz + (csz & 1);

        if (!strcmp(tag, "TIMB")) { timb_data = cd; timb_size = csz; }
        else if (!strcmp(tag, "RBRN")) { rbrn_data = cd; rbrn_size = csz; }
        else if (!strcmp(tag, "EVNT")) { evnt_data = cd; evnt_size = csz; }
    }

    // ── TIMB ──
    if (timb_data) {
        TimbChunk timb;
        parse_TIMB(timb_data, timb_size, &timb);
        printf("\n  [TIMB] %d instrument(s):\n", timb.count);
        for (int i = 0; i < timb.count; i++)
            printf("    Patch %3d  Bank %d\n", timb.timbres[i].patch, timb.timbres[i].bank);
    }
    else {
        printf("\n  [TIMB] not present\n");
    }

    // ── RBRN ──
    RbrnChunk rbrn; rbrn.count = 0;
    if (rbrn_data) {
        parse_RBRN(rbrn_data, rbrn_size, &rbrn);
        printf("\n  [RBRN] %d branch marker(s):\n", rbrn.count);
        for (int i = 0; i < rbrn.count; i++)
            printf("    Marker ID=%-3d  EVNT offset=0x%04X (%u)\n",
                rbrn.markers[i].id,
                rbrn.markers[i].offset,
                rbrn.markers[i].offset);
    }
    else {
        printf("\n  [RBRN] not present\n");
    }

    // ── EVNT ──
    if (evnt_data) {
        printf("\n  [EVNT] %u bytes\n\n", evnt_size);
        analyze_EVNT(evnt_data, evnt_size, &rbrn, xmi_only);
    }
    else {
        printf("\n  [EVNT] not present!\n");
    }
}

// ============================================================================
// Top-level XMI file router
// (detection logic from reference code: xmi_dump.cpp)
// ============================================================================

static void parse_xmi_file(const uint8_t* data, size_t size,
    int filter_track, bool xmi_only)
{
    if (size < 12) { printf("Error: file too small.\n"); return; }
    if (memcmp(data, "FORM", 4) != 0) { printf("Error: FORM header missing.\n"); return; }

    uint32_t form_size = read_be32(data + 4);

    // ── FORM XMID — single sequence ──
    if (memcmp(data + 8, "XMID", 4) == 0) {
        printf("Type : FORM XMID  (1 sequence)\n");
        if (filter_track <= 1)
            parse_sequence(data + 12, form_size - 4, 0, xmi_only);
        printf("\nTotal sequences: 1\n");
        return;
    }

    // ── FORM XDIR — directory wrapper → CAT XMID follows after FORM XDIR ──
    if (memcmp(data + 8, "XDIR", 4) == 0) {
        printf("Type : FORM XDIR  (directory container)\n");

        // Walk chunks inside XDIR to find INFO
        const uint8_t* xp = data + 12;
        const uint8_t* xend = data + 8 + form_size;
        while (xp + 8 <= xend) {
            uint32_t csz = read_be32(xp + 4);
            if (memcmp(xp, "INFO", 4) == 0 && csz >= 2) {
                uint16_t cnt_le = (uint16_t)(xp[8] | (xp[9] << 8));
                printf("  INFO: %u sequence(s) declared\n", cnt_le);
            }
            if (csz == 0) break;
            xp += 8 + csz + (csz & 1);
        }

        // CAT XMID immediately follows FORM XDIR in the file
        const uint8_t* cat = data + 8 + form_size + (form_size & 1);
        printf("  Looking for CAT at offset 0x%X...\n", (unsigned)(cat - data));

        if (cat + 12 > data + size || memcmp(cat, "CAT ", 4) != 0) {
            printf("  CAT not found at expected offset. Bytes: %02X %02X %02X %02X\n",
                cat[0], cat[1], cat[2], cat[3]);
            return;
        }

        uint32_t cat_size = read_be32(cat + 4);
        printf("  Found CAT '%.4s'  size=%u\n\n", cat + 8, cat_size);

        const uint8_t* cp = cat + 12;
        const uint8_t* cend = cat + 8 + cat_size;
        int seq_idx = 0;
        while (cp + 8 <= cend) {
            if (memcmp(cp, "FORM", 4) != 0) { cp++; continue; }
            uint32_t seq_size = read_be32(cp + 4);
            if (memcmp(cp + 8, "XMID", 4) == 0) {
                seq_idx++;
                if (filter_track == 0 || filter_track == seq_idx)
                    parse_sequence(cp + 12, seq_size - 4, seq_idx, xmi_only);
            }
            cp += 8 + seq_size + (seq_size & 1);
        }
        printf("\nTotal sequences: %d\n", seq_idx);
        return;
    }

    // ── FORM CAT — direct multi-track (some tools produce this) ──
    if (memcmp(data + 8, "CAT ", 4) == 0) {
        printf("Type : FORM CAT XMID  (multiple sequences)\n");
        if (memcmp(data + 12, "XMID", 4) != 0) { printf("Error: CAT is not XMID.\n"); return; }

        const uint8_t* cp = data + 16;
        const uint8_t* cend = data + 8 + form_size;
        int seq_idx = 0;
        while (cp + 8 <= cend) {
            if (memcmp(cp, "FORM", 4) != 0) { cp++; continue; }
            uint32_t seq_size = read_be32(cp + 4);
            if (memcmp(cp + 8, "XMID", 4) == 0) {
                seq_idx++;
                if (filter_track == 0 || filter_track == seq_idx)
                    parse_sequence(cp + 12, seq_size - 4, seq_idx, xmi_only);
            }
            cp += 8 + seq_size + (seq_size & 1);
        }
        printf("\nTotal sequences: %d\n", seq_idx);
        return;
    }

    printf("Error: Unknown FORM type '%.4s'\n", data + 8);
}

// ============================================================================
// main
// ============================================================================

int main(int argc, char* argv[]) {
    printf("===========================================\n");
    printf("  xmidump  --  XMI event dumper\n");
    printf("  Based on AIL XMIDI engine (Sound.cpp)\n");
    printf("===========================================\n\n");

    if (argc < 2) {
        printf("Usage:   xmidump <file.xmi> [track] [--xmi-only]\n\n");
        printf("  track       1..N = single sequence, 0 or omit = all\n");
        printf("  --xmi-only  show only XMI-specific controllers\n\n");
        printf("XMI controllers (from sub_A5850 in Sound.cpp):\n");
        printf("  0x6C (108)  Prefix Callback\n");
        printf("  0x6D (109)  Branch Index  -- jump to RBRN marker\n");
        printf("  0x6E (110)  Channel Lock\n");
        printf("  0x6F (111)  Channel Protect\n");
        printf("  0x73 (115)  Patch Bank Select\n");
        printf("  0x74 (116)  FOR Loop Start -- saves EVNT_ptr + repeat count\n");
        printf("  0x75 (117)  FOR Loop Next  -- jumps back or ends loop\n");
        printf("  0x76 (118)  Beat/Measure Reset\n");
        printf("  0x77 (119)  Trigger Callback (fires sub_8E0D0)\n");
        return 1;
    }

    const char* filename = argv[1];
    int  filter_track = 0;
    bool xmi_only = false;

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--xmi-only")) xmi_only = true;
        else filter_track = atoi(argv[i]);
    }

    FILE* f = fopen(filename, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open '%s'\n", filename); return 1; }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    std::vector<uint8_t> buf((size_t)fsize);
    if ((long)fread(buf.data(), 1, (size_t)fsize, f) != fsize) {
        fprintf(stderr, "Error: read failed\n"); fclose(f); return 1;
    }
    fclose(f);

    printf("File  : %s\n", filename);
    printf("Size  : %ld bytes\n", fsize);
    printf("Header: %.4s  type=%.4s\n\n", buf.data(), buf.data() + 8);

    parse_xmi_file(buf.data(), (size_t)fsize, filter_track, xmi_only);

    printf("\n===========================================\n");
    printf("  Done.\n");
    printf("===========================================\n");
    return 0;
}