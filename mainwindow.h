#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "transformation.h"

class Viewport;
class Renderer;
class ObjectModel;
class QTimer;
class QPushButton;
class Material;
class LightsContext;
class Vertex;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Viewport *vp;
    Renderer *renderer;
    ObjectModel *om;
    QTimer *timer;
    QPushButton *background;
    Material *objectMaterial;
    LightsContext *lightsContext;
    QList<Transformation*> *transformations;
    QString vertex2String(Vertex v);
    QString matrix2String(QMatrix4x4 *m);
    bool isAnimated;

private slots:
    void on_actionOpen_triggered();
    void on_actionAnimation_toggled(bool);
    void on_actionToggle_wireframe_toggled(bool);
    void reRender();
    void rotate(double x, double y);
    void autoRotate();
    void changeBackgroundColor();
    void addLight();
    void deleteSelectedLight();
    void updateLightButtons();
    void changeLightDiffuse();
    void changeLightSpecular();
    void changeLightAmbient();
    void changeLightPosition();
    void changeLightRotateWithCamera(bool);
    void changeObjectAmbient();
    void changeObjectDiffuse();
    void changeObjectSpecular();
    void changeObjectShininess(double);
    void addTransformation();
    void deleteSelectedTransformation();
    void updateTransformationUI();
    void readTransformationUI();
};

#endif // MAINWINDOW_H
