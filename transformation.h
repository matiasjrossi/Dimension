#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <QMatrix4x4>
#include "objectmodel.h"

class Transformation
{
public:
    Transformation();
    Transformation(QMatrix4x4 matrix);
    void transform(Vertex *vertex);
    void transform(QList<Vertex *> &vertexes);
private:
    QMatrix4x4 m;
};

#endif // TRANSFORMATION_H
