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

#define PI        3.14159265358979323846f

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

/**
 * 获取当前的缓冲区,他会自动处理双缓冲切换
 * @return
 */
static Image<RGB565> *getLocalImageBuffer() {
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
//    for (int i = 0; i < 6; ++i) {
//        rotateCube(cubeBuffer, i, true);
//    }
//    for (int i = 0; i < 6; ++i) {
//        rotateCube(cubeBuffer, i, false);
//    }
    startRotateCube(1, false,15);
}


/**
 * 给出描述位置平移旋转等信息的变换矩阵
 * 按 @dir 描述的方向,绘制一个正方形
 * @param M 变换矩阵
 * @param dir 绘制方向,或者说绘制立方体的哪个面
 */
static inline void drawQuad(const fMat4 &M, uint8_t dir) {
    renderer.setModelMatrix(M);
    renderer.drawQuad(GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[dir][0]],
                      GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[dir][1]],
                      GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[dir][2]],
                      GENERAL_CUBE_POINT[GENERAL_CUBE_FACES[dir][3]]);
}

bool animationBusy = false;

uint8_t animationFrameCountMax = 0;
uint8_t animationFrameCountLeft = 0;
uint8_t animationFrameDirection = 0;
bool animationFrameClockwise;


bool isAnimationBusy() {
    return animationBusy;
}

void setAnimationBusy(bool busy) {
    animationBusy = busy;
}

bool startRotateCube(uint8_t dir, bool clockwise, uint8_t frameCount) {
    if (isAnimationBusy()) {
        return false;
    }
    setAnimationBusy(true);
    animationFrameDirection = dir;
    animationFrameCountMax = frameCount;
    animationFrameCountLeft = frameCount;
    animationFrameClockwise = clockwise;
    return true;
}

float easeInOutCubic(float x) {
    return x < 0.5f ? 4.0f * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}
extern "C" void tgx_next() {
    static uint16_t cameraRotate = 0;
    getLocalImageBuffer()->clear(tgx::RGB565_Gray);  // clear the image
    renderer.clearZbuffer(); // and the zbuffer.
    cameraRotate++;

    if (cameraRotate == 360) {
        cameraRotate = 0;
    }


    float angle = 0;
    if (isAnimationBusy()) {
        if (animationFrameCountLeft==0){
            rotateCube(cubeBuffer,animationFrameDirection,animationFrameClockwise);
            setAnimationBusy(false);
            startRotateCube(rand() % 6,rand() % 2,15);
        } else{
            float dir_f = animationFrameClockwise ? -1.0f : 1.0f;
            const auto percent = static_cast<float>((animationFrameCountMax - animationFrameCountLeft)) /
                                    static_cast<float>(animationFrameCountMax);
            angle = dir_f * 90.0f * easeInOutCubic(percent);
            animationFrameCountLeft--;
        }
    }


    float radians = PI *(cameraRotate / 180.0f);
    renderer.setLookAt({8 * cos(radians), 8 * sin(radians), 8}, {0, 0, 0}, {0, 0, 1});
//    renderer.setLookAt({5, 5, -5}, {0, 0, 0}, {0, 0, 1});


    for (int i = 0; i < 6; ++i) { //分面绘制，按顺序绘制6个面
        for (int j = 0; j < 9; ++j) { //绘制每个大面的9个小facelet
            const fVec3 &position = ((fVec3 *) cubeletPosition)[cubeFacelets[i][j]];
            renderer.setMaterial(RGBf(colors[cubeBuffer[i][j]]), 1, 0, 0, 0); // set material properties
            //cubelet的位置
            fMat4 M;  //变换矩阵
            M.setScale({0.5, 0.5, 0.5}); //缩小
            M.multTranslate({position.x, position.y, position.z});  //平移到自己该在的位置
            //下面当前的facelet是否需要旋转。
            if (isAnimationBusy()) { // 有动画就是需要有旋转的面
                //所在平面方程的系数，y=ax+by+cz,把当前facelet所在的cubelet的位置带入方程，如果y大于0就代表当前位置的cubelet属于需要旋转的层
                //相当于一个平面分割了27块cubelet为需要旋转的层和无需旋转的另外两层
                //需要旋转的面的中心cubelet的位置坐标，恰好和这个分割面的表示方程的系数对应。
                const fVec3 &k = ((fVec3 *) cubeletPosition)[cubeFacelets[animationFrameDirection][4]];
                float y = k.x * position.x + k.y * position.y + k.z * position.z;
                if (y > 0) { //如果y大于0就代表当前位置的cubelet属于需要旋转的层
                    //以当前层的法线方向旋转，这层9个cubelet，中间的那个的位置如果看做一个方向向量，正好是法线方向，把他当成旋转轴。
                    const fVec3 &rotateDirection = ((fVec3 *) cubeletPosition)[cubeFacelets[animationFrameDirection][4]];
                    M.multRotate(angle, rotateDirection);   //另外要注意,旋转函数总是绕原点旋转,要注意平移和旋转的先后向顺序
                }
            }
            drawQuad(M, i);  //绘制facelet
            /*--------------------------------------------------------------------------------------------------------------------*/
            // 上面的绘制只是绘制了魔方的外皮，内部其实是空心的。
            // 旋转以后，就会露出没有绘制的内部面，也就是时现实世界把魔方旋转45度，没贴贴纸的，塑料的那部分。
            // 不能让他镂空，所以要在上面渲染上黑的的面，为了好看。
            // 一共需要绘制16个面，也就是旋转层的8个cubelet沿旋转法线反方向的8个面以及
            // 和旋转层挨着的那个中间层沿旋转法线方向的8个面。
            if (!isAnimationBusy()) { // 判断是否有转动,没有转动就不用绘制内部的面了
                continue;
            }
            renderer.setMaterial(RGBf(RGB565_Black), 1, 0, 0, 0); // set material properties
            if (i == animationFrameDirection && j != 4) {
                drawQuad(M, (i + 3) % 6);//对于前8个，就是当前facelet的反方向就行了。
                {// 后8个面  后八个面是不旋转的
                    const fVec3 &offset = ((fVec3 *) cubeletPosition)[cubeFacelets[animationFrameDirection][4]];
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
