#ifndef PHONGMODEL_H
#define PHONGMODEL_H

class Triangle;
class LightsContext;
class Material;
class QColor;

class PhongModel
{
public:
    static QColor computeColor(Triangle *t, LightsContext *lc, Material *m);
};

#endif // PHONGMODEL_H
