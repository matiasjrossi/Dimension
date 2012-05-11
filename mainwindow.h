#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    QString vertex2String(Vertex v);
//    QString qColor2String(QColor c);
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
    void changeObjectAmbient();
    void changeObjectDiffuse();
    void changeObjectSpecular();
    void changeObjectShininess(double);
};

#endif // MAINWINDOW_H
