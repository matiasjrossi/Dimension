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
        wireframeVisibility(false),
        backgroundColor(QColor(0, 25, 40)),
        wireframeColor(QColor(255, 255, 0))
{
}

QImage *Renderer::render(ObjectModel *model, QSize size, Material *objectMaterial, LightsContext *lightsContext, QList<Transformation*> *transformations)
{
    ObjectModel *modelAux = new ObjectModel(model);
    LightsContext *lightsAux = new LightsContext(lightsContext);
    Transformation toApply;
    // Object transformations
    for (int i=0; i<transformations->size(); i++) {
        Transformation *t = transformations->at(i);
        if (t->getTransformCoordinates() == Transformation::TRANSFORM_OBJECT)
            toApply *= *t;
    }
    // Camera transformations
    Transformation rot = buildRotation();
    toApply *= rot;
    for (unsigned i=0; i<lightsAux->getLightsCount(); i++) {
        Light *l = lightsAux->getLightPtrAt(i);
        if (l->isRotateWithCamera())
            rot.transform(l->getPosPtr());
    }
    for (int i=0; i<transformations->size(); i++) {
        Transformation *t = transformations->at(i);
        if (t->getTransformCoordinates() == Transformation::TRANSFORM_CAMERA)
            toApply *= *t;
    }
    toApply.transform(modelAux->getVertexes());
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
    rotX += r;
}

void Renderer::rotateY(double r)
{
    rotY += r;
}

void Renderer::rotateZ(double r)
{
    rotZ += r;
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

Transformation Renderer::buildRotation()
{
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

    return Transformation(new QMatrix4x4(xRM * yRM * zRM));
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
