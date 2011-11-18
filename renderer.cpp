#include "renderer.h"
#include "objectmodel.h"
#include <QtAlgorithms>
#include <QImage>
#include <QPainter>
#include <QLine>
#include <cmath>
#include <cassert>

double avZ(Triangle* t)
{
    return (t->a()->z() + t->b()->z() + t->c()->z()) / 3;
}

bool zLess(Triangle* a, Triangle* b)
{
    return (avZ(a) < avZ(b));
}

Renderer::Renderer() :
        rotX(0),
        rotY(0),
        rotZ(0),
        background(QColor(15,0,20)),
        object(QColor(50,150,150)),
        light(QColor(255,255,200))
{
}

QImage *Renderer::render(ObjectModel *model, QSize size)
{
    ObjectModel *aux = new ObjectModel(model);
    rotate(aux->getVertexes());
    sortTrianglesZ(aux->getTriangles());
    return paint(aux->getTriangles(), size);
    delete aux;
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

void Renderer::rotate(QList<Vertex *> &vertexes)
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

    QMatrix4x4 transformation = xRM * yRM * zRM;

    for (int i=0; i<vertexes.size(); i++)
    {
        vertexes.at(i)->setQVector(transformation.map(vertexes.at(i)->toQVector()));
    }
}

QColor Renderer::castColor(Triangle* t)
{
    double angleCosine = t->normal()*Vertex(0,0,-1);
    if (angleCosine < 0) angleCosine *= -1;
    unsigned short red =   0.6*object.red()   + 0.4*light.red()    *angleCosine;
    unsigned short green = 0.6*object.green() + 0.4*light.green()  *angleCosine;
    unsigned short blue =  0.6*object.blue()  + 0.4*light.blue()   *angleCosine;
    if (red > 255) red = 255;
    if (green > 255) green = 255;
    if (blue > 255) blue = 255;
    return QColor(red, green, blue);
}

QImage *Renderer::paint(QList<Triangle *> &triangles, QSize size)
{
    unsigned halfWidth = size.width()/2, halfHeight = size.height()/2;

    QImage *output = new QImage(size, QImage::Format_RGB32);
    output->fill(background.rgb());
    QPainter painter(output);

    for (int i=0; i<triangles.size(); i++)
    {
        Triangle *current = triangles.at(i);
        QColor color = castColor(current);
        painter.setPen(color);
        painter.setBrush(color);
        QVector<QPoint> points;
        points.push_back(QPoint(halfWidth+halfWidth*current->a()->x(),halfHeight-halfHeight*current->a()->y()));
        points.push_back(QPoint(halfWidth+halfWidth*current->b()->x(),halfHeight-halfHeight*current->b()->y()));
        points.push_back(QPoint(halfWidth+halfWidth*current->c()->x(),halfHeight-halfHeight*current->c()->y()));
        painter.drawPolygon(QPolygon(points));
    }



/*

    painter.setPen(Qt::yellow);
    for (int i=0; i<triangles.size(); i++)
    {
        Triangle *current = triangles.at(i);
        struct { QPoint a, b, c; } points;
        points.a = QPoint(halfWidth+halfWidth*current->a()->x(),halfHeight-halfHeight*current->a()->y());
        points.b = QPoint(halfWidth+halfWidth*current->b()->x(),halfHeight-halfHeight*current->b()->y());
        points.c = QPoint(halfWidth+halfWidth*current->c()->x(),halfHeight-halfHeight*current->c()->y());
        struct { QLine a, b, c; } lines;
        lines.a = QLine(points.a, points.b);
        lines.b = QLine(points.b, points.c);
        lines.c = QLine(points.c, points.a);
        painter.drawLine(lines.a);
        painter.drawLine(lines.b);
        painter.drawLine(lines.c);
    }
*/

    return output;
}

void Renderer::changeRotation(double x, double y)
{
    rotateY(x);
    rotateX(y);
}

void Renderer::setBackgroundColor(QColor c)
{
    background = c;
}

void Renderer::setLightColor(QColor c)
{
    light = c;
}

void Renderer::setObjectColor(QColor c)
{
    object = c;
}

QColor Renderer::getBackgroundColor()
{
    return background;
}

QColor Renderer::getLightColor()
{
    return light;
}

QColor Renderer::getObjectColor()
{
    return object;
}
