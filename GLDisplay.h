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
    QOpenGLShader *groundFragShader, *waterFragShader;
    QOpenGLShader *screenVertShader;
    QOpenGLShader *screenFragShader;
    QOpenGLShaderProgram *groundShaderProgram, *waterShaderProgram;
    QOpenGLShaderProgram *screenShaderProgram;
    // mvp矩阵
    QMatrix4x4 model, view, proj;

    unsigned int frameBuffer;
    unsigned int colorBuffer, depthBuffer;

    QOpenGLTexture *groundHeightMap, *waterHeightMap;

protected:
    float dist, theta, phi;
    float scaleSensitive, rotateSensitive;
    QPoint prevMousePosition;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void generateMesh();
    unsigned int verticiesCount() const;
    unsigned int indicesCount() const;
};

#endif // GLDISPLAY_H
