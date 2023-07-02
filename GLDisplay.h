#ifndef GLDISPLAY_H
#define GLDISPLAY_H

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

class GLDisplay : public QOpenGLWidget, QOpenGLFunctions_4_5_Core {
    Q_OBJECT

public:
    explicit GLDisplay(QWidget *parent = nullptr);

public:
    // 地形数据
    // 地形大小（边长），单位：米
    float TerrainSize = 1024;
    // 地形网格细分数
    int terrainGrid = 1024;
    // 最大海拔高度（也是高度系数），单位：米
    float terrainMaxHeight = 256;
    // 最低海拔高度是0

public:
    // 侵蚀算法参数：
    // 流水最大长度
    int water_len = 10;
    // 流水惯性参数（0~1）
    float p_inertia = 0.4;
    // 最小坡度
    float p_minSlope = 1.0;
    // 携带容量
    float p_capacity = 8.0;
    // 沉积系数（0~1）")
    float p_deposition = 0.1;
    // 侵蚀系数（0~1）")
    float p_erosion = 0.75;
    // 重力系数（0~10）")
    float p_gravity = 1.0;
    // 蒸发系数（0~1）")
    float p_evaporation = 0.002;
    // 侵蚀半径（1~50）")
    float p_radius = 2.0;
    // 硬度系数（0~1）
    float p_hardness = 1.0;
    // 初始速度（0~10）")
    float p_velocity = 1.0;

protected:
    // 顶点数据数组
    float *panelVertices;
    unsigned int *panelIndices;
    float *rectVertices;
    // vbo与vao
    QOpenGLBuffer *panelVBO, *panelEBO;
    QOpenGLVertexArrayObject *panelVAO;
    QOpenGLBuffer *screenVBO;
    QOpenGLVertexArrayObject *screenVAO;
    // shader
    QOpenGLShader *panelVertShader;
    QOpenGLShader *groundGeomShader;
    QOpenGLShader *groundFragShader, *waterFragShader;
    QOpenGLShader *screenVertShader;
    QOpenGLShader *screenFragShader;
    QOpenGLShaderProgram *groundShaderProgram, *waterShaderProgram;
    QOpenGLShaderProgram *screenShaderProgram;

    // 计算shader
    // 设置高度
    QOpenGLShader *setHeight;
    QOpenGLShaderProgram *setHeightProgram;
    // 侵蚀
    QOpenGLShader *erosion;
    QOpenGLShaderProgram *erosionProgram;
    // 高斯分布
    QOpenGLShader *gaussain;
    QOpenGLShaderProgram *gaussainProgram;
    // 设置buffer像素
    QOpenGLShader *setPixel;
    QOpenGLShaderProgram *setPixelProgram;

    //
    // mvp矩阵
    QMatrix4x4 model, view, proj;

    // frameBuffer，是离屏渲染的帧缓冲区
    unsigned int frameBuffer;
    unsigned int colorBuffer, depthBuffer;

    // tex(storage/compute buffer)，是地形数据
    unsigned int terrainHeightMapData;

    // 两张测试用的高度贴图
    QOpenGLTexture *groundHeightMap, *waterHeightMap;

protected:
    // 摄相机参数
    float dist, theta, phi;
    float nearPanel, farPanel;
    float scaleSensitive, rotateSensitive;
    QPoint prevMousePosition;

protected:
    // GL窗口内建函数
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // 摄像机控制函数
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void generateMesh();
    unsigned int verticiesCount() const;
    unsigned int indicesCount() const;
    void SetImagePixelRGBA32F(unsigned int data, float value, int posx,
                              int posy);
};

#endif // GLDISPLAY_H
