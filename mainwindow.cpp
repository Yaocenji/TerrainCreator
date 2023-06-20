#include "mainwindow.h"

#include "GLDisplay.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(800, 600);
    setCentralWidget(new GLDisplay(this));
}

MainWindow::~MainWindow() {
}
