#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <QMatrix4x4>
#include "objectmodel.h"

class Transformation
{
public:
    static const int TRANSFORM_OBJECT = 0;
    static const int TRANSFORM_CAMERA = 1;
    Transformation();
    Transformation(QMatrix4x4 *matrix, int transformCoordinates = TRANSFORM_OBJECT);
    ~Transformation();
    QMatrix4x4 *getMatrix();
    int getTransformCoordinates() const;
    void setTransformCoordinates(int transformCoordinates);
    void transform(Vertex *vertex);
    void transform(QList<Vertex *> &vertexes);
private:
    QMatrix4x4 *m;
    int transformCoordinates;
};

#endif // TRANSFORMATION_H
