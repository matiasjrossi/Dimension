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
    void mouseMoveEvent(QMouseEvent *);
private:
    QImage *image;
    QPoint lastMousePos;
signals:
    void resized();
    void mouseDrag(double,double);
};

#endif // VIEWPORT_H
