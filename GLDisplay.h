#ifndef GLDISPLAY_H
#define GLDISPLAY_H

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QVector>

class GLDisplay : public QOpenGLWidget, QOpenGLFunctions_4_5_Core {
    Q_OBJECT

public:
    explicit GLDisplay(QWidget *parent = nullptr);

public:
    // 地形数据
    // 地形大小（边长），单位：米
    float TerrainSize = 1024;
    // 地形网格细分数
    int TerrainGrid = 1024;
    // 最大海拔高度（也是高度系数），单位：米
    float TerrainMaxHeight = 256;
    // 最低海拔高度是0

public:
    // 顶点数据数组
    QVector<float> *vertices;
    // vbo与vao
    QOpenGLBuffer *VBO_01;
    QOpenGLVertexArrayObject *VAO_01;
    // shader
    QOpenGLShader *vertShader;
    QOpenGLShader *fragShader;
    QOpenGLShaderProgram *shaderProgram;
    // mvp矩阵
    QMatrix4x4 *model;
    QMatrix4x4 *view;
    QMatrix4x4 *proj;

public:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
};

#endif // GLDISPLAY_H
