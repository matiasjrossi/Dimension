#include "light.h"

#define RANDCOORD ((rand()%40-20)/10.0)

Light::Light() :
    iS(Qt::white),
    iD(Qt::white),
    pos(Vertex(RANDCOORD, RANDCOORD, RANDCOORD))
{
}

Light::Light(const QColor &iS, const QColor &iD, const Vertex &pos) :
    iS(iS),
    iD(iD),
    pos(pos)
{
}

Light::Light(Light *l) :
    iS(l->iS),
    iD(l->iD),
    pos(l->pos)
{
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
    Light::pos = pos;
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
    return pos;
}
