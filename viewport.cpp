#include "viewport.h"
#include <QPainter>
#include <QImage>
#include <QMouseEvent>

Viewport::Viewport(QWidget *parent) :
    QWidget(parent),
    image(NULL),
    shiftMode(false)
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
    if (e->type() == QMouseEvent::MouseButtonDblClick && e->button() == Qt::LeftButton) {
        shiftMode = true;
        setCursor(Qt::SizeAllCursor);
    } else if (e->type() == QMouseEvent::MouseButtonPress && e->button() == Qt::LeftButton) {
        shiftMode = false;
        setCursor(Qt::ClosedHandCursor);
    }
    lastMousePos = e->pos();

}

void Viewport::mouseReleaseEvent(QMouseEvent *e)
{
    if (!shiftMode)
        if (e->button() == Qt::LeftButton)
            setCursor(Qt::OpenHandCursor);
}

void Viewport::mouseMoveEvent(QMouseEvent *e)
{
    QPoint deltas = e->pos()-lastMousePos;
    if (shiftMode) {
        emit objectShift(static_cast<double>(deltas.x())/width(), static_cast<double>(deltas.y())/height());
    } else {
        if(e->buttons() & Qt::LeftButton)
        {
            emit mouseDrag(static_cast<double>(deltas.x())/width(), static_cast<double>(deltas.y())/height());
        }
    }
    lastMousePos = e->pos();
}

void Viewport::wheelEvent(QWheelEvent *e)
{
    emit mouseWheel(e->delta()/120);
}
