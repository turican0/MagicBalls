// analyze_segments.cpp
// Compile: g++ -O2 -o analyze_segments analyze_segments.cpp -lvorbisfile -lvorbis -logg

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <vorbis/vorbisfile.h>

// ─────────────────────────────────────────────────────────────────────────────
// DB080 table structure
// ─────────────────────────────────────────────────────────────────────────────
#pragma pack(1)
struct TrackSegment { int32_t startPos; int32_t length; };
struct Type_DB080_CdTrack { int16_t trackIdx; TrackSegment seg[10]; };
#pragma pack()

// Data truncated for brevity in display, use your full table here
Type_DB080_CdTrack CdTracks_DB080[28] = {
{0x0001,{{0x0000,0x02EE},{0x0339,0x012C},{0x04B0,0x01C2},{0x06BD,0x01C2},{0x08CA,0x0177},{0x0A8C,0x0177},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0C4E,0x020D}}},
{0x0002,{{0x0000,0x0465},{0x04B0,0x020D},{0x0708,0x0258},{0x09AB,0x012C},{0x0B22,0x020D},{0x0D7A,0x01C2},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0F87,0x0258}}},
{0x0003,{{0x0000,0x0384},{0x03CF,0x02A3},{0x06BD,0x0177},{0x087F,0x01C2},{0x0A8C,0x0177},{0x0C4E,0x012C},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0DC5,0x0177}}},
{0x0004,{{0x0000,0x020D},{0x0258,0x012C},{0x03CF,0x01C2},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x05DC,0x0258}}},
{0x0005,{{0x0000,0x02EE},{0x0339,0x0177},{0x04FB,0x012C},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0672,0x02A3}}},
{0x0006,{{0x0000,0x0384},{0x03CF,0x0177},{0x0591,0x0177},{0x0753,0x020D},{0x09AB,0x01C2},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0BB8,0x0177}}},
{0x0007,{{0x0000,0x0465},{0x04B0,0x0177},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0672,0x020D}}},
{0x0008,{{0x0000,0x0384},{0x03CF,0x01C2},{0x05DC,0x0177},{0x079E,0x012C},{0x0915,0x012C},{0x0A8C,0x0258},{0x0D2F,0x020D},{0x0000,0x0000},{0x0000,0x0000},{0x0F87,0x012C}}},
{0x0009,{{0x0000,0x03CF},{0x041A,0x0177},{0x05DC,0x0096},{0x06BD,0x00E1},{0x07E9,0x020D},{0x0A41,0x020D},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0C99,0x020D}}},
{0x000A,{{0x0000,0x02A3},{0x02EE,0x020D},{0x0546,0x0177},{0x0708,0x0258},{0x09AB,0x020D},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0C03,0x012C}}},
{0x000B,{{0x0000,0x0258},{0x02A3,0x0177},{0x0465,0x020D},{0x06BD,0x0177},{0x087F,0x01C2},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0A8C,0x012C}}},
{0x000C,{{0x0000,0x0546},{0x0591,0x0258},{0x0834,0x01C2},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0A41,0x012C}}},
{0x000D,{{0x0000,0x03CF},{0x041A,0x01C2},{0x0627,0x0177},{0x07E9,0x0177},{0x09AB,0x012C},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0B22,0x01C2}}},
{0x000E,{{0x0000,0x02EE},{0x0339,0x02A3},{0x0627,0x020D},{0x087F,0x01C2},{0x0A8C,0x0177},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0C4E,0x020D}}},
{0x000F,{{0x0000,0x0465},{0x04B0,0x00E1},{0x05DC,0x020D},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0834,0x020D}}},
{0x0010,{{0x0000,0x0339},{0x0384,0x020D},{0x05DC,0x00E1},{0x0708,0x00E1},{0x0834,0x0177},{0x09F6,0x012C},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0B6D,0x0177}}},
{0x0011,{{0x0000,0x0465},{0x04B0,0x0177},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0672,0x012C}}},
{0x0012,{{0x0000,0x05DC},{0x0627,0x01C2},{0x0834,0x012C},{0x09AB,0x01C2},{0x0BB8,0x0177},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0D7A,0x012C}}},
{0x0013,{{0x0000,0x020D},{0x0258,0x012C},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x03CF,0x00E1}}},
{0x0014,{{0x0000,0x0339},{0x0384,0x020D},{0x05DC,0x020D},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0834,0x01C2}}},
{0x0015,{{0x0000,0x0339},{0x0384,0x01C2},{0x0591,0x0177},{0x0753,0x020D},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x09AB,0x0177}}},
{0x0016,{{0x0000,0x0465},{0x04B0,0x00E1},{0x05DC,0x020D},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0834,0x01C2}}},
{0x0017,{{0x0000,0x03CF},{0x041A,0x012C},{0x0591,0x0177},{0x0753,0x020D},{0x09AB,0x012C},{0x0B22,0x012C},{0x0C99,0x01C2},{0x0EA6,0x0177},{0x0000,0x0000},{0x1068,0x012C}}},
{0x0018,{{0x0000,0x0339},{0x0384,0x0177},{0x0546,0x020D},{0x079E,0x012C},{0x0915,0x012C},{0x0A8C,0x012C},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0C03,0x0177}}},
{0x0019,{{0x0000,0x04B0},{0x0000,0x0000},{0x04FB,0x02A3},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x07E9,0x0627}}},
{0x001A,{{0x0000,0x0177},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000}}},
{0x001B,{{0x0000,0x0177},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000}}},
{0x001C,{{0x5BD0,0x0107},{0x5BE0,0x0107},{0x5BF0,0x0107},{0x5C00,0x0107},{0x5C10,0x0107},{0x5C24,0x0107},{0x5C34,0x0107},{0x5C44,0x0107},{0x5C60,0x0107},{0x5C70,0x0107}}}
};

// ─────────────────────────────────────────────────────────────────────────────
// Segment detection parameters
// ─────────────────────────────────────────────────────────────────────────────
static const float  SILENCE_THRESHOLD = 0.001f;  // RMS amplitude threshold
static const double MIN_SEGMENT_SEC = 0.5;     // ignore very short noises
static const int    CHUNK_SAMPLES = 2048;

struct AudioSegment { double startSec; double lengthSec; };

// Function to find areas with SOUND (not silence)
static std::vector<AudioSegment> findAudioSegments(const char* filename)
{
    std::vector<AudioSegment> result;
    OggVorbis_File vf;
    if (ov_fopen(filename, &vf) != 0) {
        fprintf(stderr, "  [!] Cannot open %s\n", filename);
        return result;
    }

    vorbis_info* vi = ov_info(&vf, -1);
    int channels = vi->channels;

    bool   inAudio = false;
    double segStart = 0.0;
    double curPos = 0.0;

    float** pcm;
    int bitstream = 0;
    long n;

    while (true) {
        n = ov_read_float(&vf, &pcm, CHUNK_SAMPLES, &bitstream);
        if (n <= 0) break;

        double rms = 0.0;
        for (int c = 0; c < channels; c++)
            for (long s = 0; s < n; s++)
                rms += (double)pcm[c][s] * pcm[c][s];
        rms = sqrt(rms / (n * channels));

        double chunkDur = (double)n / vi->rate;
        bool   hasSound = rms >= SILENCE_THRESHOLD;

        if (hasSound && !inAudio) {
            inAudio = true;
            segStart = curPos;
        }
        else if (!hasSound && inAudio) {
            double len = curPos - segStart;
            if (len >= MIN_SEGMENT_SEC)
                result.push_back({ segStart, len });
            inAudio = false;
        }
        curPos += chunkDur;
    }

    if (inAudio) {
        double len = curPos - segStart;
        if (len >= MIN_SEGMENT_SEC)
            result.push_back({ segStart, len });
    }

    ov_clear(&vf);
    return result;
}

// Get segment start positions from table (including 0x0000)
static std::vector<int32_t> tableStarts(int trackIdx)
{
    std::vector<int32_t> v;
    const Type_DB080_CdTrack& t = CdTracks_DB080[trackIdx];
    // We check slots 0..8 (slot 9 is often a total/special marker)
    for (int i = 0; i < 9; i++) {
        // Only include if segment has non-zero length in table
        if (t.seg[i].length > 0)
            v.push_back(t.seg[i].startPos);
    }
    return v;
}

static double fitCoeff(const std::vector<std::pair<int32_t, double>>& pairs)
{
    double sumXY = 0, sumXX = 0;
    for (auto& p : pairs) {
        sumXY += p.first * p.second;
        sumXX += (double)p.first * p.first;
    }
    return sumXX > 0 ? sumXY / sumXX : 0.0;
}

// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char** argv)
{
    std::string dir = "c:/Users/t.vesely/AppData/Roaming/Godot/app_userdata/MagicBalls/convertdata/speech/";

    // Prvotní odhad koeficientu (např. CDDA frame rate nebo 1/75 s), 
    // bude se zpřesňovat, pokud najdeme jasné shody.
    double currentCoeff = 0.01333333;
    std::vector<std::pair<int32_t, double>> allPairs;

    for (int i = 0; i < 27; i++) {
        char filename_buf[16];
        snprintf(filename_buf, sizeof(filename_buf), "s%02d.ogg", i + 1);
        std::string fname = dir + filename_buf;

        auto detected = findAudioSegments(fname.c_str());
        const auto& track = CdTracks_DB080[i];

        std::vector<TrackSegment> tableSegs;
        for (int j = 0; j < 10; j++) {
            if (track.seg[j].length > 0) tableSegs.push_back(track.seg[j]);
        }

        printf("##################################################\n");
        printf("TRACK %02d (%s) - Tab: %zu, Det: %zu\n", i + 1, filename_buf, tableSegs.size(), detected.size());
        printf("##################################################\n");

        // --- HEURISTICKÉ PÁROVÁNÍ ---
        // Pro každý segment v tabulce hledáme nejbližší detekovaný v OGG
        for (size_t tIdx = 0; tIdx < tableSegs.size(); tIdx++) {
            double expectedStart = tableSegs[tIdx].startPos * currentCoeff;

            int bestMatch = -1;
            double minDiff = 999.0;

            for (size_t dIdx = 0; dIdx < detected.size(); dIdx++) {
                double diff = fabs(detected[dIdx].startSec - expectedStart);
                if (diff < minDiff) {
                    minDiff = diff;
                    bestMatch = (int)dIdx;
                }
            }

            // Pokud je shoda rozumná (rozdíl < 0.5s), spárujeme je
            if (bestMatch != -1 && minDiff < 0.5) {
                auto& d = detected[bestMatch];
                auto& t = tableSegs[tIdx];

                if (t.startPos > 0) allPairs.push_back({ t.startPos, d.startSec });
                allPairs.push_back({ t.startPos + t.length, d.startSec + d.lengthSec });

                printf("  MATCH TabSeg[%zu] <-> OggSeg[%d]: diff=%.3fs\n", tIdx, bestMatch, minDiff);
                printf("    Start: 0x%04X -> %7.3fs | End: 0x%04X -> %7.3fs\n",
                    t.startPos, d.startSec, t.startPos + t.length, d.startSec + d.lengthSec);

                // Průběžná aktualizace koeficientu pro lepší hledání v dalších souborech
                if (allPairs.size() > 10) currentCoeff = fitCoeff(allPairs);
            }
            else {
                printf("  SKIP  TabSeg[%zu]: Nenalezena odpovídající oblast v OGG (očekáváno cca %.3fs)\n", tIdx, expectedStart);
            }
        }
        printf("\n");
    }

    if (!allPairs.empty()) {
        double finalCoeff = fitCoeff(allPairs);

        // --- DRUHÝ PRŮCHOD: ZJIŠTĚNÍ POSUNU (OFFSETU) ---
        printf("\n==================================================\n");
        printf("SECOND PASS: OFFSET ANALYSIS\n");
        printf("==================================================\n");

        std::vector<double> trackOffsets;

        for (int i = 0; i < 27; i++) {
            const auto& track = CdTracks_DB080[i];

            // Znovu načteme detekované segmenty pro tento konkrétní track
            char filename_buf[16];
            snprintf(filename_buf, sizeof(filename_buf), "s%02d.ogg", i + 1);
            std::string fname = dir + filename_buf;
            auto detected = findAudioSegments(fname.c_str());

            std::vector<double> currentTrackOffsets;

            // Porovnáváme segmenty s použitím fixního koeficientu
            for (int j = 0; j < 10; j++) {
                if (track.seg[j].length <= 0) continue;

                double tableStart = track.seg[j].startPos;
                double expectedStartWithNoOffset = tableStart * finalCoeff;

                // Najdeme nejbližší detekovaný segment
                double bestOffset = 0;
                double minDiff = 999.0;
                bool found = false;

                for (const auto& d : detected) {
                    // Offset = Skutečný start v OGG - (Pozice v tabulce * Koeficient)
                    double diff = fabs(d.startSec - expectedStartWithNoOffset);
                    if (diff < 0.5) { // Práh citlivosti 0.5s
                        if (diff < minDiff) {
                            minDiff = diff;
                            bestOffset = d.startSec - expectedStartWithNoOffset;
                            found = true;
                        }
                    }
                }

                if (found) {
                    currentTrackOffsets.push_back(bestOffset);
                }
            }

            // Vypočítáme průměrný posun pro tento konkrétní track
            if (!currentTrackOffsets.empty()) {
                double avgOffset = 0;
                for (double o : currentTrackOffsets) avgOffset += o;
                avgOffset /= currentTrackOffsets.size();

                printf("Track %02d (%s): Offset = %+8.4f s (z %zu bodů)\n",
                    i + 1, filename_buf, avgOffset, currentTrackOffsets.size());

                trackOffsets.push_back(avgOffset);
            }
            else {
                printf("Track %02d (%s): Offset nelze spolehlivě určit.\n", i + 1, filename_buf);
            }
        }

        // Celkové shrnutí
        double globalOffset = 0;
        if (!trackOffsets.empty()) {
            for (double o : trackOffsets) globalOffset += o;
            globalOffset /= trackOffsets.size();
        }

        printf("==================================================\n");
        printf("FINAL CALIBRATION DATA\n");
        printf("==================================================\n");
        printf("Optimal Coeff  : %.8f\n", finalCoeff);
        printf("Global Offset  : %.4f s\n", globalOffset);
        printf("Formula        : Time = (Pos * %.8f) + (%.4f)\n", finalCoeff, globalOffset);
        printf("==================================================\n");
    }

    double finalCoeff = fitCoeff(allPairs);
    // --- DRUHÝ PRŮCHOD S ROZŠÍŘENOU TOLERANCÍ PRO DROBNÉ CHYBY ---
    printf("\n==================================================\n");
    printf("SECOND PASS: SEARCHING FOR TRACK OFFSETS\n");
    printf("==================================================\n");

    for (int i = 0; i < 27; i++) {
        const auto& track = CdTracks_DB080[i];
        char filename_buf[16];
        snprintf(filename_buf, sizeof(filename_buf), "s%02d.ogg", i + 1);
        std::string fname = dir + filename_buf;
        auto detected = findAudioSegments(fname.c_str());

        // Musíme mít aspoň nějaké segmenty v tabulce i v OGG
        std::vector<TrackSegment> tableSegs;
        for (int j = 0; j < 10; j++) {
            if (track.seg[j].length > 0) tableSegs.push_back(track.seg[j]);
        }

        if (tableSegs.empty() || detected.empty()) {
            printf("Track %02d (%s): Chybí data pro analýzu.\n", i + 1, filename_buf);
            continue;
        }

        // Hledáme globální offset pro celý tento soubor.
        // Princip: Zkusíme napasovat první detekovaný segment na první tabulkový 
        // a ověříme, kolik dalších segmentů to "vysvětlí".

        double bestGlobalOffset = 0;
        int maxConfirmedSegments = -1;
        double bestAverageDiff = 999.0;

        // Zkusíme každou možnou dvojici jako potenciální startovní bod
        for (size_t dStartIdx = 0; dStartIdx < detected.size(); dStartIdx++) {
            double potentialOffset = detected[dStartIdx].startSec - (tableSegs[0].startPos * finalCoeff);

            int confirmed = 0;
            double currentTotalDiff = 0;

            for (size_t tIdx = 0; tIdx < tableSegs.size(); tIdx++) {
                double expectedWithOffset = (tableSegs[tIdx].startPos * finalCoeff) + potentialOffset;

                // Najdeme nejbližší segment v OGG pro tento očekávaný čas
                double localMinDiff = 999.0;
                for (const auto& d : detected) {
                    double dDiff = fabs(d.startSec - expectedWithOffset);
                    if (dDiff < localMinDiff) localMinDiff = dDiff;
                }

                if (localMinDiff < 0.2) { // Pokud se trefíme do 200ms, započítáme shodu
                    confirmed++;
                    currentTotalDiff += localMinDiff;
                }
            }

            // Pokud jsme našli lepší shodu (víc potvrzených segmentů nebo menší průměrná chyba)
            if (confirmed > maxConfirmedSegments || (confirmed == maxConfirmedSegments && currentTotalDiff < bestAverageDiff)) {
                maxConfirmedSegments = confirmed;
                bestAverageDiff = currentTotalDiff;
                bestGlobalOffset = potentialOffset;
            }
        }

        if (maxConfirmedSegments > 0) {
            printf("Track %02d (%s): Offset = %+8.4f s (Potvrzeno %d/%zu segmentů)\n",
                i + 1, filename_buf, bestGlobalOffset, maxConfirmedSegments, tableSegs.size());
        }
        else {
            printf("Track %02d (%s): Offset stále nebyl nalezen (pravděpodobně nesedí struktura).\n", i + 1, filename_buf);
        }
    }

    return 0;
}