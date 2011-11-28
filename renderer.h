#ifndef RENDERER_H
#define RENDERER_H

#include <QList>
#include <QMatrix4x4>
#include <QColor>
#define PI 3.1415926535897932384626433832795028841971

class QImage;
class ObjectModel;
class Triangle;
class Vertex;

class Renderer
{
public:
    Renderer();
    QImage *render(ObjectModel *model, QSize size);
    void rotateX(double);
    void rotateY(double);
    void rotateZ(double);
    void setRotationX(double);
    void setRotationY(double);
    void setRotationZ(double);
    void changeRotation(double, double);
    void setWireframeVisibility(bool);
    void setBackgroundColor(QColor);
    void setObjectColor(QColor);
    void setWireframeColor(QColor);
    void setLightColor(QColor);
    QColor getBackgroundColor();
    QColor getObjectColor();
    QColor getWireframeColor();
    QColor getLightColor();
protected:
    QColor castColor(Triangle*);
    void sortTrianglesZ(QList<Triangle*> &triangles);
    void rotate(QList<Vertex*> &vertex);
    QImage *paint(QList<Triangle*> &triangles, QSize size);
    double rotX, rotY, rotZ;
    bool wireframeVisibility;
    QColor backgroundColor, objectColor, wireframeColor, lightColor;
};

#endif // RENDERER_H
