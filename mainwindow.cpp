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
    transformations(new QList<Transformation*>()),
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

    // Mouse events for the viewport
    connect(vp, SIGNAL(mouseDrag(double,double)), this, SLOT(rotate(double,double)));
    connect(vp, SIGNAL(mouseWheel(int)), this, SLOT(zoom(int)));
    connect(vp, SIGNAL(objectShift(double,double)), this, SLOT(shift(double, double)));

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
    connect(ui->lightsAddButton, SIGNAL(released()), this, SLOT(addLight()));
    connect(ui->lightsDeleteButton, SIGNAL(released()), this, SLOT(deleteSelectedLight()));
    connect(ui->lightsListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateLightButtons()));
    connect(ui->lightsDiffuseButton, SIGNAL(released()), this, SLOT(changeLightDiffuse()));
    connect(ui->lightsSpecularButton, SIGNAL(released()), this, SLOT(changeLightSpecular()));
    connect(ui->lightsXSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changeLightPosition()));
    connect(ui->lightsYSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changeLightPosition()));
    connect(ui->lightsZSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changeLightPosition()));
    connect(ui->lightsRotateWithCameraCheckbox, SIGNAL(toggled(bool)), this, SLOT(changeLightRotateWithCamera(bool)));
    addLight();
    lightsContext->getLightPtrAt(0)->setID(QColor(176,176,176));
    updateLightButtons();

    //Transformations tab
    connect(ui->transAddButton, SIGNAL(released()), this, SLOT(addTransformation()));
    connect(ui->transDeleteButton, SIGNAL(released()), this, SLOT(deleteSelectedTransformation()));
    connect(ui->transListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateTransformationUI()));

    // Common
    connect(ui->transTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(readTransformationUI()));
    connect(ui->transCoordinatesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(readTransformationUI()));

    // Linear
    connect(ui->transA1, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transA2, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transA3, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transA4, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transB1, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transB2, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transB3, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transB4, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transC1, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transC2, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transC3, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transC4, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transD1, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transD2, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transD3, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));
    connect(ui->transD4, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));

    // Twisting
    connect(ui->twistingSpinsSpinBox, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));

    // Bending
    connect(ui->bendingRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(readTransformationUI()));



    updateTransformationUI();


    timer->setInterval(1000/FPS);
    connect(timer, SIGNAL(timeout()), this, SLOT(autoRotate()));

    connect(vp, SIGNAL(resized()), this, SLOT(reRender()));
    on_actionOpen_triggered(); // Load dialog at create
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
        timer->start();
    } else {
        timer->stop();
    }
    isAnimated = s;
}

void MainWindow::on_actionToggle_wireframe_toggled(bool s)
{
    renderer->setWireframeVisibility(s);
    if (!isAnimated) reRender();
}

void MainWindow::reRender()
{
    if (om == NULL) return;
    vp->setImage(renderer->render(om, vp->size(), objectMaterial, lightsContext, transformations));
}

void MainWindow::rotate(double x, double y)
{
    renderer->changeRotation(x, y);
    if (!isAnimated) reRender();
}

void MainWindow::zoom(int z)
{
    renderer->zoom(z);
    if (!isAnimated) reRender();
}

void MainWindow::shift(double x, double y)
{
    renderer->shift(x, y);
    if (!isAnimated) reRender();
}

void MainWindow::autoRotate()
{
    rotate(1.0/(FPS*4), 1.0/(FPS*40));
    reRender();
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
        ui->lightsRotateWithCameraCheckbox->setChecked(false);
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
        ui->lightsRotateWithCameraCheckbox->setChecked(l.isRotateWithCamera());
    }
    bool valid = (pos != -1);
    ui->lightsDiffuseButton->setEnabled(valid);
    ui->lightsSpecularButton->setEnabled(valid);
    ui->lightsXSpinbox->setEnabled(valid);
    ui->lightsYSpinbox->setEnabled(valid);
    ui->lightsZSpinbox->setEnabled(valid);
    ui->lightsDeleteButton->setEnabled(valid);
    ui->lightsRotateWithCameraCheckbox->setEnabled(valid);
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

void MainWindow::changeLightRotateWithCamera(bool r)
{
    int pos = ui->lightsListWidget->currentRow();
    if (pos != -1) {
        Light *l = lightsContext->getLightPtrAt(pos);
        l->setRotateWithCamera(r);
        if (!isAnimated) reRender();
    }
}

void MainWindow::addTransformation()
{
    Transformation *t = new Transformation();
    transformations->append(t);
    ui->transListWidget->setCurrentItem(new QListWidgetItem(t->toString(), ui->transListWidget));
    if (!isAnimated) reRender();
}

void MainWindow::deleteSelectedTransformation()
{
    unsigned pos = ui->transListWidget->currentRow();
    delete (ui->transListWidget->takeItem(pos)); // This fires an event that calls updateTransformationUI().
    transformations->removeAt(pos); //Don't remove the transformation before that event is processed!
    if (!isAnimated) reRender();
}

void MainWindow::readTransformationUI()
{
    int pos = ui->transListWidget->currentRow();
    if (pos != -1) {
        Transformation *t = transformations->at(pos);
        int transformationType = 10 + ui->transTypeComboBox->currentIndex();
        t->setTransformationType(transformationType);
        ui->linearProperties->setVisible(false);
        ui->twistingProperties->setVisible(false);
        ui->bendingProperties->setVisible(false);
        switch (transformationType) {
            case Transformation::TRANSFORMATION_LINEAR: {
                ui->linearProperties->setVisible(true);
                QVector4D a;
                QVector4D b;
                QVector4D c;
                QVector4D d;
                a.setX(ui->transA1->value());
                a.setY(ui->transA2->value());
                a.setZ(ui->transA3->value());
                a.setW(ui->transA4->value());
                b.setX(ui->transB1->value());
                b.setY(ui->transB2->value());
                b.setZ(ui->transB3->value());
                b.setW(ui->transB4->value());
                c.setX(ui->transC1->value());
                c.setY(ui->transC2->value());
                c.setZ(ui->transC3->value());
                c.setW(ui->transC4->value());
                d.setX(ui->transD1->value());
                d.setY(ui->transD2->value());
                d.setZ(ui->transD3->value());
                d.setW(ui->transD4->value());
                QMatrix4x4 *m = t->getMatrix();
                m->setRow(0, a);
                m->setRow(1, b);
                m->setRow(2, c);
                m->setRow(3, d);
                }
                break;
            case Transformation::TRANSFORMATION_TAPPERING:
                break;
            case Transformation::TRANSFORMATION_TWISTING:
                ui->twistingProperties->setVisible(true);
                t->setSpins(ui->twistingSpinsSpinBox->value());
                break;
            case Transformation::TRANSFORMATION_BENDING:
                ui->bendingProperties->setVisible(true);
                t->setRadius(ui->bendingRadiusSpinBox->value());
                break;
        }

        // Common
        t->setTransformCoordinates(ui->transCoordinatesComboBox->currentIndex());
        ui->transListWidget->currentItem()->setText(t->toString());
        if (!isAnimated) reRender();
    }
}

void MainWindow::updateTransformationUI()
{
    int pos = ui->transListWidget->currentRow();
    if (pos == -1) {
        // Set to linear
        ui->transTypeComboBox->setCurrentIndex(0);
        ui->linearProperties->setVisible(true);
        ui->twistingProperties->setVisible(false);
        ui->bendingProperties->setVisible(false);

        // Reset Linear options
        ui->transA1->setValue(0.0);
        ui->transA2->setValue(0.0);
        ui->transA3->setValue(0.0);
        ui->transA4->setValue(0.0);
        ui->transB1->setValue(0.0);
        ui->transB2->setValue(0.0);
        ui->transB3->setValue(0.0);
        ui->transB4->setValue(0.0);
        ui->transC1->setValue(0.0);
        ui->transC2->setValue(0.0);
        ui->transC3->setValue(0.0);
        ui->transC4->setValue(0.0);
        ui->transD1->setValue(0.0);
        ui->transD2->setValue(0.0);
        ui->transD3->setValue(0.0);
        ui->transD4->setValue(0.0);

        // Reset Common options
        ui->transCoordinatesComboBox->setCurrentIndex(0);

    } else {
        ui->linearProperties->setVisible(false);
        ui->twistingProperties->setVisible(false);
        ui->bendingProperties->setVisible(false);
        // Get the transformation
        Transformation *t = transformations->at(pos);
        switch (t->getTransformationType()) {
            case Transformation::TRANSFORMATION_LINEAR: {
                ui->linearProperties->setVisible(true);
                QMatrix4x4 *m = t->getMatrix();
                QVector4D a = m->row(0);
                QVector4D b = m->row(1);
                QVector4D c = m->row(2);
                QVector4D d = m->row(3);
                ui->transA1->setValue(a.x());
                ui->transA2->setValue(a.y());
                ui->transA3->setValue(a.z());
                ui->transA4->setValue(a.w());
                ui->transB1->setValue(b.x());
                ui->transB2->setValue(b.y());
                ui->transB3->setValue(b.z());
                ui->transB4->setValue(b.w());
                ui->transC1->setValue(c.x());
                ui->transC2->setValue(c.y());
                ui->transC3->setValue(c.z());
                ui->transC4->setValue(c.w());
                ui->transD1->setValue(d.x());
                ui->transD2->setValue(d.y());
                ui->transD3->setValue(d.z());
                ui->transD4->setValue(d.w());
                }
                break;
            case Transformation::TRANSFORMATION_TAPPERING:
                break;
            case Transformation::TRANSFORMATION_TWISTING:
                ui->twistingProperties->setVisible(true);
                ui->twistingSpinsSpinBox->setValue(t->getSpins());
                break;
            case Transformation::TRANSFORMATION_BENDING:
                ui->bendingProperties->setVisible(true);
                ui->bendingRadiusSpinBox->setValue(t->getRadius());
                break;
        }
        // Common
        ui->transTypeComboBox->blockSignals(true);
        ui->transTypeComboBox->setCurrentIndex(t->getTransformationType() - 10);
        ui->transTypeComboBox->blockSignals(false);
        ui->transCoordinatesComboBox->blockSignals(true);
        ui->transCoordinatesComboBox->setCurrentIndex(t->getTransformCoordinates());
        ui->transCoordinatesComboBox->blockSignals(false);
    }
    // Disable/enable the whole UI depending on pos == -1
    bool valid = (pos != -1);
    // Common
    ui->transTypeComboBox->setEnabled(valid);
    ui->transDeleteButton->setEnabled(valid);
    ui->transCoordinatesComboBox->setEnabled(valid);
    // Linear
    ui->transA1->setEnabled(valid);
    ui->transA2->setEnabled(valid);
    ui->transA3->setEnabled(valid);
    ui->transA4->setEnabled(valid);
    ui->transB1->setEnabled(valid);
    ui->transB2->setEnabled(valid);
    ui->transB3->setEnabled(valid);
    ui->transB4->setEnabled(valid);
    ui->transC1->setEnabled(valid);
    ui->transC2->setEnabled(valid);
    ui->transC3->setEnabled(valid);
    ui->transC4->setEnabled(valid);
    ui->transD1->setEnabled(valid);
    ui->transD2->setEnabled(valid);
    ui->transD3->setEnabled(valid);
    ui->transD4->setEnabled(valid);
    // Tappering
    // Twisting
    ui->twistingSpinsSpinBox->setEnabled(valid);
    // Bending
    ui->bendingRadiusSpinBox->setEnabled(valid);
}
