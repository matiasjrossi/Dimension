#ifndef LIGHTSCONTEXT_H
#define LIGHTSCONTEXT_H

#include <vector>
class Light;

class LightsContext
{
public:
    LightsContext();
    LightsContext(LightsContext *l);
    unsigned addLight(const Light &);
    void deleteLight(unsigned);
private:
    std::vector<Light*> lights;
};

#endif // LIGHTSCONTEXT_H
