#ifndef RENDERER_H
#define RENDERER_H

#include <QList>
#include <QColor>
#include "transformation.h"
#define PI 3.1415926535897932384626433832795028841971

class QImage;
class ObjectModel;
class Triangle;
class Vertex;
class Material;
class LightsContext;

class Renderer
{
public:
    Renderer();
    QImage *render(ObjectModel *model, QSize size, Material *objectMaterial, LightsContext *lightsContext);
    void rotateX(double);
    void rotateY(double);
    void rotateZ(double);
    void setRotationX(double);
    void setRotationY(double);
    void setRotationZ(double);
    void changeRotation(double, double);
    void setWireframeVisibility(bool);
    void setBackgroundColor(QColor);
    void setWireframeColor(QColor);
    QColor getBackgroundColor();
    QColor getWireframeColor();
protected:
    void sortTrianglesZ(QList<Triangle*> &triangles);
    void rotate(QList<Vertex*> &vertex);
    QImage *paint(QList<Triangle*> &triangles, QSize size, Material *objectMaterial, LightsContext *lightsContext);
    double rotX, rotY, rotZ;
    bool wireframeVisibility;
    QColor backgroundColor, wireframeColor;
};

#endif // RENDERER_H
