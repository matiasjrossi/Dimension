#include "lightscontext.h"

#include "light.h"

LightsContext::LightsContext()
{
}

LightsContext::LightsContext(LightsContext *l)
{
    for (unsigned i=0; i<l->lights.size(); i++)
        lights.push_back(new Light(l->lights.at(i)));
}
