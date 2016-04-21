#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    renderArea = new RenderArea;
    ui->centralWidget->layout()->addWidget(renderArea);

    renderArea->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
