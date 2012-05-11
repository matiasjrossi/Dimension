#include "lightscontext.h"

#include "light.h"

LightsContext::LightsContext() :
    ambient(Qt::white)
{
}

LightsContext::LightsContext(LightsContext *l) :
    ambient(l->ambient)
{
    for (unsigned i=0; i<l->lights.size(); i++)
        lights.push_back(new Light(l->lights.at(i)));
}

void LightsContext::setAmbient(QColor c)
{
    ambient = c;
}

QColor LightsContext::getAmbient() const
{
    return ambient;
}

unsigned LightsContext::addLight(Light *l)
{
    lights.push_back(l);
    return lights.size() - 1;
}

void LightsContext::deleteLight(unsigned i)
{
    lights.erase(lights.begin()+i);
}

Light LightsContext::getLightAt(unsigned pos) const
{
    return Light(lights.at(pos));
}

Light *LightsContext::getLightPtrAt(unsigned pos)
{
    return lights.at(pos);
}
 unsigned LightsContext::getLightsCount() const
 {
     return lights.size();
 }
