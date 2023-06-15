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
#include "cube.h"
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

Image<RGB565> *getLocalImageBuffer() {
    return &imageBuffer[screenIndex];
}

Renderer3D<RGB565, TGX_SHADER_MASK_ALL, float> renderer;

extern "C" void tgx_setup() {
    renderer.setViewportSize(SLX, SLY);
    renderer.setOffset(0, 0);
    renderer.setImage(getLocalImageBuffer()); // set the image to draw onto (ie the screen framebuffer)
    renderer.setZbuffer(zbuf); // set the z buffer for depth testing
    renderer.setPerspective(45, ((float) SLX) / SLY, 1.0f, 100.0f);  // set the perspective projection matrix.
    renderer.setCulling(1);
    renderer.setTextureQuality(SHADER_TEXTURE_NEAREST);
    renderer.setTextureWrappingMode(SHADER_TEXTURE_WRAP_POW2);
    renderer.setShaders(SHADER_TEXTURE);


}


extern "C" void tgx_next() {
    static uint32_t r = 0;
    getLocalImageBuffer()->clear(tgx::RGB565_Gray);  // clear the image
    renderer.clearZbuffer(); // and the zbuffer.
    r++;

    renderer.setLookAt({15 * cosf(r * 0.08f), 15 * sinf(r * 0.08), 15}, {0, 0, 0}, {0, 0, 1});


    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 9; ++j) {
            renderer.setMaterial(RGBf(((RGB565 *) colors)[i]), 1, 0, 0, 0); // set material properties
            fVec3 pVec3 = ((fVec3 *) cubeletPosition)[cubeFacelets[i][j]];

            renderer.setModelPosScaleRot({pVec3.x * 2, pVec3.y * 2, pVec3.z * 2}, {1, 1, 1},
                                         0); // set the position of the mesh

            renderer.drawQuad(GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[i * 4 + 0]],
                              GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[i * 4 + 1]],
                              GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[i * 4 + 2]],
                              GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[i * 4 + 3]]);
        }
    }


    screenIndex = !screenIndex;
    renderer.setImage(getLocalImageBuffer());
}

extern "C" uint16_t *getFrame() {
    return (uint16_t *) fb[!screenIndex];
}


/** en of file */
