#include <stdio.h>
#include "cstdint"

/**
 * 测试魔方旋转换面函数
 */

const uint8_t DirectionOrder[6] = {0, 1, 2, 3, 4, 5};
const uint8_t SideFaceletRotate[6][12] = {
        {47, 46, 45, 11, 10, 9,  20, 19, 18, 38, 37, 36},   // U
        {35, 32, 29, 26, 23, 20, 8,  5,  2,  45, 48, 51},    // R
        {44, 41, 38, 6,  7,  8,  9,  12, 15, 29, 28, 27},      // F
        {42, 43, 44, 24, 25, 26, 15, 16, 17, 51, 52, 53},     // D
        {0,  3,  6,  18, 21, 24, 27, 30, 33, 53, 50, 47},     // L
        {2,  1,  0,  36, 39, 42, 33, 34, 35, 17, 14, 11}      // B
};

const uint8_t MainFaceletRotate[8] = {1, 2, 3, 6, 9, 8, 7, 4};


void RotateCube(uint8_t cube[6][9], uint8_t dir, bool clockwise) {
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
        for (int offset = 0; offset < 2; ++offset) {
            auto temp = cube_main[0 + offset];
            cube_main[0 + offset] = cube_main[6 + offset];
            cube_main[6 + offset] = cube_main[4 + offset];
            cube_main[4 + offset] = cube_main[2 + offset];
            cube_main[2 + offset] = temp;
        }
    } else {
        for (int offset = 0; offset < 2; ++offset) {
            auto temp = cube_main[0 + offset];
            cube_main[0 + offset] = cube_main[2 + offset];
            cube_main[2 + offset] = cube_main[4 + offset];
            cube_main[4 + offset] = cube_main[6 + offset];
            cube_main[6 + offset] = temp;
        }
    }
}

void printCube(uint8_t cube[54]) {
    // 打印魔方状态
    for (int i = 0; i < 54; i++) {
        printf("%2d ", cube[i]);
        if ((i + 1) % 9 == 0) {
            printf("\n");
        }
    }
}

int main() {
    uint8_t cube[6][9] = {
            {0, 0, 0, 0, 0, 0, 0, 0, 0},  // U
            {1, 1, 1, 1, 1, 1, 1, 1, 1},  // R
            {2, 2, 2, 2, 2, 2, 2, 2, 2},  // F
            {3, 3, 3, 3, 3, 3, 3, 3, 3},  // D
            {4, 4, 4, 4, 4, 4, 4, 4, 4},  // L
            {5, 5, 5, 5, 5, 5, 5, 5, 5}   // B
    };

    printf("初始状态:\n");
    printCube((uint8_t *) (cube));

    bool flag = true;
    for (int i = 0; i < 12; i++) {
        RotateCube(cube, i / 2, flag);
        flag = !flag;
    }


    printf("\n旋转后的状态:\n");
    printCube((uint8_t *) (cube));

    return 0;
}

