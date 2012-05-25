#ifndef LIGHT_H
#define LIGHT_H

#include <QColor>
#include "objectmodel.h"

class Light
{
public:
    Light();
    Light(QColor const &iS, QColor const &iD, const Vertex &pos, const bool &rotateWithCamera);
    Light(Light *l);
    ~Light();
    void setIS(const QColor &iS);
    void setID(const QColor &iD);
    void setPos(const Vertex &pos);
    void setRotateWithCamera(const bool &rotateWithCamera);
    QColor getIS() const;
    QColor getID() const;
    Vertex getPos() const;
    Vertex *getPosPtr();
    bool isRotateWithCamera() const;
private:
    QColor iS;
    QColor iD;
    Vertex *pos;
    bool rotateWithCamera;
};

#endif // LIGHT_H


