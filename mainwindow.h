#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Viewport;
class Renderer;
class ObjectModel;
class QTimer;
class QPushButton;

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
    QPushButton *background, *object, *light;

private slots:
    void on_actionAnimation_toggled(bool );
    void on_action_Open_triggered();
    void reRender();
    void rotate(double x, double y);
    void autoRotate();
    void changeBackgroundColor();
    void changeObjectColor();
    void changeLightColor();
};

#endif // MAINWINDOW_H
