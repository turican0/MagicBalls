// xmi_dump.cpp - XMI chunk analyzer
// Compile: cl xmi_dump.cpp /Fe:xmi_dump.exe
// Usage:   xmi_dump.exe file.xmi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ─────────────────────────────────────────────
// Helper functions
// ─────────────────────────────────────────────

static uint32_t read_be32(const uint8_t* p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
        ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}
static uint16_t read_be16(const uint8_t* p) {
    return (uint16_t)((p[0] << 8) | p[1]);
}

// Read XMI variable-length quantity (same format as MIDI VLQ)
static uint32_t read_vlq(const uint8_t* p, int* bytes_read) {
    uint32_t val = 0;
    int i = 0;
    do {
        val = (val << 7) | (p[i] & 0x7F);
        i++;
    } while ((p[i - 1] & 0x80) && i < 4);
    *bytes_read = i;
    return val;
}

// ─────────────────────────────────────────────
// Structures
// ─────────────────────────────────────────────

typedef struct {
    uint16_t id;
    uint32_t offset;   // offset into EVNT data
} RbrnMarker;

typedef struct {
    int        count;
    RbrnMarker markers[64];
} RbrnChunk;

typedef struct {
    int      patch;
    int      bank;
} Timbre;

typedef struct {
    int     count;
    Timbre  timbres[128];
} TimbChunk;

// ─────────────────────────────────────────────
// Chunk parsers
// ─────────────────────────────────────────────

static void parse_RBRN(const uint8_t* data, uint32_t size, RbrnChunk* out) {
    out->count = 0;
    if (size < 2) return;
    uint16_t count = read_be16(data);
    out->count = count;
    for (int i = 0; i < count && i < 64; i++) {
        const uint8_t* entry = data + 2 + i * 6;
        out->markers[i].id = read_be16(entry);
        out->markers[i].offset = read_be32(entry + 2);
    }
}

static void parse_TIMB(const uint8_t* data, uint32_t size, TimbChunk* out) {
    out->count = 0;
    if (size < 2) return;

    uint16_t count = read_be16(data);

    // Ověř že data jsou dostatečně velká
    uint32_t expected_size = 2 + (uint32_t)count * 2;
    if (expected_size > size) {
        printf("  [TIMB] WARNING: count=%u but size=%u, truncating\n", count, size);
        count = (uint16_t)((size - 2) / 2);
    }

    out->count = count;
    for (int i = 0; i < count && i < 128; i++) {
        out->timbres[i].patch = (int)(uint8_t)data[2 + i * 2];
        out->timbres[i].bank = (int)(uint8_t)data[3 + i * 2];
    }
}

// Walk EVNT data, count ticks and estimate time.
// Returns total tick count and tempo (default 500000 µs/beat = 120 BPM)
// Walk EVNT data, count ticks and estimate time.
static void analyze_EVNT(const uint8_t* evnt_data, uint32_t evnt_size,
    const RbrnChunk* rbrn,
    uint32_t* out_total_ticks,
    double* out_total_seconds)
{
    uint32_t tempo = 500000;
    uint32_t total_ticks = 0;
    double total_seconds = 0.0;
    const uint8_t* p = evnt_data;
    const uint8_t* end = evnt_data + evnt_size;

    printf("\n    [EVNT Event Dump]\n");
    printf("    Ticks    | Offset  | Event Details\n");
    printf("    ---------|---------|----------------------------------------\n");

    while (p < end) {
        uint32_t offset = (uint32_t)(p - evnt_data);

        // Kontrola RBRN markerů
        for (int i = 0; i < rbrn->count; i++) {
            if (rbrn->markers[i].offset == offset) {
                printf("    %8u | 0x%05X | >>> BRANCH POINT ID %d <<<\n",
                    total_ticks, offset, rbrn->markers[i].id);
            }
        }

        // 1. XMI delay — sumování 7-bit hodnot (NE concatenace jako MIDI VLQ!)
        // Delay bajty mají vysoký bit = 0
        // Event bajty mají vysoký bit = 1
        if (p < end && (*p & 0x80) == 0) {
            uint32_t delta = 0;
            while (p < end && (*p & 0x80) == 0) {
                delta += *p;
                if (*p != 0x7F) { p++; break; }
                p++;
            }
            total_ticks += delta;
            double beat_len = tempo / 1000000.0;
            total_seconds += (double)delta / 120.0 * beat_len;
        }

        if (p >= end) break;

        uint32_t event_offset = (uint32_t)(p - evnt_data);
        uint8_t status = *p++;
        uint8_t type = status & 0xF0;
        uint8_t chan = status & 0x0F;

        if (status == 0xFF) {
            // META event
            uint8_t m_type = *p++;
            // Meta délka je standardní MIDI VLQ (concatenace)
            uint32_t m_len = 0;
            while (p < end) {
                uint8_t b = *p++;
                m_len = (m_len << 7) | (b & 0x7F);
                if (!(b & 0x80)) break;
            }
            if (m_type == 0x51 && m_len >= 3) {
                tempo = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
                printf("    %8u | 0x%05X | [TEMPO] %u us/beat = %.1f BPM\n",
                    total_ticks, event_offset, tempo, 60000000.0 / tempo);
            }
            else if (m_type == 0x2F) {
                printf("    %8u | 0x%05X | [END OF TRACK]\n",
                    total_ticks, event_offset);
            }
            else if (m_type == 0x03) {
                printf("    %8u | 0x%05X | [TRACK NAME] \"%.*s\"\n",
                    total_ticks, event_offset, (int)m_len, p);
            }
            else if (m_type == 0x06 || m_type == 0x07) {
                printf("    %8u | 0x%05X | [MARKER] \"%.*s\"\n",
                    total_ticks, event_offset, (int)m_len, p);
            }
            p += m_len;
        }
        else if (type == 0x90) {
            // NOTE ON — XMI má navíc duration jako VLQ za velocity
            uint8_t note = *p++;
            uint8_t vel = *p++;
            // Duration: standardní MIDI VLQ (concatenace)
            uint32_t dur = 0;
            while (p < end) {
                uint8_t b = *p++;
                dur = (dur << 7) | (b & 0x7F);
                if (!(b & 0x80)) break;
            }
            // Tichý Note On (vel=0) loguj jen pokud chceš všechny eventy
            if (vel > 0) {
                printf("    %8u | 0x%05X | [NOTE ON]  Ch %2d  Note %3d  Vel %3d  Dur %u\n",
                    total_ticks, event_offset, chan, note, vel, dur);
            }
        }
        else if (type == 0x80) {
            // NOTE OFF — v XMI by neměly být, ale pro robustnost
            uint8_t note = *p++;
            uint8_t vel = *p++;
            printf("    %8u | 0x%05X | [NOTE OFF] Ch %2d  Note %3d  Vel %3d\n",
                total_ticks, event_offset, chan, note, vel);
        }
        else if (type == 0xB0) {
            // CONTROL CHANGE
            uint8_t cc = *p++;
            uint8_t val = *p++;
            if (cc == 0 || cc == 32) {
                printf("    %8u | 0x%05X | [BANK]       Ch %2d  Bank %d\n",
                    total_ticks, event_offset, chan, val);
            }
            else if (cc == 116) {
                printf("    %8u | 0x%05X | [LOOP START] Ch %2d  Val %d  <<<<<<\n",
                    total_ticks, event_offset, chan, val);
            }
            else if (cc == 117) {
                printf("    %8u | 0x%05X | [LOOP END]   Ch %2d  Val %d  >>>>>>\n",
                    total_ticks, event_offset, chan, val);
            }
            else {
                printf("    %8u | 0x%05X | [CC]         Ch %2d  CC %3d  Val %3d\n",
                    total_ticks, event_offset, chan, cc, val);
            }
        }
        else if (type == 0xC0) {
            uint8_t patch = *p++;
            printf("    %8u | 0x%05X | [PATCH]      Ch %2d  Patch %d\n",
                total_ticks, event_offset, chan, patch);
        }
        else if (type == 0xD0) {
            uint8_t pressure = *p++;
            printf("    %8u | 0x%05X | [AFTERTOUCH] Ch %2d  Val %d\n",
                total_ticks, event_offset, chan, pressure);
        }
        else if (type == 0xA0) {
            uint8_t note = *p++;
            uint8_t val = *p++;
            printf("    %8u | 0x%05X | [POLY AT]    Ch %2d  Note %d  Val %d\n",
                total_ticks, event_offset, chan, note, val);
        }
        else if (type == 0xE0) {
            uint8_t lo = *p++;
            uint8_t hi = *p++;
            int bend = ((hi << 7) | lo) - 8192;
            printf("    %8u | 0x%05X | [PITCH BEND] Ch %2d  Val %d\n",
                total_ticks, event_offset, chan, bend);
        }
        else if (status == 0xF0 || status == 0xF7) {
            // SysEx
            uint32_t s_len = 0;
            while (p < end) {
                uint8_t b = *p++;
                s_len = (s_len << 7) | (b & 0x7F);
                if (!(b & 0x80)) break;
            }
            printf("    %8u | 0x%05X | [SYSEX]  len=%u\n",
                total_ticks, event_offset, s_len);
            p += s_len;
        }
        else {
            printf("    %8u | 0x%05X | [UNKNOWN] status=0x%02X\n",
                total_ticks, event_offset, status);
            // Nebezpečné — raději zastav aby se parser neztratil
            break;
        }
    }

    *out_total_ticks = total_ticks;
    *out_total_seconds = total_seconds;
}

// ─────────────────────────────────────────────
// Main XMI parser
// ─────────────────────────────────────────────

static void parse_xmi_sequence(const uint8_t* form_data, uint32_t form_size, int seq_index) {
    printf("\n+==========================================+\n");
    printf("|  Sequence #%d\n", seq_index);
    printf("+==========================================+\n");

    const uint8_t* p = form_data;
    const uint8_t* end = form_data + form_size;

    const uint8_t* timb_data = NULL; uint32_t timb_size = 0;
    const uint8_t* rbrn_data = NULL; uint32_t rbrn_size = 0;
    const uint8_t* evnt_data = NULL; uint32_t evnt_size = 0;

    // Walk chunks inside FORM XMID
    while (p + 8 <= end) {
        char tag[5];
        memcpy(tag, p, 4); tag[4] = 0;
        uint32_t size = read_be32(p + 4);
        const uint8_t* chunk_data = p + 8;
        p += 8 + size + (size & 1); // align to even byte boundary

        if (strcmp(tag, "TIMB") == 0) { timb_data = chunk_data; timb_size = size; }
        else if (strcmp(tag, "RBRN") == 0) { rbrn_data = chunk_data; rbrn_size = size; }
        else if (strcmp(tag, "EVNT") == 0) { evnt_data = chunk_data; evnt_size = size; }
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
    RbrnChunk rbrn;
    rbrn.count = 0;
    if (rbrn_data) {
        parse_RBRN(rbrn_data, rbrn_size, &rbrn);
        printf("\n  [RBRN] %d branch marker(s):\n", rbrn.count);
        for (int i = 0; i < rbrn.count; i++) {
            printf("    Marker ID=%d  EVNT offset=%u (0x%04X)\n",
                rbrn.markers[i].id,
                rbrn.markers[i].offset,
                rbrn.markers[i].offset);
        }
    }
    else {
        printf("\n  [RBRN] not present - no loops or branch points\n");
    }

    // ── EVNT ──
    if (evnt_data) {
        printf("\n  [EVNT] size: %u bytes\n", evnt_size);
        uint32_t total_ticks;
        double   total_seconds;
        analyze_EVNT(evnt_data, evnt_size, &rbrn, &total_ticks, &total_seconds);
        printf("\n  Total ticks : %u\n", total_ticks);
        printf("  Total time  : %.2f s (%.2f min)\n", total_seconds, total_seconds / 60.0);

        // Show marker positions in time
        if (rbrn.count > 0) {
            printf("\n  Marker positions (estimated at 120 BPM / 500000 us):\n");
            for (int i = 0; i < rbrn.count; i++) {
                // XMI: offset is in ticks (120 ticks/beat)
                double marker_sec = (double)rbrn.markers[i].offset / 120.0 * 0.5;
                printf("    Marker ID=%-3d  offset=%-6u ticks  ~%.2f s  ~%.2f min\n",
                    rbrn.markers[i].id,
                    rbrn.markers[i].offset,
                    marker_sec,
                    marker_sec / 60.0);
            }
        }
    }
    else {
        printf("\n  [EVNT] not present!\n");
    }
}

static void parse_xmi_file(const uint8_t* data, size_t size) {
    if (size < 12) { printf("Error: File is too small.\n"); return; }

    // Verify FORM header
    if (memcmp(data, "FORM", 4) != 0) {
        printf("Error: Not an XMI file (FORM header missing).\n"); return;
    }
    uint32_t form_size = read_be32(data + 4);

    // XMID, CAT or XDIR?
    if (memcmp(data + 8, "XMID", 4) == 0) {
        // Single track
        printf("Type: FORM XMID (1 sequence)\n");
        parse_xmi_sequence(data + 12, form_size - 4, 0);
    }
    else if (memcmp(data + 8, "CAT ", 4) == 0) {
        // Multiple tracks in CAT container
        if (memcmp(data + 12, "XMID", 4) != 0) {
            printf("Error: CAT is not XMID.\n"); return;
        }
        printf("Type: CAT XMID (multiple sequences)\n");
        const uint8_t* p = data + 16;
        const uint8_t* end = data + 8 + form_size;
        int seq_idx = 0;
        while (p + 8 <= end) {
            if (memcmp(p, "FORM", 4) != 0) { p++; continue; }
            uint32_t seq_size = read_be32(p + 4);
            if (memcmp(p + 8, "XMID", 4) == 0) {
                parse_xmi_sequence(p + 12, seq_size - 4, seq_idx++);
            }
            p += 8 + seq_size + (seq_size & 1);
        }
        printf("\nTotal sequences: %d\n", seq_idx);
    }
    else if (memcmp(data + 8, "XDIR", 4) == 0) {
        printf("Type: FORM XDIR (directory container)\n");

        // Read INFO from inside FORM XDIR
        const uint8_t* xdir_end = data + 8 + form_size;
        const uint8_t* p = data + 12;
        while (p + 8 <= xdir_end) {
            uint32_t csz = read_be32(p + 4);
            printf("  XDIR chunk: '%.4s'  size=%u\n", p, csz);
            if (memcmp(p, "INFO", 4) == 0 && csz >= 2) {
                // INFO count can be little-endian or big-endian depending on version
                uint16_t cnt_be = read_be16(p + 8);
                uint16_t cnt_le = (uint16_t)((p[8]) | (p[9] << 8));
                printf("INFO: %u sequence(s) declared (le=%u)\n", cnt_be, cnt_le);
            }
            if (csz == 0) break;
            p += 8 + csz + (csz & 1);
        }

        // CAT XMID follows FORM XDIR at the top level of the file
        // i.e. right after FORM XDIR block
        const uint8_t* cat_ptr = data + 8 + form_size + (form_size & 1);
        printf("\n  Looking for CAT at offset %u (0x%X)...\n",
            (unsigned)(cat_ptr - data), (unsigned)(cat_ptr - data));

        if (cat_ptr + 12 <= data + size &&
            memcmp(cat_ptr, "CAT ", 4) == 0) {
            uint32_t cat_size = read_be32(cat_ptr + 4);
            printf("  Found CAT '%.4s'  size=%u\n", cat_ptr + 8, cat_size);

            const uint8_t* cp = cat_ptr + 12;
            const uint8_t* cend = cat_ptr + 8 + cat_size;
            int seq_idx = 0;
            while (cp + 8 <= cend) {
                if (memcmp(cp, "FORM", 4) != 0) { cp++; continue; }
                uint32_t seq_size = read_be32(cp + 4);
                if (memcmp(cp + 8, "XMID", 4) == 0) {
                    parse_xmi_sequence(cp + 12, seq_size - 4, seq_idx++);
                }
                else {
                    printf("  Skipping FORM '%.4s' size=%u\n", cp + 8, seq_size);
                }
                cp += 8 + seq_size + (seq_size & 1);
            }
            printf("\nTotal sequences parsed: %d\n", seq_idx);
        }
        else {
            printf("  CAT not found at expected offset.\n");
            printf("  Bytes at that offset: %02X %02X %02X %02X\n",
                cat_ptr[0], cat_ptr[1], cat_ptr[2], cat_ptr[3]);
        }
    }
    else {
        printf("Error: Unknown FORM type (%.4s).\n", data + 8);
    }
}

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main(int argc, char* argv[]) {
    printf("===========================================\n");
    printf("  XMI Dump - XMI file analyzer\n");
    printf("===========================================\n\n");

    if (argc < 2) {
        printf("Usage:   xmi_dump.exe <file.xmi>\n");
        printf("Example: xmi_dump.exe music0.xmi\n");
        return 1;
    }

    FILE* f = nullptr;
    fopen_s(&f, argv[1], "rb");
    if (!f) {
        printf("Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(file_size);
    if (!buffer) { printf("Error: Out of memory.\n"); fclose(f); return 1; }

    fread(buffer, 1, file_size, f);
    fclose(f);

    printf("File  : %s\n", argv[1]);
    printf("Size  : %ld bytes\n", file_size);

    parse_xmi_file(buffer, file_size);

    free(buffer);
    printf("\n===========================================\n");
    printf("  Done.\n");
    printf("===========================================\n");
    return 0;
}