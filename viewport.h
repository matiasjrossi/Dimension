#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QWidget>

class QImage;

class Viewport : public QWidget
{
    Q_OBJECT
public:
    Viewport(QWidget *parent=NULL);
    void setImage(QImage *image);
protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
private:
    QImage *image;
    QPoint lastMousePos;
    bool shiftMode;
signals:
    void resized();
    void mouseDrag(double,double);
    void mouseWheel(int);
    void objectShift(double, double);
};

#endif // VIEWPORT_H
