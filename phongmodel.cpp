#include "phongmodel.h"

#include "objectmodel.h"
#include "lightscontext.h"
#include "material.h"
#include <QColor>
#include "light.h"
#include <QDebug>

#define NORMALIZED_VIEWER_DIRECTION Vertex(0.0, 0.0, -1.0)

QColor PhongModel::computeColor(Triangle *t, LightsContext *lc, Material *m)
{
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;

    //Ambient light term
    r += (float(lc->getAmbient().red())/255) * (float(m->getKA().red())/255);
    g += (float(lc->getAmbient().green())/255) * (float(m->getKA().green())/255);
    b += (float(lc->getAmbient().blue())/255) * (float(m->getKA().blue())/255);

    //Spot lights
    for (unsigned i=0; i<lc->getLightsCount(); i++) {
        Light l = lc->getLightAt(i);
        Vertex normalizedLightDirection = t->orthoCenter().getDistance(l.getPos()).getNormalized();
        Vertex normalizedNormal = t->normal().getNormalized();
        Vertex normalizedReflectionDirection = normalizedNormal*(2*(normalizedLightDirection*normalizedNormal)) - normalizedLightDirection;

        //Diffuse light term
        float lightIncidence = normalizedLightDirection * normalizedNormal;
        r += (float(m->getKD().red())/255) *
                (float(l.getID().red())/255) *
                lightIncidence;
        g += (float(m->getKD().green())/255) *
                (float(l.getID().green())/255) *
                lightIncidence;
        b += (float(m->getKD().blue())/255) *
                (float(l.getID().blue())/255) *
                lightIncidence;

        float specularIncidence = pow(normalizedReflectionDirection*NORMALIZED_VIEWER_DIRECTION, m->getQ());
        //Specular light term
        r += (float(m->getKS().red())/255) *
                (float(l.getIS().red())/255) *
                specularIncidence;
        g += (float(m->getKS().green())/255) *
                (float(l.getIS().green())/255) *
                specularIncidence;
        b += (float(m->getKS().blue())/255) *
                (float(l.getIS().blue())/255) *
                specularIncidence;

    }

    int _r, _g, _b;
    _r = r*255;
    _g = g*255;
    _b = b*255;
    if (_r>255) _r=255;
    if (_g>255) _g=255;
    if (_b>255) _b=255;
    if (_r<0) _r=0;
    if (_g<0) _g=0;
    if (_b<0) _b=0;

    return QColor(_r, _g, _b);
}
