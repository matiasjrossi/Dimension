#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "viewport.h"
#include "renderer.h"
#include "objectmodel.h"
#include "suropener.h"
#include "renderer.h"
#include <QTimer>
#include <QFileDialog>
#include <QPushButton>
#include <QColorDialog>
#include <QLabel>
#include <QShowEvent>

#define FPS 50

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    vp(new Viewport(this)),
    renderer(new Renderer()), //??
    om(NULL), //objectmodel
    timer(new QTimer(this)), // used to tick the auto-rotate animation

    //These are the buttons from the top, used to choose the colors
    background(new QPushButton(this)),
    object(new QPushButton(this)),
    wireframe(new QPushButton(this)),
    light(new QPushButton(this))
{
    ui->setupUi(this);
    setCentralWidget(vp);

    ui->mainToolBar->addAction(ui->actionOpen);
    ui->mainToolBar->addAction(ui->actionAnimation);
    ui->mainToolBar->addAction(ui->actionToggle_wireframe);
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionAnimation->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionToggle_wireframe->setIcon(QIcon(":/wireframe.png"));

    // Color buttons setup
    background->setAutoFillBackground(true);
    object->setAutoFillBackground(true);
    wireframe->setAutoFillBackground(true);
    light->setAutoFillBackground(true);
    background->setPalette(QPalette(renderer->getBackgroundColor()));
    object->setPalette(QPalette(renderer->getObjectColor()));
    wireframe->setPalette(QPalette(renderer->getWireframeColor()));
    light->setPalette(QPalette(renderer->getLightColor()));
    background->setFlat(true);
    object->setFlat(true);
    wireframe->setFlat(true);
    light->setFlat(true);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(new QLabel("Colors: ", this));
    ui->mainToolBar->addWidget(background);
    ui->mainToolBar->addWidget(object);
    ui->mainToolBar->addWidget(wireframe);
    ui->mainToolBar->addWidget(light);
    connect(background, SIGNAL(released()), this, SLOT(changeBackgroundColor()));
    connect(object, SIGNAL(released()), this, SLOT(changeObjectColor()));
    connect(wireframe, SIGNAL(released()), this, SLOT(changeWireframeColor()));
    connect(light, SIGNAL(released()), this, SLOT(changeLightColor()));

    timer->setInterval(1000/FPS);
    connect(timer, SIGNAL(timeout()), this, SLOT(autoRotate()));

    connect(vp, SIGNAL(resized()), this, SLOT(reRender()));
    on_actionOpen_triggered(); // Load dialog at create
    on_actionAnimation_toggled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, QString("Select the surface to load"), NULL, "Surfaces (*.sur *.SUR)");
    if (filepath == NULL) return;
    if (om != NULL)
        delete om;
    om = SUROpener::openSUR(filepath);  // Reuse this :-)
}

void MainWindow::on_actionAnimation_toggled(bool s)
{
    if (s)
    {
        disconnect(this, SLOT(rotate(double,double)));
        timer->start();
    } else {
        timer->stop();
        connect(vp, SIGNAL(mouseDrag(double,double)), this, SLOT(rotate(double,double)));
    }
}

void MainWindow::on_actionToggle_wireframe_toggled(bool s)
{
    renderer->setWireframeVisibility(s);
    reRender();
}

void MainWindow::reRender()
{
    if (om == NULL) return;
    vp->setImage(renderer->render(om, vp->size()));
}

void MainWindow::rotate(double x, double y)
{
    renderer->changeRotation(x, y);
    reRender();
}

void MainWindow::autoRotate()
{
    rotate(PI/(FPS*2), PI/(FPS*20));
}

/*
 * Handlers for color change
 */
void MainWindow::changeBackgroundColor()
{
    QColor color;
    if((color = QColorDialog::getColor(renderer->getBackgroundColor(), this)).isValid())
    {
        renderer->setBackgroundColor(color);
        background->setPalette(QPalette(color));
    }
    reRender();
}

void MainWindow::changeObjectColor()
{
    QColor color;
    if((color = QColorDialog::getColor(renderer->getObjectColor(), this)).isValid())
    {
        renderer->setObjectColor(color);
        object->setPalette(QPalette(color));
    }
    reRender();
}


void MainWindow::changeWireframeColor()
{
    QColor color;
    if((color = QColorDialog::getColor(renderer->getWireframeColor(), this)).isValid())
    {
        renderer->setWireframeColor(color);
        wireframe->setPalette(QPalette(color));
    }
    reRender();
}

void MainWindow::changeLightColor()
{
    QColor color;
    if((color = QColorDialog::getColor(renderer->getLightColor(), this)).isValid())
    {
        renderer->setLightColor(color);
        light->setPalette(QPalette(color));
    }
    reRender();
}
