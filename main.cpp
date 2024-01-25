// system headers
#include <iostream>

// define bitboard data type
#define U64 unsigned long long

class CBoard {
    U64 whitePawns;
    U64 whiteKnights;
    U64 whiteBishops;
    U64 whiteRooks;
    U64 whiteQueens;
    U64 whiteKing;

    U64 blackPawns;
    U64 blackKnights;
    U64 blackBishops;
    U64 blackRooks;
    U64 blackQueens;
    U64 blackKing;
};

/*********************\
 ======================
   Bit Manipulations
 ======================
\*********************/

// set/get/pop macros
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)

// count bits
static inline int countBits(U64 bitboard) {
    int count = 0;
    // until no bits on board are set (uncounted)
    while (bitboard) {
        // reset least significant bit
        bitboard &= bitboard - 1;
        count++;
    }
    return count;
}

// get index of least significant first bit index
static inline int ls1bIndex(U64 bitboard) {
    // ensure bitboard has bits set
    if (bitboard) {
        return countBits((bitboard & -bitboard) - 1);
    } else {
        // return illegal bit index
        return -1;
    }
}

// board squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

// sides to move
enum {
    white, black
};

// rook bishop enum
enum {
    rook, bishop
};

const char *squareToCoordinates[] = {
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

// prints bitboard representation
void printBitboard(U64 bitboard) {

    printf("\n");

    // loop over ranks
    for (int rank = 0; rank < 8; rank++) {
        // loop over files
        for (int file = 0; file < 8; file++) {
            // convert file and rank into square index
            int square = rank * 8 + file;

            // print ranks
            if (!file) {
                printf("  %d ", 8 - rank);
            }

            printf(" %d", get_bit(bitboard, square) ? 1 : 0);
        }
        // print newline every rank
        printf("\n");
    }

    // print board files
    printf("     a b c d e f g h \n\n");

    // print bitboard as unsigned decimal value
    printf("     Bitboard: %llud\n\n ", bitboard);
}

/*********************\
 ======================
        Attacks
 ======================
\*********************/

// not in 'a' file constant
const U64 not_a_file = 18374403900871474942ULL;
// not in 'h' file constant
const U64 not_h_file = 9187201950435737471ULL;
const U64 not_hg_file = 4557430888798830399ULL;
const U64 not_ab_file = 18229723555195321596ULL;

// pawn attack table
U64 pawnAttacks[2][64];

// knight attack table
U64 knightAttacks[64];

// king attack table
U64 kingAttacks[64];

// relevant occupancy bitcount for every square on board
const int bishopRelevantBits[64] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6,
};

const int rookRelevantBits[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12,
};

const U64 rookMagicNumbers[64] =
        {
                0x98060280800000ULL,
                0x200102080410000ULL,
                0x1084003808010000ULL,
                0x240040040880000ULL,
                0x400024020140206ULL,
                0x400a11002040000ULL,
                0x400100140844800ULL,
                0x2080003444810200ULL,
                0x3010200800000ULL,
                0x4006422210000ULL,
                0x944100402080000ULL,
                0x8104100012c0a000ULL,
                0x4800800400088000ULL,
                0x802200040000ULL,
                0x800108024800ULL,
                0x40200092004400ULL,
                0xe40840026010000ULL,
                0x10020c020100002ULL,
                0x2010040014ULL,
                0x10240014088036ULL,
                0x818080040b0000ULL,
                0x805000a010020000ULL,
                0x4250012041800400ULL,
                0x4001200800409200ULL,
                0x100010a08040000ULL,
                0x20601008003ULL,
                0x20000043a0008001ULL,
                0x8000030030082001ULL,
                0x2808083444ULL,
                0x10e701041201800ULL,
                0x4480000c02020000ULL,
                0x124010108000ULL,
                0x600108400410001ULL,
                0x6000043000200002ULL,
                0xa804850c04004002ULL,
                0x14008080000ULL,
                0x4000240004004001ULL,
                0x2004120008101000ULL,
                0x10000c4285003000ULL,
                0xc00000020100d000ULL,
                0x1c0040a021220006ULL,
                0x8000004020028011ULL,
                0x8060c20003ULL,
                0x8020008400440001ULL,
                0xa62040000ULL,
                0x1418000202060010ULL,
                0x2404000401010000ULL,
                0x42100820000ULL,
                0xc000004000800080ULL,
                0x100002000400106ULL,
                0x900002048a401200ULL,
                0x201001000082100ULL,
                0x40000042a010a00ULL,
                0x130000409508200ULL,
                0x800000111000408ULL,
                0x80c000891220040ULL,
                0x280048440122066ULL,
                0x600004010920104ULL,
                0x308001008020500cULL,
                0x820000425001001ULL,
                0x8200002c0201084ULL,
                0x802024004209040cULL,
                0x1060000801100204ULL,
                0x88102100488402ULL,
        };

const U64 bishopMagicNumbers[64] =
        {
                0x2020448008100ULL,
                0x1820843102002050ULL,
                0x8908108112005000ULL,
                0x4042088010220ULL,
                0x4124504000060141ULL,
                0x2001042240440002ULL,
                0x104008884100082ULL,
                0x61208020210d0ULL,
                0x1015060a1092212ULL,
                0x10b4080228004900ULL,
                0x8200040822004025ULL,
                0x2022082044800ULL,
                0xa002411140224800ULL,
                0x120084802500004ULL,
                0x412804606104280ULL,
                0xcab0088088080250ULL,
                0x480081010c202ULL,
                0x108803602244400ULL,
                0x20884a1003820010ULL,
                0x44044824001061ULL,
                0x2200400a000a0ULL,
                0x6000248020802ULL,
                0x181020200900400ULL,
                0x8802208200842404ULL,
                0x120226064040400ULL,
                0x4021004284808ULL,
                0x4001404094050200ULL,
                0x480a040008010820ULL,
                0x2194082044002002ULL,
                0x2008a20001004200ULL,
                0x40908041041004ULL,
                0x881002200540404ULL,
                0x4001082002082101ULL,
                0x8110408880880ULL,
                0x8000404040080200ULL,
                0x200020082180080ULL,
                0x1184440400114100ULL,
                0xc220008020110412ULL,
                0x4088084040090100ULL,
                0x8822104100121080ULL,
                0x100111884008200aULL,
                0x2844040288820200ULL,
                0x90901088003010ULL,
                0x1000a218000400ULL,
                0x1102010420204ULL,
                0x8414a3483000200ULL,
                0x6410849901420400ULL,
                0x201080200901040ULL,
                0x204880808050002ULL,
                0x1001008201210000ULL,
                0x16a6300a890040aULL,
                0x8049000441108600ULL,
                0x2212002060410044ULL,
                0x100086308020020ULL,
                0x484241408020421ULL,
                0x105084028429c085ULL,
                0x4282480801080cULL,
                0x81c098488088240ULL,
                0x1400000090480820ULL,
                0x4444000030208810ULL,
                0x1020142010820200ULL,
                0x2234802004018200ULL,
                0xc2040450820a00ULL,
                0x2101021090020ULL,
        };

// generate pawn attacks
U64 maskPawnAttacks(int sideToMove, int square) {
    //define piece bitboard
    U64 bitboard = 0ULL;
    // define result attacks bitboard
    U64 attacks = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // white pawns
    if (!sideToMove) {
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    }
        // black pawns
    else {
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    }

    // return attack map
    return attacks;
}



// generate knight attacks
U64 maskKnightAttacks(int square) {
    // init empty bitboard
    U64 bitboard = 0ULL;
    // init attack table
    U64 attacks = 0ULL;

    set_bit(bitboard, square);

    if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);

    if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);

    return attacks;
}

U64 maskKingAttacks(int square) {
    // init empty bitboard
    U64 bitboard = 0ULL;
    // init attack table
    U64 attacks = 0ULL;

    set_bit(bitboard, square);

    // shift by 1, 7, 8, 9 bits on both sides

    // attacks above and directly right
    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);
    if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
    if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);

    // attacks below and directly left
    if (bitboard << 8) attacks |= (bitboard << 8);
    if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);
    if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
    if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);


    return attacks;
}

// mask bishop attacks for magic bitboard
U64 maskBishopAttacks(int square) {
    // init empty bitboard
    U64 bitboard = 0ULL;

    // init attack table
    U64 attacks = 0ULL;

    // init ranks, files
    int rank, file;

    // init target ranks and files
    int targetRank = square / 8;
    int targetFile = square % 8;

    // mask relevant bishop occupancy bits
    for (rank = targetRank + 1, file = targetFile + 1; rank <= 6 && file <= 6; rank++, file++) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    for (rank = targetRank - 1, file = targetFile + 1; rank >= 1 && file <= 6; rank--, file++) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    for (rank = targetRank + 1, file = targetFile - 1; rank <= 6 && file >= 1; rank++, file--) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    for (rank = targetRank - 1, file = targetFile - 1; rank >= 1 && file >= 1; rank--, file--) {
        attacks |= (1ULL << (rank * 8 + file));
    }

    return attacks;
}

// mask rook attacks for magic bitboard
U64 maskRookAttacks(int square) {
    // init empty bitboard
    U64 bitboard = 0ULL;

    // init attack table
    U64 attacks = 0ULL;

    // init ranks, files
    int rank, file;

    // init target ranks and files
    int targetRank = square / 8;
    int targetFile = square % 8;

    // mask relevant rook occupancy bits
    for (rank = targetRank + 1; rank <= 6; rank++) {
        attacks |= (1ULL << (rank * 8 + targetFile));
    }
    for (rank = targetRank - 1; rank >= 1; rank--) {
        attacks |= (1ULL << (rank * 8 + targetFile));
    }
    for (file = targetFile + 1; file <= 6; file++) {
        attacks |= (1ULL << (targetRank * 8 + file));
    }
    for (file = targetFile - 1; file >= 1; file--) {
        attacks |= (1ULL << (targetRank * 8 + file));
    }

    return attacks;
}

// generate bishop attacks
U64 generateBishopAttacks(int square, U64 block) {
    // init empty bitboard
    U64 bitboard = 0ULL;

    // init attack table
    U64 attacks = 0ULL;

    // init ranks, files
    int rank, file;

    // init target ranks and files
    int targetRank = square / 8;
    int targetFile = square % 8;

    // mask relevant bishop occupancy bits
    for (rank = targetRank + 1, file = targetFile + 1; rank <= 7 && file <= 7; rank++, file++) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) { break; }
    }
    for (rank = targetRank - 1, file = targetFile + 1; rank >= 0 && file <= 7; rank--, file++) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) { break; }
    }
    for (rank = targetRank + 1, file = targetFile - 1; rank <= 7 && file >= 0; rank++, file--) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) { break; }
    }
    for (rank = targetRank - 1, file = targetFile - 1; rank >= 0 && file >= 0; rank--, file--) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) { break; }
    }

    return attacks;
}

// generate rook attacks on the fly
U64 generateRookAttacks(int square, U64 block) {
    // init empty bitboard
    U64 bitboard = 0ULL;

    // init attack table
    U64 attacks = 0ULL;

    // init ranks, files
    int rank, file;

    // init target ranks and files
    int targetRank = square / 8;
    int targetFile = square % 8;

    // mask relevant rook occupancy bits
    for (rank = targetRank + 1; rank <= 6; rank++) {
        attacks |= (1ULL << (rank * 8 + targetFile));
        if ((1ULL << (rank * 8 + targetFile)) & block) { break; }
    }
    for (rank = targetRank - 1; rank >= 1; rank--) {
        attacks |= (1ULL << (rank * 8 + targetFile));
        if ((1ULL << (rank * 8 + targetFile)) & block) { break; }
    }
    for (file = targetFile + 1; file <= 6; file++) {
        attacks |= (1ULL << (targetRank * 8 + file));
        if ((1ULL << (targetRank * 8 + file)) & block) { break; }
    }
    for (file = targetFile - 1; file >= 1; file--) {
        attacks |= (1ULL << (targetRank * 8 + file));
        if ((1ULL << (targetRank * 8 + file)) & block) { break; }
    }

    return attacks;
}

void init_leaper_attacks() {
    for (int square = 0; square < 64; square++) {
        pawnAttacks[white][square] = maskPawnAttacks(white, square);
        pawnAttacks[black][square] = maskPawnAttacks(black, square);

        knightAttacks[square] = maskKnightAttacks(square);
        kingAttacks[square] = maskKingAttacks(square);
    }
}

U64 setOccupancy(int index, int bitsInMask, U64 attackMask) {
    // init occupancy map
    U64 occupancy = 0ULL;

    // loop over range of bits in attack mask
    for (int i = 0; i < bitsInMask; i++) {
        // get LS1B index of attack mask
        int square = ls1bIndex(attackMask);

        // pop LS1B
        pop_bit(attackMask, square);

        // make sure occupancy is on board
        if (index & (1 << i)) {
            // populate occupancy map
            occupancy |= (1ULL << square);
        }
    }

    // return occupancy map
    return occupancy;
}
/*********************\
 ======================
  Random Number Logic
 ======================
\*********************/

// pseudo-random number randomState
unsigned int randomState = 1804289383;

// generate 32 bit pseudo legal numbers
unsigned int getRandomU32Number() {
    // get current randomState
    unsigned int num = randomState;

    // XOR shift 32 algorithm
    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;

    // update random number randomState
    randomState = num;

    // return random number
    return num;
}

U64 getRandomU64Numbers() {
    U64 n1, n2, n3, n4;

    n1 = (U64) (getRandomU32Number()) & 0xFFFF; // slice upper from MSB side
    n2 = (U64) (getRandomU32Number()) & 0xFFFF;
    n3 = (U64) (getRandomU32Number()) & 0xFFFF;
    n4 = (U64) (getRandomU32Number()) & 0xFFFF;

    // return random u64 number
    return (n1 | (n2 << 16) | (n3 << 32) | (n4 << 48));
}

/*********************\
 ======================
   Magic Number Logic
 ======================
\*********************/

// generate magic number candidate
U64 generateMagicNumberCandidate() {
    return getRandomU64Numbers() & getRandomU64Numbers() & getRandomU64Numbers();
}

U64 findMagicNumber(int square, int relevantBits, int bishop) {
    // init occupancies
    U64 occupancies[4096];

    // init attack tables
    U64 attacks[4096];

    // init used attacks
    U64 usedAttacks[4096];

    // init attack mask for current piece
    U64 attackMask = bishop ? maskBishopAttacks(square) : maskRookAttacks(square);

    // init occupancy indices
    int occupancyIndices = 1 << relevantBits;

    // iterate over occupancy indices
    for (int i = 0; i < occupancyIndices; i++) {
        // init occupancies
        occupancies[i] = setOccupancy(i, relevantBits, attackMask);

        // init attacks
        attacks[i] = bishop ? generateBishopAttacks(square, occupancies[i]) : generateRookAttacks(square,
                                                                                                  occupancies[i]);
    }

    // test magic numbers
    for (int randomCount = 0; randomCount < 1e8; randomCount++) {
        // generate magic number candidate
        U64 magicNumber = generateMagicNumberCandidate();

        // skip inappropriate magic nums
        if (countBits(((attackMask) * magicNumber) & 0xFF00000000000000) < 6) {
            continue;
        }

        // init usedAttacks array
        memset(usedAttacks, 0ULL, sizeof(usedAttacks));

        // init index and fail flag
        int index, fail;

        // test magic index
        for (index = 0, fail = 0; !fail && index < occupancyIndices; index++) {
            int magicIndex = (int) (occupancies[index] * magicNumber >> (64 - relevantBits));

            // if magic index works
            if (usedAttacks[magicIndex] == 0ULL) {
                // init usedAttacks
                usedAttacks[magicIndex] = attacks[index];
            }
                // magic index doesn't work
            else if (usedAttacks[magicIndex] != attacks[index]) {
                fail = 1;
            }
        }

        // if magic number works return it
        if (!fail) {
            return magicNumber;
        }

    }
    // if magic number doesn't work
    printf("magic number fails");
    return 0ULL;
}

void initMagicNumbers() {
    // loop for 64 board squares
    for (int square = 0; square < 64; square++) {
        //init rook magic numbers
        printf(" 0x%llxULL,\n", findMagicNumber(square, rookRelevantBits[square], rook));
    }
    printf("\n-------------------------------\n");
    for (int square = 0; square < 64; square++) {
        //init bishop magic numbers
        printf(" 0x%llxULL,\n", findMagicNumber(square, bishopRelevantBits[square], bishop));
    }
}

/*********************\
 ======================
      Main Driver
 ======================
\*********************/

int main() {
    // init all variables
    init_leaper_attacks();

    // mask piece attacks at given square
    U64 attackMask = maskRookAttacks(a1);

    // init magic numbers routine
    initMagicNumbers();

    return 0;
}
