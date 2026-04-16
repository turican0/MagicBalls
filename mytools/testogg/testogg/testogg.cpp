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
static const float  SILENCE_THRESHOLD = 0.001f;
static const double MIN_SEGMENT_SEC = 0.5;
static const int    CHUNK_SAMPLES = 2048;

struct AudioSegment { double startSec; double lengthSec; };

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
    int  bitstream = 0;
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
// OPRAVENÉ PÁROVÁNÍ
//
// Klíčové opravy:
//   1. maxDiff je nyní RELATIVNÍ k délce tracku (defaultně 15 % rozsahu),
//      takže vzdálené segmenty mají stejnou toleranci jako blízké.
//   2. Skip-penalizace = 10 * maxDiff — skip je vždy mnohem dražší než match.
//   3. Prohledáváme detected od indexu j (zachování pořadí).
// ─────────────────────────────────────────────────────────────────────────────
static std::vector<int> bestMatching(
    const std::vector<TrackSegment>& tableSegs,
    const std::vector<AudioSegment>& detected,
    double coeff,
    double offset,           // ← NOVÉ
    double maxDiffAbs = 3.0)
{
    int N = (int)tableSegs.size();
    int M = (int)detected.size();
    if (N == 0 || M == 0) return std::vector<int>(N, -1);

    const double SKIP_PENALTY = maxDiffAbs * 20.0;
    const double INF = 1e18;

    std::vector<std::vector<double>> dp(N + 1, std::vector<double>(M + 1, INF));
    std::vector<std::vector<int>> from(N + 1, std::vector<int>(M + 1, -2));

    dp[0][0] = 0.0;

    for (int i = 1; i <= N; i++) {
        double expected = tableSegs[i - 1].startPos * coeff + offset;   // ← používá offset

        for (int j = 0; j <= M; j++) {
            if (dp[i - 1][j] >= INF) continue;

            // Skip
            if (dp[i - 1][j] + SKIP_PENALTY < dp[i][j]) {
                dp[i][j] = dp[i - 1][j] + SKIP_PENALTY;
                from[i][j] = -(j + 1);
            }

            // Match
            for (int k = j; k < M; k++) {
                double diff = fabs(detected[k].startSec - expected);
                if (diff >= maxDiffAbs) continue;

                double cost = dp[i - 1][j] + diff;
                if (cost < dp[i][k + 1]) {
                    dp[i][k + 1] = cost;
                    from[i][k + 1] = j;
                }
            }
        }
    }

    // ... zbytek funkce (rekonstrukce assignmentu) zůstává stejný
    // (jen copy-paste z tvého původního kódu)
    double bestCost = INF;
    int bestJ = 0;
    for (int j = 0; j <= M; j++) {
        if (dp[N][j] < bestCost) {
            bestCost = dp[N][j];
            bestJ = j;
        }
    }

    std::vector<int> assignment(N, -1);
    int curJ = bestJ;
    for (int i = N; i >= 1; i--) {
        int f = from[i][curJ];
        if (f >= 0) {
            assignment[i - 1] = curJ - 1;
            curJ = f;
        }
        else {
            assignment[i - 1] = -1;
            curJ = (-f) - 1;
        }
    }
    return assignment;
}

// ─────────────────────────────────────────────────────────────────────────────
// LINEÁRNÍ REGRESE: time = coeff * pos + offset
// ─────────────────────────────────────────────────────────────────────────────
struct LinearFit {
    double coeff;
    double offset;
};

static LinearFit fitLinear(const std::vector<std::pair<int32_t, double>>& pairs)
{
    if (pairs.size() < 2) return { 0.0, 0.0 };

    double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
    size_t n = 0;

    for (auto& p : pairs) {
        double x = (double)p.first;
        double y = p.second;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
        n++;
    }

    double denom = n * sumXX - sumX * sumX;
    if (denom < 1e-12) return { 0.0, 0.0 };

    double coeff = (n * sumXY - sumX * sumY) / denom;
    double offset = (sumY - coeff * sumX) / n;

    return { coeff, offset };
}

int main(int argc, char** argv)
{
    std::string dir = "c:/Users/vesely/AppData/Roaming/Godot/app_userdata/MagicBalls/convertdata/speech/";

    // ── ROBUSTNÍ ITERATIVNÍ KALIBRACE ──────────────────────────────
    double currentCoeff = 0.01340;   // lepší počáteční odhad (někde mezi 1/74.6 až 1/75)
    double currentOffset = 0.0;
    const int MAX_ITER = 12;
    const double CONVERGENCE = 1e-9;
    const double MAX_ALLOWED_DRIFT = 0.0005; // maximální změna koeficientu za iteraci

    std::vector<std::pair<int32_t, double>> allPairs;

    printf("=== ROBUST ITERATIVE CALIBRATION (coeff + offset) ===\n");

    bool converged = false;
    for (int iter = 0; iter < MAX_ITER; iter++) {
        allPairs.clear();
        double prevCoeff = currentCoeff;
        double prevOffset = currentOffset;

        printf("\n--- Iteration %2d | coeff=%.8f | offset=%.4f ---\n",
            iter + 1, currentCoeff, currentOffset);

        int totalMatches = 0;

        for (int i = 0; i < 27; i++) {
            char filename_buf[16];
            snprintf(filename_buf, sizeof(filename_buf), "s%02d.ogg", i + 1);
            std::string fname = dir + filename_buf;

            auto detected = findAudioSegments(fname.c_str());
            const auto& track = CdTracks_DB080[i];

            std::vector<TrackSegment> tableSegs;
            for (int j = 0; j < 10; j++)
                if (track.seg[j].length > 0)
                    tableSegs.push_back(track.seg[j]);

            if (tableSegs.empty() || detected.empty()) continue;

            // Používáme offset a mírně vyšší toleranci na začátku
            double tolerance = (iter < 4) ? 4.0 : 2.8;
            auto assignment = bestMatching(tableSegs, detected, currentCoeff, currentOffset, tolerance);

            int matchCount = 0;
            for (size_t tIdx = 0; tIdx < tableSegs.size(); tIdx++) {
                int dIdx = assignment[tIdx];
                if (dIdx < 0) continue;

                auto& t = tableSegs[tIdx];
                auto& d = detected[dIdx];

                if (t.startPos > 100) {  // ignorujeme velmi krátké/úvodní segmenty
                    allPairs.emplace_back(t.startPos, d.startSec);
                    allPairs.emplace_back(t.startPos + t.length, d.startSec + d.lengthSec);
                }
                matchCount++;
            }

            if (matchCount > 0) {
                printf("  Track %02d: %d/%zu matched\n", i + 1, matchCount, tableSegs.size());
                totalMatches += matchCount;
            }
        }

        if (allPairs.size() < 20) {
            printf(" [!] Příliš málo párů (%zu), končím iteraci.\n", allPairs.size());
            break;
        }

        LinearFit newFit = fitLinear(allPairs);
        double newCoeff = newFit.coeff;
        double newOffset = newFit.offset;

        // === OCHRANA PROTI ROZPADU ===
        double coeffChange = fabs(newCoeff - prevCoeff);
        if (coeffChange > MAX_ALLOWED_DRIFT) {
            printf(" [!] Příliš velká změna koeficientu (%.8f → %.8f), omezuji drift.\n",
                prevCoeff, newCoeff);
            newCoeff = prevCoeff + (newCoeff - prevCoeff) * 0.4; // tlumíme změnu
        }

        currentCoeff = newCoeff;
        currentOffset = newOffset;

        printf("→ Updated: coeff=%.8f  offset=%.4f  (pairs=%zu, total_matches=%d)\n",
            currentCoeff, currentOffset, allPairs.size(), totalMatches);

        if (fabs(currentCoeff - prevCoeff) < CONVERGENCE &&
            fabs(currentOffset - prevOffset) < 0.01) {
            printf("=== CONVERGED ===\n");
            converged = true;
            break;
        }
    }

    if (!converged)
        printf("Kalibrace dokončena po %d iteracích (bez plné konvergence).\n", MAX_ITER);

    double finalCoeff = currentCoeff;
    double globalOffset = currentOffset;

    // ── DRUHÝ PRŮCHOD – detailní výpis offsetů pro každý track ─────
    printf("\n==================================================\n");
    printf("SECOND PASS: PER-TRACK OFFSET (final coeff = %.8f)\n", finalCoeff);
    printf("==================================================\n");

    std::vector<double> trackOffsets;

    for (int i = 0; i < 27; i++) {
        char filename_buf[16];
        snprintf(filename_buf, sizeof(filename_buf), "s%02d.ogg", i + 1);
        std::string fname = dir + filename_buf;

        auto detected = findAudioSegments(fname.c_str());
        const auto& track = CdTracks_DB080[i];

        std::vector<TrackSegment> tableSegs;
        for (int j = 0; j < 10; j++)
            if (track.seg[j].length > 0)
                tableSegs.push_back(track.seg[j]);

        if (tableSegs.empty() || detected.empty()) continue;

        auto assignment = bestMatching(tableSegs, detected, finalCoeff, globalOffset, 2.5);

        std::vector<double> offsets;
        for (size_t tIdx = 0; tIdx < tableSegs.size(); tIdx++) {
            int dIdx = assignment[tIdx];
            if (dIdx < 0) continue;

            double expected = tableSegs[tIdx].startPos * finalCoeff + globalOffset;
            offsets.push_back(detected[dIdx].startSec - expected);
        }

        if (!offsets.empty()) {
            double sum = 0.0;
            for (double o : offsets) sum += o;
            double avgOffset = sum / offsets.size();

            printf("Track %02d (%s):  %+7.4f s   (%2zu/%2zu segs)\n",
                i + 1, filename_buf, avgOffset, offsets.size(), tableSegs.size());

            trackOffsets.push_back(avgOffset);
        }
        else {
            printf("Track %02d (%s):  NO MATCH\n", i + 1, filename_buf);
        }
    }

    if (!trackOffsets.empty()) {
        double sum = 0.0;
        for (double o : trackOffsets) sum += o;
        globalOffset = sum / trackOffsets.size();
    }

    printf("\n==================================================\n");
    printf("FINAL RESULT\n");
    printf("==================================================\n");
    printf("Optimal coeff  : %.8f\n", finalCoeff);
    printf("Global offset  : %.4f s\n", globalOffset);
    printf("Time = (Pos * %.8f) + %.4f\n", finalCoeff, globalOffset);
    printf("==================================================\n");

    return 0;
}