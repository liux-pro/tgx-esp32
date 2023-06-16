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
#define PI		3.14159265358979323846f

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


int8_t fuck = 3;
int16_t degrees = 0;


void drawQuad(const fMat4 &M, uint8_t direction) {
    renderer.setModelMatrix(M);

    renderer.drawQuad(GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[direction * 4 + 0]],
                      GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[direction * 4 + 1]],
                      GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[direction * 4 + 2]],
                      GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[direction * 4 + 3]]);
}

extern "C" void tgx_next() {
    static uint16_t r = 0;
    getLocalImageBuffer()->clear(tgx::RGB565_Gray);  // clear the image
    renderer.clearZbuffer(); // and the zbuffer.
    r++;

    if (r == 360) {
        r = 0;
    }
    degrees++;
    if (degrees == 360) {
        degrees = 0;
        fuck++;
        fuck=fuck%6;
    }



    float radians= PI * (r/180.0f);
    renderer.setLookAt({8*cos(radians), 8*sin(radians), 8}, {0, 0, 0}, {0, 0, 1});
//    renderer.setLookAt({5, 5, -5}, {0, 0, 0}, {0, 0, 1});


    for (int i = 0; i < 6; ++i) { //分面绘制，按顺序绘制6个面
        for (int j = 0; j < 9; ++j) { //绘制每个大面的9个小facelet
            const fVec3 &position = ((fVec3 *) cubeletPosition)[cubeFacelets[i][j]];
            renderer.setMaterial(RGBf(((RGB565 *) colors)[i]), 1, 0, 0, 0); // set material properties
            //cubelet的位置
            fMat4 M;  //变换矩阵
            M.setScale({0.5, 0.5, 0.5}); //缩小
            M.multTranslate({position.x, position.y, position.z});  //平移到自己该在的位置
            //下面当前的facelet是否需要旋转。
            if (fuck > -1) { //魔方不被拧的时候就是-1,0到5对应是哪个面要被拧。
                //所在平面方程的系数，y=ax+by+cz,把当前facelet所在的cubelet的位置带入方程，如果y大于0就代表当前位置的cubelet属于需要旋转的层
                //相当于一个平面分割了27块cubelet为需要旋转的层和无需旋转的另外两层
                //需要旋转的面的中心cubelet的位置坐标，恰好和这个分割面的表示方程的系数对应。
                const fVec3 &k = ((fVec3 *) cubeletPosition)[cubeFacelets[fuck][4]];
                float y = k.x * position.x + k.y * position.y + k.z * position.z;
                if (y > 0) { //如果y大于0就代表当前位置的cubelet属于需要旋转的层
                    //以当前层的法线方向旋转，这层9个cubelet，中间的那个的位置如果看做一个方向向量，正好是法线方向，把他当成旋转轴。
                    const fVec3 &rotateDirection = ((fVec3 *) cubeletPosition)[cubeFacelets[fuck][4]];
                    M.multRotate(degrees, rotateDirection);
                }
            }
            drawQuad(M, i);  //绘制facelet
            /*--------------------------------------------------------------------------------------------------------------------*/
            // 上面的绘制只是绘制了魔方的外皮，内部其实是空心的。
            // 旋转以后，就会露出没有绘制的内部面，也就是时现实世界把魔方旋转45度，没贴贴纸的，塑料的那部分。
            // 不能让他镂空，所以要在上面渲染上黑的的面，为了好看。
            // 一共需要绘制16个面，也就是旋转层的8个cubelet沿旋转法线反方向的8个面以及
            // 和旋转层挨着的那个中间层沿旋转法线方向的8个面。
            renderer.setMaterial(RGBf(RGB565_Purple), 1, 0, 0, 0); // set material properties
            if (i == fuck && j != 4) {
                drawQuad(M, (i + 3) % 6);//对于前8个，就是当前facelet的反方向就行了。
                {// 后8个面  后八个面是不旋转的
                    const fVec3 &offset = ((fVec3 *) cubeletPosition)[cubeFacelets[fuck][4]];
                    const fVec3 &midLayerPosition = position - offset; //当前cubelet沿旋转轴法线反方向的那个cubelet的位置
                    M.setZero();
                    M.setScale({0.5, 0.5, 0.5}); //缩小
                    M.multTranslate({midLayerPosition.x, midLayerPosition.y, midLayerPosition.z});  //平移到自己该在的位置
                    drawQuad(M, i);
                }
            }


        }
    }


    screenIndex = !screenIndex;
    renderer.setImage(getLocalImageBuffer());
}

extern "C" uint16_t *getFrame() {
    return (uint16_t *) fb[!screenIndex];
}


/** en of file */
