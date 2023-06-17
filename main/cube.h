#include "tgx.h"

using namespace tgx;

/*
 * up right front down left behind
 * 魔方的六个面，以后所有数据都是按这个顺序
 */
typedef enum : int8_t {
    U = 0,
    R = 1,
    F = 2,
    D = 3,
    L = 4,
    B = 5
} Faces;


/*
 * 魔方颜色数据保存在这里
 */
extern uint8_t cubeBuffer[6][9];
/*
 * 旋转一层时,该层侧面的12个facelet,按顺时针
 */
extern const uint8_t SideFaceletRotate[6][12];

/*
 * 每个facelet所在的的cubelet所对应cubeletPosition的索引
 */
extern const uint8_t cubeFacelets[6][9];

/*
 * 定义魔方六个面的颜色，标准配色。
 */
extern const RGB565 colors[6];

/*
 * 魔方27的cubelet对应的位置坐标。
 */
extern const fVec3 cubeletPosition[3][3][3];

/*
 * 一个普通立方体的的8个顶点坐标
 */
extern const tgx::fVec3 GENERAL_CUBE_POINT[8];
/*
 * 一个立方体的六个面，每面上的四个点，用这4顶点画正方形，6个正方形组成立方体
 */
extern const uint16_t GENERAL_CUBE_FACES[6][4];

/**
 * 旋转魔方的某层
 * @param cube cubeBuffer
 * @param dir 旋转哪层   0-6
 * @param clockwise 顺时针或逆时针
 */
void rotateCube(uint8_t cube[6][9], uint8_t dir, bool clockwise);
/**
 * 开始旋转魔方某层的动画
 * @param dir 旋转哪层   0-6
 * @param clockwise 顺时针或逆时针
 * @param frameCount 动画持续多少帧率
 * @return 是否成功开始动画,如果上一次动画ing,就会失败
 */
bool startRotateCube(uint8_t dir, bool clockwise, uint8_t frameCount);