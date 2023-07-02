#include "GLDisplay.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>

GLDisplay::GLDisplay(QWidget *parent) : QOpenGLWidget(parent) {
    // 将所有将要在堆内存中创建的成员指针初始化为0
    panelVertices = nullptr;
    panelIndices = nullptr;
    panelVBO = nullptr;
    panelEBO = nullptr;
    panelVAO = nullptr;
    screenVBO = nullptr;
    screenVAO = nullptr;

    panelVertShader = nullptr;
    groundGeomShader = nullptr;
    groundFragShader = nullptr;
    waterFragShader = nullptr;
    screenVertShader = nullptr;
    screenFragShader = nullptr;
    groundShaderProgram = nullptr;
    waterShaderProgram = nullptr;
    screenShaderProgram = nullptr;

    setHeight = nullptr;
    setHeightProgram = nullptr;
    erosion = nullptr;
    erosionProgram = nullptr;
    gaussain = nullptr;
    gaussainProgram = nullptr;
    setPixel = nullptr;
    setPixelProgram = nullptr;

    frameBuffer = 0;
    colorBuffer = 0;
    depthBuffer = 0;

    terrainHeightMapData = 0;
    // gaussainKernelData = 0;

    groundHeightMap = nullptr;
    waterHeightMap = nullptr;

    scaleSensitive = 0.001f;
    rotateSensitive = 0.005f;
}

void GLDisplay::initializeGL() {
    // 摄像机参数
    dist = 1;
    theta = 0;
    phi = 1;
    nearPanel = 0.01f;
    farPanel = 10.0f;

    model = QMatrix4x4();
    model.scale(0.001f);
    view = QMatrix4x4();
    view.lookAt(QVector3D(dist * sin(phi) * cos(theta), dist * cos(phi),
                          dist * sin(phi) * sin(theta)),
                QVector3D(), QVector3D(0, 1, 0));
    proj = QMatrix4x4();
    proj.perspective(45, ((float)width()) / height(), nearPanel, farPanel);
    qDebug() << proj;

    initializeOpenGLFunctions();

    generateMesh();

    panelVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    panelEBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    panelVAO = new QOpenGLVertexArrayObject();
    panelVBO->create();
    panelEBO->create();
    panelVAO->create();

    panelVAO->bind();
    panelVBO->bind();
    panelVBO->allocate(panelVertices, sizeof(float) * verticiesCount());
    panelEBO->bind();
    panelEBO->allocate(panelIndices, sizeof(unsigned int) * indicesCount());

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    screenVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    screenVAO = new QOpenGLVertexArrayObject();
    screenVBO->create();
    screenVAO->create();

    screenVAO->bind();
    screenVBO->bind();
    screenVBO->allocate(rectVertices, sizeof(float) * 12);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    panelVertShader = new QOpenGLShader(QOpenGLShader::Vertex);
    groundGeomShader = new QOpenGLShader(QOpenGLShader::Geometry);
    groundFragShader = new QOpenGLShader(QOpenGLShader::Fragment);
    waterFragShader = new QOpenGLShader(QOpenGLShader::Fragment);
    screenVertShader = new QOpenGLShader(QOpenGLShader::Vertex);
    screenFragShader = new QOpenGLShader(QOpenGLShader::Fragment);
    panelVertShader->compileSourceFile(":/Shaders/PanelVertShader.vert");
    groundGeomShader->compileSourceFile(":/Shaders/GroundGeomShader.geom");
    groundFragShader->compileSourceFile(":/Shaders/GroundFragShader.frag");
    waterFragShader->compileSourceFile(":/Shaders/WaterFragShader.frag");
    screenVertShader->compileSourceFile(":/Shaders/ScreenVertShader.vert");
    screenFragShader->compileSourceFile(":/Shaders/ScreenFragShader.frag");

    groundShaderProgram = new QOpenGLShaderProgram();
    groundShaderProgram->create();
    groundShaderProgram->addShader(panelVertShader);
    groundShaderProgram->addShader(groundGeomShader);
    groundShaderProgram->addShader(groundFragShader);
    groundShaderProgram->link();

    waterShaderProgram = new QOpenGLShaderProgram();
    waterShaderProgram->create();
    waterShaderProgram->addShader(panelVertShader);
    waterShaderProgram->addShader(groundGeomShader);
    waterShaderProgram->addShader(waterFragShader);
    waterShaderProgram->link();

    screenShaderProgram = new QOpenGLShaderProgram();
    screenShaderProgram->create();
    screenShaderProgram->addShader(screenVertShader);
    screenShaderProgram->addShader(screenFragShader);
    screenShaderProgram->link();

    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           colorBuffer, 0);

    glGenTextures(1, &depthBuffer);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width(), height(), 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depthBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    groundHeightMap =
        new QOpenGLTexture(QImage(":/Images/Ground.png").mirrored());
    groundHeightMap->setWrapMode(QOpenGLTexture::ClampToEdge);
    groundHeightMap->setMinMagFilters(QOpenGLTexture::Linear,
                                      QOpenGLTexture::Linear);

    waterHeightMap =
        new QOpenGLTexture(QImage(":/Images/Water.png").mirrored());
    waterHeightMap->setWrapMode(QOpenGLTexture::ClampToEdge);
    waterHeightMap->setMinMagFilters(QOpenGLTexture::Linear,
                                     QOpenGLTexture::Linear);

    // 地形计算：buffer与shader
    setHeight = new QOpenGLShader(QOpenGLShader::Compute);
    setHeight->compileSourceFile(":/Shaders/setHeight.comp");

    setHeightProgram = new QOpenGLShaderProgram();
    setHeightProgram->create();
    setHeightProgram->addShader(setHeight);
    setHeightProgram->link();

    // 侵蚀
    erosion = new QOpenGLShader(QOpenGLShader::Compute);
    erosion->compileSourceFile(":/Shaders/Erosion.comp");

    erosionProgram = new QOpenGLShaderProgram();
    erosionProgram->create();
    erosionProgram->addShader(erosion);
    erosionProgram->link();

    // 设置像素着色器
    setPixel = new QOpenGLShader(QOpenGLShader::Compute);
    setPixel->compileSourceFile(":/Shaders/SetPixelData.comp");

    setPixelProgram = new QOpenGLShaderProgram();
    setPixelProgram->create();
    setPixelProgram->addShader(setPixel);
    setPixelProgram->link();

    qDebug() << glGetError();

    // 计算buffer（是image/texure不是render buffer）
    glGenTextures(1, &terrainHeightMapData);
    glBindTexture(GL_TEXTURE_2D, terrainHeightMapData);
    glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, terrainGrid, terrainGrid);

    // 填充地形buffer：
    // 1、绑定地形buffer到0纹理位置
    glBindImageTexture(0, terrainHeightMapData, 0, GL_FALSE, 0, GL_READ_WRITE,
                       GL_RGBA32F);

    // 2、绑定计算着色器
    setHeightProgram->bind();

    // 2、运行计算着色器
    glDispatchCompute(32, 32, 1);

    //     完成地形侵蚀
    //     检查错误
    //     地形数据绑定到0的纹理位置
    //     绑定计算buffer
    glBindImageTexture(0, terrainHeightMapData, 0, GL_FALSE, 0, GL_READ_WRITE,
                       GL_RGBA32F);
    // 绑定计算着色器
    erosionProgram->bind();
    // 传参
    erosionProgram->setUniformValue("Grid", terrainGrid);
    erosionProgram->setUniformValue("Size", TerrainSize);
    erosionProgram->setUniformValue("Height", terrainMaxHeight);

    erosionProgram->setUniformValue("water_len", 600);
    erosionProgram->setUniformValue("p_inertia", 0.603f);
    erosionProgram->setUniformValue("p_minSlope", 0.0001f);
    erosionProgram->setUniformValue("p_capacity", 4.0f);
    erosionProgram->setUniformValue("p_deposition", 0.418f);
    erosionProgram->setUniformValue("p_erosion", 0.622f);
    erosionProgram->setUniformValue("p_gravity", 1.27f);
    erosionProgram->setUniformValue("p_evaporation", 0.039f);
    erosionProgram->setUniformValue("p_radius", 5.0f);
    erosionProgram->setUniformValue("p_hardness", 0.517f);
    erosionProgram->setUniformValue("p_velocity", 2.42f);

    erosionProgram->setUniformValue("time", 0.0f);

    qDebug() << glGetError();

    for (int i = 5; i < 500; ++i) {
        erosionProgram->setUniformValue("time", (float)i * 0.5623f + 5);
        glDispatchCompute(32, 1, 1);
    }
}

void GLDisplay::resizeGL(int w, int h) {
    view = QMatrix4x4();
    view.lookAt(QVector3D(dist * sin(phi) * cos(theta), dist * cos(phi),
                          dist * sin(phi) * sin(theta)),
                QVector3D(), QVector3D(0, 1, 0));
    proj = QMatrix4x4();
    proj.perspective(45, ((float)w) / h, nearPanel, farPanel);
}

void GLDisplay::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClearColor(0.2f, 0.8f, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    groundHeightMap->bind(0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    groundShaderProgram->bind();
    groundShaderProgram->setUniformValue("model", model);
    groundShaderProgram->setUniformValue("view", view);
    groundShaderProgram->setUniformValue("proj", proj);
    groundShaderProgram->setUniformValue("maxHeight", terrainMaxHeight);
    groundShaderProgram->setUniformValue("gridSize", TerrainSize);
    groundShaderProgram->setUniformValue("nearPanel", nearPanel);
    groundShaderProgram->setUniformValue("farPanel", farPanel);
    groundShaderProgram->setUniformValue("heightMap", 0);
    waterShaderProgram->setUniformValue("depthBuffer", 1);
    panelVAO->bind();
    glDrawElements(GL_TRIANGLES, indicesCount(), GL_UNSIGNED_INT, 0);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    waterHeightMap->bind(0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    waterShaderProgram->bind();
    waterShaderProgram->setUniformValue("model", model);
    waterShaderProgram->setUniformValue("view", view);
    waterShaderProgram->setUniformValue("proj", proj);
    waterShaderProgram->setUniformValue("maxHeight", terrainMaxHeight);
    waterShaderProgram->setUniformValue("gridSize", TerrainSize);
    waterShaderProgram->setUniformValue("nearPanel", nearPanel);
    waterShaderProgram->setUniformValue("farPanel", farPanel);
    waterShaderProgram->setUniformValue("heightMap", 0);
    waterShaderProgram->setUniformValue("depthBuffer", 1);
    panelVAO->bind();
    glDrawElements(GL_TRIANGLES, indicesCount(), GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    screenShaderProgram->bind();
    screenShaderProgram->setUniformValue("colorBuffer", 0);
    screenShaderProgram->setUniformValue("depthBuffer", 1);
    screenVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLDisplay::mousePressEvent(QMouseEvent *event) {
    prevMousePosition = event->pos();
}

void GLDisplay::mouseMoveEvent(QMouseEvent *event) {
    QPoint delta = event->pos() - prevMousePosition;
    theta += rotateSensitive * delta.x();
    phi -= rotateSensitive * delta.y();
    view = QMatrix4x4();
    view.lookAt(QVector3D(dist * sin(phi) * cos(theta), dist * cos(phi),
                          dist * sin(phi) * sin(theta)),
                QVector3D(), QVector3D(0, 1, 0));
    prevMousePosition = event->pos();
    update();
}

void GLDisplay::mouseReleaseEvent(QMouseEvent *event) {
    mouseMoveEvent(event);
    update();
}

void GLDisplay::wheelEvent(QWheelEvent *event) {
    dist *= exp(-(event->angleDelta().x() + event->angleDelta().y()) *
                scaleSensitive);
    view = QMatrix4x4();
    view.lookAt(QVector3D(dist * sin(phi) * cos(theta), dist * cos(phi),
                          dist * sin(phi) * sin(theta)),
                QVector3D(), QVector3D(0, 1, 0));
    update();
}

void GLDisplay::generateMesh() {
    panelVertices = new float[verticiesCount()];
    float unit = ((float)TerrainSize) / terrainGrid;
    float offset = ((float)TerrainSize) / 2;
    for (int z = 0; z < terrainGrid; z++) {
        for (int x = 0; x < terrainGrid; x++) {
            int index = (z * terrainGrid + x) * 2;
            panelVertices[index + 0] = x * unit - offset;
            panelVertices[index + 1] = z * unit - offset;
        }
    }

    panelIndices = new unsigned int[indicesCount()];
    for (int z = 0; z < terrainGrid - 1; z++) {
        for (int x = 0; x < terrainGrid - 1; x++) {
            int iindex = (z * (terrainGrid - 1) + x) * 6;
            int vindex = z * terrainGrid + x;
            panelIndices[iindex + 0] = vindex + 0;
            panelIndices[iindex + 1] = vindex + 1;
            panelIndices[iindex + 2] = vindex + terrainGrid + 1;
            panelIndices[iindex + 3] = vindex + 0;
            panelIndices[iindex + 4] = vindex + terrainGrid + 1;
            panelIndices[iindex + 5] = vindex + terrainGrid;
        }
    }

    rectVertices = new float[12]{-1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1};
}

unsigned int GLDisplay::verticiesCount() const {
    return terrainGrid * terrainGrid * 2;
}

unsigned int GLDisplay::indicesCount() const {
    return (terrainGrid - 1) * (terrainGrid - 1) * 6;
}

void GLDisplay::SetImagePixelRGBA32F(unsigned int data, float value, int posx,
                                     int posy) {
    glBindImageTexture(0, data, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    setPixelProgram->bind();
    setPixelProgram->setUniformValue("value", value);
    setPixelProgram->setUniformValue("posx", posx);
    setPixelProgram->setUniformValue("posy", posy);
    glDispatchCompute(1, 1, 1);
}
