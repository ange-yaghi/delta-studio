#include "../include/font_map.h"

int dbasic::FONT_MAP[256];
const int dbasic::BOLD_OFFSET = 92;

void dbasic::InitializeFontMap() {
    for (int i = 0; i < 256; i++) {
        FONT_MAP[i] = 185;
    }

    FONT_MAP['A'] = 0;
    FONT_MAP['B'] = 1;
    FONT_MAP['C'] = 2;
    FONT_MAP['D'] = 3;
    FONT_MAP['E'] = 4;
    FONT_MAP['F'] = 5;
    FONT_MAP['G'] = 6;
    FONT_MAP['H'] = 7;
    FONT_MAP['I'] = 8;
    FONT_MAP['J'] = 9;
    FONT_MAP['K'] = 10;
    FONT_MAP['L'] = 11;
    FONT_MAP['M'] = 12;
    FONT_MAP['N'] = 13;
    FONT_MAP['O'] = 14;
    FONT_MAP['P'] = 15;
    FONT_MAP['Q'] = 16;
    FONT_MAP['R'] = 17;
    FONT_MAP['S'] = 18;
    FONT_MAP['T'] = 19;
    FONT_MAP['U'] = 20;
    FONT_MAP['V'] = 21;
    FONT_MAP['W'] = 22;
    FONT_MAP['X'] = 23;
    FONT_MAP['Y'] = 24;
    FONT_MAP['Z'] = 25;

    FONT_MAP['a'] = 26;
    FONT_MAP['b'] = 27;
    FONT_MAP['c'] = 28;
    FONT_MAP['d'] = 29;
    FONT_MAP['e'] = 30;
    FONT_MAP['f'] = 31;
    FONT_MAP['g'] = 32;
    FONT_MAP['h'] = 33;
    FONT_MAP['i'] = 34;
    FONT_MAP['j'] = 35;
    FONT_MAP['k'] = 36;
    FONT_MAP['l'] = 37;
    FONT_MAP['m'] = 38;
    FONT_MAP['n'] = 39;
    FONT_MAP['o'] = 40;
    FONT_MAP['p'] = 41;
    FONT_MAP['q'] = 42;
    FONT_MAP['r'] = 43;
    FONT_MAP['s'] = 44;
    FONT_MAP['t'] = 45;
    FONT_MAP['u'] = 46;
    FONT_MAP['v'] = 47;
    FONT_MAP['w'] = 48;
    FONT_MAP['x'] = 49;
    FONT_MAP['y'] = 50;
    FONT_MAP['z'] = 51;

    FONT_MAP['1'] = 52;
    FONT_MAP['2'] = 53;
    FONT_MAP['3'] = 54;
    FONT_MAP['4'] = 55;
    FONT_MAP['5'] = 56;
    FONT_MAP['6'] = 57;
    FONT_MAP['7'] = 58;
    FONT_MAP['8'] = 59;
    FONT_MAP['9'] = 60;
    FONT_MAP['0'] = 61;

    FONT_MAP['!'] = 62;
    FONT_MAP['@'] = 63;
    FONT_MAP['#'] = 64;
    FONT_MAP['$'] = 65;
    FONT_MAP['%'] = 66;
    FONT_MAP['^'] = 67;
    FONT_MAP['&'] = 68;
    FONT_MAP['*'] = 69;
    FONT_MAP['('] = 70;
    FONT_MAP[')'] = 71;

    FONT_MAP['-'] = 72;
    FONT_MAP['_'] = 73;
    FONT_MAP['='] = 74;
    FONT_MAP['+'] = 75;
    FONT_MAP['{'] = 76;
    FONT_MAP['}'] = 77;
    FONT_MAP['['] = 78;
    FONT_MAP[']'] = 79;
    FONT_MAP['|'] = 80;
    FONT_MAP['\\'] = 81;

    FONT_MAP[','] = 82;
    FONT_MAP['.'] = 83;
    FONT_MAP['<'] = 84;
    FONT_MAP['>'] = 85;
    FONT_MAP['/'] = 86;
    FONT_MAP['?'] = 87;
    FONT_MAP[';'] = 88;
    FONT_MAP[':'] = 89;
    FONT_MAP['"'] = 90;
    FONT_MAP['\''] = 91;

    FONT_MAP[179] = 207; ///
    FONT_MAP[180] = 213; ///
    FONT_MAP[195] = 208; ///
    FONT_MAP[197] = 209; ///
    FONT_MAP[196] = 212; ///
    FONT_MAP[217] = 232; //
    FONT_MAP[191] = 186; //
    FONT_MAP[218] = 185; //
    FONT_MAP[192] = 231; //
    FONT_MAP[193] = 210; //
    FONT_MAP[194] = 211; //

    FONT_MAP[' '] = 188;
}
