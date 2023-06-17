#include "cube.h"

using namespace tgx;


uint8_t cubeBuffer[6][9] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  // U
        {1, 1, 1, 1, 1, 1, 1, 1, 1},  // R
        {2, 2, 2, 2, 2, 2, 2, 2, 2},  // F
        {3, 3, 3, 3, 3, 3, 3, 3, 3},  // D
        {4, 4, 4, 4, 4, 4, 4, 4, 4},  // L
        {5, 5, 5, 5, 5, 5, 5, 5, 5}   // B
};

const uint8_t SideFaceletRotate[6][12] = {
        {47, 46, 45, 11, 10, 9,  20, 19, 18, 38, 37, 36},     // U
        {35, 32, 29, 26, 23, 20, 8,  5,  2,  45, 48, 51},     // R
        {44, 41, 38, 6,  7,  8,  9,  12, 15, 29, 28, 27},     // F
        {42, 43, 44, 24, 25, 26, 15, 16, 17, 51, 52, 53},     // D
        {0,  3,  6,  18, 21, 24, 27, 30, 33, 53, 50, 47},     // L
        {2,  1,  0,  36, 39, 42, 33, 34, 35, 17, 14, 11}      // B
};


const uint8_t cubeFacelets[6][9] = {
        {8,  17, 26, 5,  14, 23, 2,  11, 20},
        {20, 23, 26, 19, 22, 25, 18, 21, 24},
        {2,  11, 20, 1,  10, 19, 0,  9,  18},
        {0,  9,  18, 3,  12, 21, 6,  15, 24},
        {8,  5,  2,  7,  4,  1,  6,  3,  0},
        {26, 17, 8,  25, 16, 7,  24, 15, 6}
};


const RGB565 colors[6] = {
        RGB565_Yellow, RGB565_Green, RGB565_Red, RGB565_White, RGB565_Blue, RGB565_Orange
};


const fVec3 cubeletPosition[3][3][3] = {
        {
                {
                        {-1, -1, -1}, {-1, -1, 0}, {-1, -1, 1}
                },
                {
                        {-1, 0, -1}, {-1, 0, 0}, {-1, 0, 1}
                },
                {
                        {-1, 1, -1}, {-1, 1, 0}, {-1, 1, 1}
                }
        },
        {
                {
                        {0,  -1, -1}, {0,  -1, 0}, {0,  -1, 1}
                },
                {
                        {0,  0, -1}, {0,  0, 0}, {0,  0, 1}
                },
                {
                        {0,  1, -1}, {0,  1, 0}, {0,  1, 1}
                }
        },
        {
                {
                        {1,  -1, -1}, {1,  -1, 0}, {1,  -1, 1}
                },
                {
                        {1,  0, -1}, {1,  0, 0}, {1,  0, 1}
                },
                {
                        {1,  1, -1}, {1,  1, 0}, {1,  1, 1}
                }
        }
};


const tgx::fVec3 GENERAL_CUBE_POINT[8] =
        {
                {-1, 1,  1},
                {-1, -1, 1},
                {1,  -1, 1},
                {1,  1,  1},
                {1,  1,  -1},
                {1,  -1, -1},
                {-1, -1, -1},
                {-1, 1,  -1}
        };

const uint16_t GENERAL_CUBE_FACES[6][4] =
        {
                {0, 1, 2, 3},
                {3, 2, 5, 4},
                {1, 6, 5, 2},
                {4, 5, 6, 7},
                {7, 6, 1, 0},
                {7, 0, 3, 4},
        };


void rotateCube(uint8_t cube[6][9], uint8_t dir, bool clockwise) {
    auto *const cube_flat = (uint8_t *) cube;
    uint8_t *const cube_main = cube[dir];
    const uint8_t *sideFaceletRotate = SideFaceletRotate[dir];

    if (clockwise) {
        for (int offset = 0; offset < 3; ++offset) {
            auto temp = cube_flat[sideFaceletRotate[0 + offset]];
            cube_flat[sideFaceletRotate[0 + offset]] = cube_flat[sideFaceletRotate[9 + offset]];
            cube_flat[sideFaceletRotate[9 + offset]] = cube_flat[sideFaceletRotate[6 + offset]];
            cube_flat[sideFaceletRotate[6 + offset]] = cube_flat[sideFaceletRotate[3 + offset]];
            cube_flat[sideFaceletRotate[3 + offset]] = temp;
        }
    } else {
        for (int offset = 0; offset < 3; ++offset) {
            auto temp = cube_flat[sideFaceletRotate[0 + offset]];
            cube_flat[sideFaceletRotate[0 + offset]] = cube_flat[sideFaceletRotate[3 + offset]];
            cube_flat[sideFaceletRotate[3 + offset]] = cube_flat[sideFaceletRotate[6 + offset]];
            cube_flat[sideFaceletRotate[6 + offset]] = cube_flat[sideFaceletRotate[9 + offset]];
            cube_flat[sideFaceletRotate[9 + offset]] = temp;
        }
    }


    if (clockwise) {
        auto temp = cube_main[0];
        cube_main[0] = cube_main[6];
        cube_main[6] = cube_main[8];
        cube_main[8] = cube_main[2];
        cube_main[2] = temp;

        temp = cube_main[1];
        cube_main[1] = cube_main[3];
        cube_main[3] = cube_main[7];
        cube_main[7] = cube_main[5];
        cube_main[5] = temp;
    } else {
        auto temp = cube_main[0];
        cube_main[0] = cube_main[2];
        cube_main[2] = cube_main[8];
        cube_main[8] = cube_main[6];
        cube_main[6] = temp;


        temp = cube_main[1];
        cube_main[1] = cube_main[5];
        cube_main[5] = cube_main[7];
        cube_main[7] = cube_main[3];
        cube_main[3] = temp;
    }
}