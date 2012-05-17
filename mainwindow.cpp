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
#include "material.h"
#include "lightscontext.h"
#include "light.h"
#include <QToolBar>

#define FPS 50

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    vp(new Viewport(this)),
    renderer(new Renderer()),
    om(NULL), //objectmodel
    timer(new QTimer(this)), // used to tick the auto-rotate animation
    background(new QPushButton(this)),
    objectMaterial(new Material(QColor(135,160,180), QColor(255,255,255), QColor(170, 200, 225), 20.0f)),
    lightsContext(new LightsContext()),
    isAnimated(false)
{
    ui->setupUi(this);
    setCentralWidget(vp);

    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionAnimation->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionToggle_wireframe->setIcon(QIcon(":/wireframe.png"));

    QToolBar *toolBar = new QToolBar();
    toolBar->addAction(ui->actionOpen);
    toolBar->addAction(ui->actionAnimation);
    toolBar->addAction(ui->actionToggle_wireframe);

    ui->dockWidgetContents->layout()->addWidget(toolBar);


    // Background button setup
    background->setText("Background");
    QWidget *stretch = new QWidget(toolBar);
    stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    toolBar->addWidget(stretch);
    toolBar->addWidget(background);
    connect(background, SIGNAL(released()), this, SLOT(changeBackgroundColor()));

    //Right pane setup
    ui->dockWidget->setTitleBarWidget(new QWidget(this));

    //Object tab
    ui->objectAmbientButton->setPalette(QPalette(objectMaterial->getKA()));
    ui->objectAmbientButton->setText(objectMaterial->getKA().name());
    ui->objectDiffuseButton->setPalette(QPalette(objectMaterial->getKD()));
    ui->objectDiffuseButton->setText(objectMaterial->getKD().name());
    ui->objectSpecularButton->setPalette(QPalette(objectMaterial->getKS()));
    ui->objectSpecularButton->setText(objectMaterial->getKS().name());
    ui->objectShininessSpinbox->setValue(objectMaterial->getQ());
    connect(ui->objectAmbientButton, SIGNAL(released()), this, SLOT(changeObjectAmbient()));
    connect(ui->objectDiffuseButton, SIGNAL(released()), this, SLOT(changeObjectDiffuse()));
    connect(ui->objectSpecularButton, SIGNAL(released()), this, SLOT(changeObjectSpecular()));
    connect(ui->objectShininessSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changeObjectShininess(double)));

    //Lights tab
    ui->lightsAmbientButton->setPalette(QPalette(lightsContext->getAmbient()));
    ui->lightsAmbientButton->setText(lightsContext->getAmbient().name());
    connect(ui->lightsAmbientButton, SIGNAL(released()), this, SLOT(changeLightAmbient()));
    connect(ui->lightsAdd, SIGNAL(released()), this, SLOT(addLight()));
    connect(ui->lightsDelete, SIGNAL(released()), this, SLOT(deleteSelectedLight()));
    connect(ui->lightsListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateLightButtons()));
    connect(ui->lightsDiffuseButton, SIGNAL(released()), this, SLOT(changeLightDiffuse()));
    connect(ui->lightsSpecularButton, SIGNAL(released()), this, SLOT(changeLightSpecular()));
    connect(ui->lightsXSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changeLightPosition()));
    connect(ui->lightsYSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changeLightPosition()));
    connect(ui->lightsZSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changeLightPosition()));
    addLight();
    lightsContext->getLightPtrAt(0)->setID(QColor(176,176,176));
    updateLightButtons();



    timer->setInterval(1000/FPS);
    connect(timer, SIGNAL(timeout()), this, SLOT(autoRotate()));

    connect(vp, SIGNAL(resized()), this, SLOT(reRender()));
    on_actionOpen_triggered(); // Load dialog at create
//    ///////////////77
//    om = SUROpener::openSUR("surfaces/misil-ori.sur");
//    reRender();
//    ////////////7////
    ui->actionAnimation->toggle();
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
    bool wasAnimated = isAnimated;
    if (wasAnimated)
        ui->actionAnimation->toggle();
    QString filepath = QFileDialog::getOpenFileName(this, QString("Select the surface to load"), NULL, "Surfaces (*.sur *.SUR)");
    if (filepath != NULL) {
        if (om != NULL)
            delete om;
        om = SUROpener::openSUR(filepath);  // Reuse this :-)
    }
    if (wasAnimated)
        ui->actionAnimation->toggle();
    else
        reRender();
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
    isAnimated = s;
}

void MainWindow::on_actionToggle_wireframe_toggled(bool s)
{
    renderer->setWireframeVisibility(s);
    reRender();
}

void MainWindow::reRender()
{
    if (om == NULL) return;
    vp->setImage(renderer->render(om, vp->size(), objectMaterial, lightsContext));
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

void MainWindow::addLight()
{
    Light *l = new Light();
    lightsContext->addLight(l);
    ui->lightsListWidget->setCurrentItem(new QListWidgetItem(vertex2String(l->getPos()), ui->lightsListWidget));
    if (!isAnimated) reRender();
}

void MainWindow::deleteSelectedLight()
{
    unsigned pos = ui->lightsListWidget->currentRow();
    delete (ui->lightsListWidget->takeItem(pos)); // This fires an event that calls updateLightsButtons().
    lightsContext->deleteLight(pos); //Don't remove the light before that event is processed!
    if (!isAnimated) reRender();
}

QString MainWindow::vertex2String(Vertex v)
{
    return QString("(%1, %2, %3)").arg(v.x()).arg(v.y()).arg(v.z());
}

void MainWindow::updateLightButtons()
{
    int pos = ui->lightsListWidget->currentRow();
    if (pos == -1) {
        ui->lightsDiffuseButton->setPalette(QPalette(Qt::gray));
        ui->lightsDiffuseButton->setText("");
        ui->lightsSpecularButton->setPalette(QPalette(Qt::gray));
        ui->lightsSpecularButton->setText("");
        //Vector
        ui->lightsXSpinbox->setValue(0.0);
        ui->lightsYSpinbox->setValue(0.0);
        ui->lightsZSpinbox->setValue(0.0);
    } else {
        Light l = lightsContext->getLightAt(pos);
        //Intensities
        ui->lightsDiffuseButton->setPalette(QPalette(l.getID()));
        ui->lightsDiffuseButton->setText(l.getID().name());
        ui->lightsSpecularButton->setPalette(QPalette(l.getIS()));
        ui->lightsSpecularButton->setText(l.getIS().name());
        //Vector
        ui->lightsXSpinbox->setValue(l.getPos().x());
        ui->lightsYSpinbox->setValue(l.getPos().y());
        ui->lightsZSpinbox->setValue(l.getPos().z());
    }
    bool valid = (pos != -1);
    ui->lightsDiffuseButton->setEnabled(valid);
    ui->lightsSpecularButton->setEnabled(valid);
    ui->lightsXSpinbox->setEnabled(valid);
    ui->lightsYSpinbox->setEnabled(valid);
    ui->lightsZSpinbox->setEnabled(valid);
    ui->lightsDelete->setEnabled(valid);
}

/*
 * Handler for color change
 */
void MainWindow::changeBackgroundColor()
{
    QColor color;
    if((color = QColorDialog::getColor(renderer->getBackgroundColor(), this)).isValid())
    {
        renderer->setBackgroundColor(color);
        if (!isAnimated) reRender();
    }
}

void MainWindow::changeObjectAmbient()
{
    QColor color;
    if((color = QColorDialog::getColor(objectMaterial->getKA(), this)).isValid())
    {
        objectMaterial->setKA(color);
        ui->objectAmbientButton->setPalette(QPalette(color));
        ui->objectAmbientButton->setText(color.name());
        if (!isAnimated) reRender();
    }
}

void MainWindow::changeObjectDiffuse()
{
    QColor color;
    if((color = QColorDialog::getColor(objectMaterial->getKD(), this)).isValid())
    {
        objectMaterial->setKD(color);
        ui->objectDiffuseButton->setPalette(QPalette(color));
        ui->objectDiffuseButton->setText(color.name());
        if (!isAnimated) reRender();
    }
}

void MainWindow::changeObjectSpecular()
{
    QColor color;
    if((color = QColorDialog::getColor(objectMaterial->getKS(), this)).isValid())
    {
        objectMaterial->setKS(color);
        ui->objectSpecularButton->setPalette(QPalette(color));
        ui->objectSpecularButton->setText(color.name());
        if (!isAnimated) reRender();
    }
}

void MainWindow::changeObjectShininess(double d)
{
    objectMaterial->setQ(d);
    if (!isAnimated) reRender();
}

void MainWindow::changeLightAmbient()
{
    QColor color;
    if((color = QColorDialog::getColor(lightsContext->getAmbient(), this)).isValid())
    {
        lightsContext->setAmbient(color);
        ui->lightsAmbientButton->setPalette(QPalette(color));
        ui->lightsAmbientButton->setText(color.name());
        if (!isAnimated) reRender();
    }
}

void MainWindow::changeLightDiffuse()
{
    int pos = ui->lightsListWidget->currentRow();
    if (pos != -1) {
        Light *l = lightsContext->getLightPtrAt(pos);
        QColor color;
        if((color = QColorDialog::getColor(l->getID(), this)).isValid())
        {
            l->setID(color);
            ui->lightsDiffuseButton->setPalette(QPalette(color));
            ui->lightsDiffuseButton->setText(color.name());
            if (!isAnimated) reRender();
        }
    }
}

void MainWindow::changeLightSpecular()
{
    int pos = ui->lightsListWidget->currentRow();
    if (pos != -1) {
        Light *l = lightsContext->getLightPtrAt(pos);
        QColor color;
        if((color = QColorDialog::getColor(l->getIS(), this)).isValid())
        {
            l->setIS(color);
            ui->lightsSpecularButton->setPalette(QPalette(color));
            ui->lightsSpecularButton->setText(color.name());
            if (!isAnimated) reRender();
        }
    }
}

void MainWindow::changeLightPosition()
{
    int pos = ui->lightsListWidget->currentRow();
    if (pos != -1) {
        Light *l = lightsContext->getLightPtrAt(pos);
        l->setPos(Vertex(ui->lightsXSpinbox->value(), ui->lightsYSpinbox->value(), ui->lightsZSpinbox->value()));
        ui->lightsListWidget->currentItem()->setText(vertex2String(l->getPos()));
        if (!isAnimated) reRender();
    }
}
