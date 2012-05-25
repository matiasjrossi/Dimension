#include "light.h"

#define RANDCOORD ((rand()%40-20)/10.0)

Light::Light() :
    iS(Qt::white),
    iD(Qt::white),
    pos(new Vertex(RANDCOORD, RANDCOORD, RANDCOORD)),
    rotateWithCamera(false)
{
}

Light::Light(const QColor &iS, const QColor &iD, const Vertex &pos, const bool &rotateWithCamera) :
    iS(iS),
    iD(iD),
    pos(new Vertex(pos)),
    rotateWithCamera(rotateWithCamera)
{
}

Light::Light(Light *l) :
    iS(l->iS),
    iD(l->iD),
    pos(new Vertex(l->pos)),
    rotateWithCamera(l->rotateWithCamera)
{
}

Light::~Light()
{
    delete pos;
}

void Light::setID(const QColor &iD)
{
    Light::iD = iD;
}

void Light::setIS(const QColor &iS)
{
    Light::iS = iS;
}

void Light::setPos(const Vertex &pos)
{
    Vertex *old = Light::pos;
    Light::pos = new Vertex(pos);
    delete old;
}

void Light::setRotateWithCamera(const bool &rotateWithCamera)
{
    Light::rotateWithCamera = rotateWithCamera;
}

QColor Light::getID() const
{
    return iD;
}

QColor Light::getIS() const
{
    return iS;
}

Vertex Light::getPos() const
{
    return *pos;
}

Vertex *Light::getPosPtr()
{
    return pos;
}

bool Light::isRotateWithCamera() const
{
    return rotateWithCamera;
}
