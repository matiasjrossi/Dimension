#include "light.h"

Light::Light() :
    iS(),
    iD()
{
}

Light::Light(const QColor &iS, const QColor &iD) :
    iS(iS),
    iD(iD)
{
}

Light::Light(Light *l) :
    iS(l->iS),
    iD(l->iD)
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

QColor Light::getID() const
{
    return iD;
}

QColor Light::getIS() const
{
    return iS;
}
