#include "renderer.h"
#include "objectmodel.h"
#include "lightscontext.h"
#include "material.h"
#include <QtAlgorithms>
#include <QImage>
#include <QPainter>
#include <QLine>
#include <cmath>
#include <cassert>
#include "phongmodel.h"
#include "light.h"

#define SHIFT_MOD 2.0

// z value average
double avZ(Triangle* t)
{
    return (t->a()->z() + t->b()->z() + t->c()->z()) / 3;
}
// painter's algorithm
bool zLess(Triangle* a, Triangle* b)
{
    return (avZ(a) < avZ(b));
}

Renderer::Renderer() :
    rotX(0),
    rotY(0),
    rotZ(0),
    _zoom(0),
    shiftX(0.0),
    shiftY(0.0),
    wireframeVisibility(false),
    backgroundColor(QColor(0, 25, 40)),
    wireframeColor(QColor(255, 255, 0))
{
}

QImage *Renderer::render(ObjectModel *model, QSize size, Material *objectMaterial, LightsContext *lightsContext, QList<Transformation*> *transformations)
{
    ObjectModel *modelAux = new ObjectModel(model);
    LightsContext *lightsAux = new LightsContext(lightsContext);
    // Object transformations
    for (int i=0; i<transformations->size(); i++) {
        Transformation *t = transformations->at(i);
        if (t->getTransformCoordinates() == Transformation::TRANSFORM_OBJECT)
            t->transform(modelAux->getVertexes());
    }
    // Camera transformations
    Transformation rot = buildViewportTransformation();
    rot.transform(modelAux->getVertexes());
    for (unsigned i=0; i<lightsAux->getLightsCount(); i++) {
        Light *l = lightsAux->getLightPtrAt(i);
        if (l->isRotateWithCamera())
            rot.transform(l->getPosPtr());
    }
    for (int i=0; i<transformations->size(); i++) {
        Transformation *t = transformations->at(i);
        if (t->getTransformCoordinates() == Transformation::TRANSFORM_CAMERA)
            t->transform(modelAux->getVertexes());
    }
    sortTrianglesZ(modelAux->getTriangles());
    QImage* result = paint(modelAux->getTriangles(), size, objectMaterial, lightsAux);
    delete modelAux;
    delete lightsAux;
    return result;
}

double modulus(double a, double b)
{
    int result = static_cast<int>( a / b );
    return a - static_cast<double>( result ) * b;
}

void Renderer::rotateX(double r)
{
    setRotationX(rotX + 2*PI*r);
}

void Renderer::rotateY(double r)
{
    setRotationY(rotY + 2*PI*r);
}

void Renderer::rotateZ(double r)
{
    setRotationY(rotY + 2*PI*r);
}

void Renderer::setRotationX(double r)
{
    rotX = r;
}

void Renderer::setRotationY(double r)
{
    rotY = r;
}

void Renderer::setRotationZ(double r)
{
    rotZ = r;
}

void Renderer::sortTrianglesZ(QList<Triangle*> &triangles)
{
    qSort(triangles.begin(), triangles.end(), zLess);
}

Transformation Renderer::buildViewportTransformation()
{
    // Scale object based on _zoom
    double zoomScale = 1.0;
    if (_zoom < 0)
        zoomScale += _zoom / 10.0;
    else
        zoomScale += _zoom;

    QMatrix4x4 scale(zoomScale, 0, 0, 0,
                     0, zoomScale, 0, 0,
                     0, 0, zoomScale, 0,
                     0, 0, 0, zoomScale);

    // Rotate the object from its center
    QMatrix4x4 xRM(1, 0, 0, 0,
                   0, cos(rotX), -sin(rotX), 0,
                   0, sin(rotX), cos(rotX), 0,
                   0, 0, 0, 1);
    QMatrix4x4 yRM(cos(rotY), 0, sin(rotY), 0,
                   0, 1, 0, 0,
                   -sin(rotY), 0, cos(rotY), 0,
                   0, 0, 0, 1);
    QMatrix4x4 zRM(cos(rotZ), -sin(rotZ), 0, 0,
                   sin(rotZ), cos(rotZ), 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1);

    // Shift the object +(shiftX, shiftY, 0)
    QMatrix4x4 shift(1, 0, 0,    shiftX,
                     0, 1, 0, -1*shiftY,
                     0, 0, 1, 0,
                     0, 0, 0, 1);

    Transformation t(Transformation::TRANSFORMATION_LINEAR);
    t.setMatrix(new QMatrix4x4(scale * shift * xRM * yRM * zRM));
    return t;
}

QImage *Renderer::paint(QList<Triangle*> &triangles, QSize size, Material *objectMaterial, LightsContext *lightsContext)
{
    //    qDebug() << "[Renderer::paint] Rendering frame";

    // Prepare output bitmap
    QImage *output = new QImage(size, QImage::Format_RGB32);
    // Paint background
    output->fill(backgroundColor.rgb());

    QRect clippedRect;
    if (size.width() == size.height()) {
        clippedRect.setRect(0, 0, size.width(), size.height());
    } else if (size.width() > size.height()) {
        clippedRect.setRect((size.width()-size.height())/2, 0, size.height(), size.height());
    } else {
        clippedRect.setRect(0, (size.height()-size.width())/2, size.width(), size.width());
    }

    unsigned halfWidth = clippedRect.width()/2, halfHeight = clippedRect.height()/2;

    QPainter painter(output);

    for (int i=0; i<triangles.size(); i++)
    {
        Triangle *current = triangles.at(i);
        QColor color = PhongModel::computeColor(current, lightsContext, objectMaterial);
        painter.setPen(color);
        painter.setBrush(color);
        QPoint points[3];
        points[0] = QPoint(clippedRect.x()+halfWidth+halfWidth*current->a()->x(),clippedRect.y()+halfHeight-halfHeight*current->a()->y());
        points[1] = QPoint(clippedRect.x()+halfWidth+halfWidth*current->b()->x(),clippedRect.y()+halfHeight-halfHeight*current->b()->y());
        points[2] = QPoint(clippedRect.x()+halfWidth+halfWidth*current->c()->x(),clippedRect.y()+halfHeight-halfHeight*current->c()->y());
        painter.drawPolygon(points, 3);
        if (wireframeVisibility == true) {
            painter.setPen(wireframeColor);
            QLine lines[3];
            lines[0] = QLine(points[0], points[1]);
            lines[1] = QLine(points[1], points[2]);
            lines[2] = QLine(points[2], points[0]);
            painter.drawLines(lines, 3);
        }
    }

    return output;
}


void Renderer::changeRotation(double x, double y)
{
    rotateY(x);
    rotateX(y);
}

void Renderer::setWireframeVisibility(bool v)
{
    wireframeVisibility = v;
}

void Renderer::setBackgroundColor(QColor c)
{
    backgroundColor = c;
}

void Renderer::setWireframeColor(QColor c)
{
    wireframeColor = c;
}

QColor Renderer::getBackgroundColor()
{
    return backgroundColor;
}

QColor Renderer::getWireframeColor()
{
    return wireframeColor;
}

void Renderer::setZoom(int zoom)
{
    if (zoom > 9)
        zoom = 9;
    else if (zoom < -9)
        zoom = -9;

    Renderer::_zoom = zoom;
}

void Renderer::zoom(int delta)
{
    setZoom(_zoom + delta);
}

void Renderer::setShiftX(double s)
{
    if (s > SHIFT_MOD) s = SHIFT_MOD;
    else if (s < -SHIFT_MOD) s = -SHIFT_MOD;
    shiftX = s;
}

void Renderer::setShiftY(double s)
{
    if (s > SHIFT_MOD) s = SHIFT_MOD;
    else if (s < -SHIFT_MOD) s = -SHIFT_MOD;
    shiftY = s;
}

void Renderer::shift(double x, double y)
{
    setShiftX(shiftX + x * SHIFT_MOD);
    setShiftY(shiftY + y * SHIFT_MOD);
}
