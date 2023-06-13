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
#include <esp_heap_caps.h>
#include <esp_timer.h>
#include <esp_log.h>
#include "config.h"

// the tgx library.
#include "tgx.h"

// the mesh we will draw.
#include "buddha.h"
#include "naruto.h"

using namespace tgx;

const int SLX = SCREEN_WIDTH; // image dimension
const int SLY = SCREEN_HEIGHT; //

// the framebuffer we draw onto
EXT_RAM_BSS_ATTR uint16_t fb[2][SCREEN_HEIGHT][SCREEN_WIDTH];
bool screenIndex = true;


// the z-buffer in 16 bits precision
uint16_t *zbuf;

// the image that encapsulate framebuffer fb
Image<RGB565> imageBuffer[2] = {
        Image<RGB565>(fb[0], SLX, SLY),
        Image<RGB565>(fb[1], SLX, SLY)
};

Image<RGB565> *getImageBuffer() {
    return &imageBuffer[screenIndex];
}

const Shader LOADED_SHADERS =
        SHADER_PERSPECTIVE | SHADER_ZBUFFER | SHADER_FLAT | SHADER_GOURAUD | SHADER_NOTEXTURE | SHADER_TEXTURE_NEAREST |
        SHADER_TEXTURE_WRAP_POW2;
Renderer3D<RGB565, LOADED_SHADERS, uint16_t> renderer;


extern "C" void tgx_setup() {
    zbuf = (uint16_t *) malloc(SLX * SLY * sizeof(uint16_t));

    renderer.setViewportSize(SLX, SLY);
    renderer.setOffset(0, 0);
    renderer.setImage(getImageBuffer()); // set the image to draw onto (ie the screen framebuffer)
    renderer.setZbuffer(zbuf); // set the z buffer for depth testing
    renderer.setPerspective(45, ((float) SLX) / SLY, 1.0f, 100.0f);  // set the perspective projection matrix.
    renderer.setMaterial(RGBf(0.85f, 0.55f, 0.25f), 0.2f, 0.7f, 0.8f,
                         64); // bronze color with a lot of specular reflexion.
    renderer.setCulling(1);
    renderer.setTextureQuality(SHADER_TEXTURE_NEAREST);
    renderer.setTextureWrappingMode(SHADER_TEXTURE_WRAP_POW2);

}

/** Compute the model matrix according to the current time */
tgx::fMat4 moveModel(int &loopnumber) {
    const float end1 = 6000;
    const float end2 = 2000;
    const float end3 = 6000;
    const float end4 = 2000;

    int tot = (int) (end1 + end2 + end3 + end4);
    int m = esp_timer_get_time() / 1000;

    loopnumber = m / tot;
    float t = m % tot;

    const float dilat = 9; // scale model
    const float roty = 360 * (t / 4000); // rotate 1 turn every 4 seconds
    float tz, ty;
    if (t < end1) { // far away
        tz = -25;
        ty = 0;
    } else {
        t -= end1;
        if (t < end2) { // zooming in
            t /= end2;
            tz = -25 + 18 * t;
            ty = -6.5f * t;
        } else {
            t -= end2;
            if (t < end3) { // close up
                tz = -7;
                ty = -6.5f;
            } else { // zooming out
                t -= end3;
                t /= end4;
                tz = -7 - 18 * t;
                ty = -6.5 + 6.5 * t;
            }
        }
    }

    fMat4 M;
    M.setScale({dilat, dilat, dilat}); // scale the model
    M.multRotate(-roty, {0, 1, 0}); // rotate around y
    M.multTranslate({0, ty, tz}); // translate
    return M;
}


int loopnumber = 0;
int prev_loopnumber = -1;

extern "C" void tgx_next() {
    // compute the model position
    fMat4 M = moveModel(loopnumber);
    renderer.setModelMatrix(M);

    // draw the 3D mesh
    getImageBuffer()->fillScreen(RGB565_Cyan);              // clear the framebuffer (black background)
    renderer.clearZbuffer();                    // clear the z-buffer

    // choose the shader to use
    switch (loopnumber % 4) {
        case 0:
            renderer.setShaders(SHADER_GOURAUD | SHADER_TEXTURE);
            renderer.drawMesh(&naruto_1, false);
            break;

        case 1:
            renderer.drawWireFrameMesh(&naruto_1, true);
            break;

        case 2:
            renderer.setShaders(SHADER_FLAT);
            renderer.drawMesh(&naruto_1, false);
            break;

        case 3:
            renderer.setShaders(SHADER_GOURAUD);
            renderer.drawMesh(&naruto_1, false);
            break;
    }

    if (prev_loopnumber != loopnumber) {
        prev_loopnumber = loopnumber;
        switch (loopnumber % 4) {
            case 0:
                ESP_LOGI("?", "Gouraud shading / texturing");
                break;
            case 1:
                ESP_LOGI("?", "Wireframe");
                break;
            case 2:
                ESP_LOGI("?", "Flat Shading");
                break;
            case 3:
                ESP_LOGI("?", "Gouraud shading");
                break;
        }
    }
    screenIndex = !screenIndex;
    renderer.setImage(getImageBuffer()); // set the image to draw onto (ie the screen framebuffer)
}

extern "C" uint16_t *getFrame() {
    return (uint16_t *) fb[!screenIndex];
}


/** en of file */
