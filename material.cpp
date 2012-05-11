#include "material.h"

Material::Material(const QColor &kA, const QColor &kS, const QColor &kD, const float &q) :
    kA(kA),
    kS(kS),
    kD(kD),
    q(q)
{
}

void Material::setKA(const QColor &kA)
{
    Material::kA = kA;
}

void Material::setKS(const QColor &kS)
{
    Material::kS = kS;
}

void Material::setKD(const QColor &kD)
{
    Material::kD = kD;
}

void Material::setQ(const float &q)
{
    Material::q = q;
}

QColor Material::getKA() const
{
    return kA;
}

QColor Material::getKS() const
{
    return kS;
}

QColor Material::getKD() const
{
    return kD;
}

float Material::getQ() const
{
    return q;
}
