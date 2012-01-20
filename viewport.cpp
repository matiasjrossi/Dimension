#include "viewport.h"
#include <QPainter>
#include <QImage>
#include <QMouseEvent>

Viewport::Viewport(QWidget *parent) :
        QWidget(parent),
        image(NULL)
{
    setMouseTracking(true);
    setCursor(Qt::OpenHandCursor);
}

void Viewport::setImage(QImage *image)
{
    if (this->image != NULL) {
        delete this->image;
    }
    this->image = image;
    update();
}

void Viewport::paintEvent(QPaintEvent *)
{
    if (image != NULL) {
        QPainter painter(this);
        painter.drawImage(QPoint(0,0),*image);
    }
}

void Viewport::resizeEvent(QResizeEvent *)
{
    emit resized();
}

void Viewport::mousePressEvent(QMouseEvent *e)
{
    lastMousePos = e->pos();
}

void Viewport::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        QPoint deltas = lastMousePos-e->pos();
        emit mouseDrag(static_cast<double>(deltas.x())/width(), static_cast<double>(-1*deltas.y())/height());
        lastMousePos = e->pos();
    }
}
