#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <QMatrix4x4>
#include "objectmodel.h"

class Transformation
{
public:
    static const int TRANSFORM_OBJECT = 0;
    static const int TRANSFORM_CAMERA = 1;
    static const int TRANSFORMATION_LINEAR = 10;
    static const int TRANSFORMATION_TWISTING = 11;
    static const int TRANSFORMATION_TAPPERING = 12;
    static const int TRANSFORMATION_BENDING = 13;

    Transformation(int transformationType = TRANSFORMATION_LINEAR, int transformCoordinates = TRANSFORM_OBJECT);
    ~Transformation();

    int getTransformCoordinates() const;
    void setTransformCoordinates(int transformCoordinates);
    int getTransformationType() const;
    void setTransformationType(int);

    void transform(Vertex *vertex);
    void transform(QList<Vertex *> &vertexes);
    QString toString();

    // Linear
    QMatrix4x4 *getMatrix();
    void setMatrix(QMatrix4x4*);

    // Twisting
    double getSpins() const;
    void setSpins(double);

    // Bending
    double getRadius() const;
    void setRadius(double);

private:
    int transformationType;
    int transformCoordinates;

    // Linear
    QMatrix4x4 *matrix;

    // Twisting
    double spins;

    // Bending
    double radius;

    double maxZ;
    double minZ;

};

#endif // TRANSFORMATION_H
