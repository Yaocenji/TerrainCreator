#include "GLDisplay.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>

GLDisplay::GLDisplay(QWidget *parent) : QOpenGLWidget(parent) {
    panelVertices = nullptr;
    panelIndices = nullptr;
    panelVBO = nullptr;
    panelEBO = nullptr;
    panelVAO = nullptr;
    screenVBO = nullptr;
    screenVAO = nullptr;

    panelVertShader = nullptr;
    groundFragShader = nullptr;
    waterFragShader = nullptr;
    screenVertShader = nullptr;
    screenFragShader = nullptr;
    groundShaderProgram = nullptr;
    waterShaderProgram = nullptr;
    screenShaderProgram = nullptr;

    frameBuffer = 0;
    colorBuffer = 0;
    depthBuffer = 0;

    groundHeightMap = nullptr;
    waterHeightMap = nullptr;

    scaleSensitive = 0.001f;
    rotateSensitive = 0.005f;
}

void GLDisplay::initializeGL() {
    dist = 1;
    theta = 0;
    phi = 1;

    model = QMatrix4x4();
    model.scale(0.001f);
    view = QMatrix4x4();
    view.lookAt(QVector3D(dist * sin(phi) * cos(theta), dist * cos(phi),
                          dist * sin(phi) * sin(theta)),
                QVector3D(), QVector3D(0, 1, 0));
    proj = QMatrix4x4();
    proj.perspective(45, ((float)width()) / height(), 0.1f, 10000);
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
    groundFragShader = new QOpenGLShader(QOpenGLShader::Fragment);
    waterFragShader = new QOpenGLShader(QOpenGLShader::Fragment);
    screenVertShader = new QOpenGLShader(QOpenGLShader::Vertex);
    screenFragShader = new QOpenGLShader(QOpenGLShader::Fragment);
    panelVertShader->compileSourceFile(":/Shaders/PanelVertShader.vert");
    groundFragShader->compileSourceFile(":/Shaders/GroundFragShader.frag");
    waterFragShader->compileSourceFile(":/Shaders/WaterFragShader.frag");
    screenVertShader->compileSourceFile(":/Shaders/ScreenVertShader.vert");
    screenFragShader->compileSourceFile(":/Shaders/ScreenFragShader.frag");

    groundShaderProgram = new QOpenGLShaderProgram();
    groundShaderProgram->create();
    groundShaderProgram->addShader(panelVertShader);
    groundShaderProgram->addShader(groundFragShader);
    groundShaderProgram->link();

    waterShaderProgram = new QOpenGLShaderProgram();
    waterShaderProgram->create();
    waterShaderProgram->addShader(panelVertShader);
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
}

void GLDisplay::resizeGL(int w, int h) {
    view = QMatrix4x4();
    view.lookAt(QVector3D(dist * sin(phi) * cos(theta), dist * cos(phi),
                          dist * sin(phi) * sin(theta)),
                QVector3D(), QVector3D(0, 1, 0));
    proj = QMatrix4x4();
    proj.perspective(45, ((float)w) / h, 0.1f, 10);
}

void GLDisplay::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
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
    groundShaderProgram->setUniformValue("nearPanel", 0.1f);
    groundShaderProgram->setUniformValue("farPanel", 10.0f);
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
    waterShaderProgram->setUniformValue("nearPanel", 0.1f);
    waterShaderProgram->setUniformValue("farPanel", 10.0f);
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
