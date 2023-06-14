/********************************************************************************
 * @file buddhaOnCPU.cpp
 *
 * Example: using the TGX to draw a 3D mesh (displayed using the CImg library).
 *
 * For Windows/Linuc/MacOS.
 *
 * Building the example:
 *
 * 1. Install CMake (version 3.10 later).
 *
 * 2. Open a shell/terminal inside the directory that contains this file.
 *
 * 3. Install CImg (only for Linux/MacOS)
 *    - If on Linux/Debian, run: "sudo apt install cimg-dev"
 *    - If on MacOS/homebrew, run: "brew install cimg"
 *
 * 4. run the following commands:
 *       mkdir build
 *       cd build
 *       cmake ..
 *
 * 4. This will create the project files into the /build directory which can now
 *    be used to build the example. .
 *
 *    -> On Windows. Open the Visual Studio solution file "buddhaOnCPU.sln" and
 *       build the example from within the IDE.
 *
 *    -> On Linux/MacOS. Run "make" to build the example.
 *
 *******************************************************************************/
#include <esp_attr.h>
#include <esp_log.h>
#include "config.h"

#include "tgx.h"
#include "logo_texture.h"

using namespace tgx;

const int SLX = SCREEN_WIDTH; // image dimension
const int SLY = SCREEN_HEIGHT; //

// the framebuffer we draw onto
EXT_RAM_BSS_ATTR uint16_t fb[2][SCREEN_HEIGHT][SCREEN_WIDTH];
bool screenIndex = true;


// the z-buffer in 16 bits precision
EXT_RAM_BSS_ATTR float zbuf[SLX * SLY];

// the image that encapsulate framebuffer fb
Image<RGB565> imageBuffer[2] = {
        Image<RGB565>(fb[0], SLX, SLY),
        Image<RGB565>(fb[1], SLX, SLY)
};

Image<RGB565> *getImageBuffer() {
    return &imageBuffer[screenIndex];
}

Renderer3D<RGB565, TGX_SHADER_MASK_ALL, float> renderer;

extern "C" void tgx_setup() {
    renderer.setViewportSize(SLX, SLY);
    renderer.setOffset(0, 0);
    renderer.setImage(getImageBuffer()); // set the image to draw onto (ie the screen framebuffer)
    renderer.setZbuffer(zbuf); // set the z buffer for depth testing
    renderer.setPerspective(45, ((float) SLX) / SLY, 1.0f, 100.0f);  // set the perspective projection matrix.
    renderer.setCulling(1);
    renderer.setTextureQuality(SHADER_TEXTURE_NEAREST);
    renderer.setTextureWrappingMode(SHADER_TEXTURE_WRAP_POW2);
    renderer.setShaders(SHADER_TEXTURE);


}

void drawCube(float x, float y, float z, RGB565 c) {
    renderer.setMaterial(tgx::RGBf(c), 1, 0, 0, 0); // set material properties
//    renderer.setMaterialColor(tgx::RGBf(1, 0, 0)); // set material properties
    renderer.setModelPosScaleRot({x, y, z}, {5, 5, 5}, 0); // set the position of the mesh


    float epsx = 0, epsy = 0;


//    for (int i = 0; i < 6; ++i) {
//        renderer.drawQuad(UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 0]],
//                          UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 1]],
//                          UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 2]],
//                          UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 3]]);
//    }

    if (x+y+z>25){
        //    renderer.drawMesh(&cube, true);
        for (int i = 0; i < 6; ++i) {
            epsx = fast_inv((float) (logo_texture.lx() - 1));
            epsy = fast_inv((float) (logo_texture.ly() - 1));

            tgx::fVec2 t_front[4] = {tgx::fVec2(epsx, epsy), tgx::fVec2(epsx, 1 - epsy), tgx::fVec2(1 - epsx, 1 - epsy),
                                     tgx::fVec2(1 - epsx, epsy)};
            renderer.drawQuad(UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 0]],
                              UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 1]],
                              UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 2]],
                              UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 3]], nullptr, nullptr, nullptr, nullptr,
                              &t_front[0], &t_front[1], &t_front[2], &t_front[3], &logo_texture);
        }
    } else{
        //    renderer.drawMesh(&cube, true);
        for (int i = 0; i < 6; ++i) {
            renderer.drawQuad(UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 0]],
                              UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 1]],
                              UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 2]],
                              UNIT_CUBE_VERTICES[UNIT_CUBE_FACES[i * 4 + 3]]);
        }
    }

}

RGB565 colors[3][3][3] = {
        {
                {
                        RGB565_Black,  RGB565_White,  RGB565_Red
                },
                {
                        RGB565_Blue,    RGB565_Green, RGB565_Purple
                },
                {
                        RGB565_Orange, RGB565_Cyan,   RGB565_Lime
                }
        },
        {
                {
                        RGB565_Salmon, RGB565_Maroon, RGB565_Yellow
                },
                {
                        RGB565_Magenta, RGB565_Olive, RGB565_Teal
                },
                {
                        RGB565_Gray,   RGB565_Silver, RGB565_Navy
                }
        },
        {
                {
                        RGB565_Black,  RGB565_White,  RGB565_Red
                },
                {
                        RGB565_Blue,    RGB565_Green, RGB565_Purple
                },
                {
                        RGB565_Orange, RGB565_Cyan,   RGB565_Lime
                }
        }
};


extern "C" void tgx_next() {
    static uint32_t r = 0;
    getImageBuffer()->clear(tgx::RGB565_Gray);  // clear the image
    renderer.clearZbuffer(); // and the zbuffer.
    r++;

    renderer.setLookAt({70 * cosf(r * 0.08f), 70 * sinf(r * 0.08), 50}, {0, 0, 0}, {0, 0, 1});



    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                if (i==0 && j==0 && k==0) continue;
                drawCube(i * 10, j * 10, k * 10, colors[i + 1][j + 1][k + 1]);
            }
        }
    }
    screenIndex = !screenIndex;
    renderer.setImage(getImageBuffer()); // set the image to draw onto (ie the screen framebuffer)
}

extern "C" uint16_t *getFrame() {
    return (uint16_t *) fb[!screenIndex];
}


/** en of file */
