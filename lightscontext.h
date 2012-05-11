#ifndef LIGHTSCONTEXT_H
#define LIGHTSCONTEXT_H

#include <vector>
#include <QColor>
class Light;

class LightsContext
{
public:
    LightsContext();
    LightsContext(LightsContext *l);
    void setAmbient(QColor);
    QColor getAmbient() const;
    unsigned addLight(Light *);
    void deleteLight(unsigned);
    Light getLightAt(unsigned) const;
    Light *getLightPtrAt(unsigned);
    unsigned getLightsCount() const;
private:
    std::vector<Light*> lights;
    QColor ambient;
};

#endif // LIGHTSCONTEXT_H
