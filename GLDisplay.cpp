#include "GLDisplay.h"

GLDisplay::GLDisplay(QWidget *parent) : QOpenGLWidget(parent) {
    vertices = nullptr;
    VBO_01 = nullptr;
    VAO_01 = nullptr;
    vertShader = nullptr;
    fragShader = nullptr;
    shaderProgram = nullptr;
    model = nullptr;
    view = nullptr;
    proj = nullptr;
}

void GLDisplay::initializeGL() {
    // 初始化OpenGL原生函数
    initializeOpenGLFunctions();
    // 填充一个数组：
    vertices = new QVector<float>;
    // todo: 遍历小正方形，每个正方形生成两个三角形，六个顶点

    VBO_01 = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    VBO_01->create();
    VAO_01 = new QOpenGLVertexArrayObject();
    VAO_01->create();

    VAO_01->bind();
    VBO_01->bind();

    VBO_01->allocate();
}

void GLDisplay::resizeGL(int w, int h) {
}

void GLDisplay::paintGL() {
}
