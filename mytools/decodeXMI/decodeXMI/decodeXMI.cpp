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
    out->count = count;
    for (int i = 0; i < count && i < 128; i++) {
        out->timbres[i].patch = data[2 + i * 2];
        out->timbres[i].bank = data[3 + i * 2];
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
    uint8_t running_status = 0;

    printf("\n    [Bullfrog XMI Scan: Patches, Banks & Loops]\n");
    printf("    Time (s) | Offset  | Event Details\n");
    printf("    ---------|---------|----------------------------------------\n");

    while (p < end) {
        uint32_t offset = (uint32_t)(p - evnt_data);

        // Kontrola RBRN markerů (Bullfrog branching body)
        for (int i = 0; i < rbrn->count; i++) {
            if (rbrn->markers[i].offset == offset) {
                printf("    %7.2f  | 0x%05X | >>> BRANCH POINT ID %d <<<\n", total_seconds, offset, rbrn->markers[i].id);
            }
        }

        // 1. Čtení Delta Time (XMI VLQ)
        int vlq_len = 0;
        uint32_t delta = (*p < 0x80) ? *p++ : read_vlq(p, &vlq_len);
        if (vlq_len > 0) p += vlq_len;

        total_ticks += delta;
        total_seconds += (double)delta * (tempo / 1000000.0) / 120.0;

        if (p >= end) break;

        // 2. Status Byte a Running Status (Klíčové pro Bullfrog!)
        uint8_t status = *p;
        if (status < 0x80) {
            status = running_status;
        }
        else {
            status = *p++;
            if (status < 0xF0) running_status = status;
        }

        uint8_t type = status & 0xF0;
        uint8_t chan = status & 0x0F;

        // 3. Detekce Patch / Bank / Loop
        if (type == 0xC0) { // PATCH CHANGE
            uint8_t patch = *p++;
            printf("    %7.2f  | 0x%05X | [PATCH] Ch %d -> #%d\n", total_seconds, offset, chan, patch);
        }
        else if (type == 0xB0) { // CONTROL CHANGE (Banky a Smyčky)
            uint8_t cc = *p++;
            uint8_t val = *p++;
            if (cc == 0 || cc == 32) {
                printf("    %7.2f  | 0x%05X | [BANK] Ch %d -> Bank %d\n", total_seconds, offset, chan, val);
            }
            else if (cc == 116) {
                printf("    %7.2f  | 0x%05X | [LOOP START] (CC 116) <<<<<<\n", total_seconds, offset);
            }
            else if (cc == 117) {
                printf("    %7.2f  | 0x%05X | [LOOP JUMP/END] (CC 117, Val: %d) >>>>>>\n", total_seconds, offset, val);
            }
        }
        else if (status == 0xFF) { // META (Tempo, Text)
            uint8_t m_type = *p++;
            int m_len_b = 0;
            uint32_t m_len = read_vlq(p, &m_len_b);
            p += m_len_b;
            if (m_type == 0x51) {
                tempo = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
            }
            else if (m_type == 0x06 || m_type == 0x07) {
                printf("    %7.2f  | 0x%05X | [MARKER] \"%.*s\"\n", total_seconds, offset, (int)m_len, p);
            }
            p += m_len;
            running_status = 0;
        }
        else if (type == 0x90) { // NOTE ON (XMI Specifikum - má délku tónu navíc!)
            p += 2; // note, velocity
            int dur_len = 0;
            read_vlq(p, &dur_len); // Skip XMI Duration
            p += dur_len;
        }
        // Přeskakování ostatních MIDI dat, aby se parser neztratil
        else if (type == 0x80 || type == 0xA0 || type == 0xE0) p += 2;
        else if (type == 0xD0) p += 1;
        else if (status == 0xF0 || status == 0xF7) {
            int s_len_b = 0;
            uint32_t s_len = read_vlq(p, &s_len_b);
            p += s_len_b + s_len;
            running_status = 0;
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