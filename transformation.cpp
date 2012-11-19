#include "transformation.h"
#include <limits>

#define PI 3.14159265359

Transformation::Transformation(int transformationType, int transformCoordinates) :
    transformationType(transformationType),
    transformCoordinates(transformCoordinates),

    matrix(new QMatrix4x4()),

    spins(0.10),

    radius(0.25),

    maxZ(0.0),
    minZ(0.0)

{
}

Transformation::~Transformation()
{
    delete matrix;
}


int Transformation::getTransformCoordinates() const
{
    return transformCoordinates;
}

void Transformation::setTransformCoordinates(int transformCoordinates)
{
    Transformation::transformCoordinates = transformCoordinates;
}

int Transformation::getTransformationType() const
{
    return transformationType;
}

void Transformation::setTransformationType(int transformationType)
{
    switch (transformationType) {
    case TRANSFORMATION_LINEAR:
    case TRANSFORMATION_TWISTING:
    case TRANSFORMATION_TAPPERING:
    case TRANSFORMATION_BENDING:
        Transformation::transformationType = transformationType;
        break;
    default:
        qDebug() << "Unknown transformationType: " << transformationType << ". Ignoring.";
    }
}



void Transformation::transform(Vertex *vertex)
{
    switch(transformationType) {
        case TRANSFORMATION_LINEAR:
            vertex->setQVector((*matrix)*(vertex->toQVector()));
            break;
        case TRANSFORMATION_TWISTING: {
        double f = spins*PI/(maxZ-minZ)*vertex->z();
                QMatrix4x4 m(cos(f), -1*sin(f), 0, 0,
                             sin(f),    cos(f), 0, 0,
                                  0,         0, 1, 0,
                                  0,         0, 0, 1);
                vertex->setQVector(m*(vertex->toQVector()));
            }
            break;
        case TRANSFORMATION_TAPPERING: {
                double f = maxZ - vertex->z();
                QMatrix4x4 m(f, 0, 0, 0,
                             0, f, 0, 0,
                             0, 0, 1, 0,
                             0, 0, 0, 1);
                vertex->setQVector(m*(vertex->toQVector()));
            }
            break;
        case TRANSFORMATION_BENDING: {
                // Avoid division by zero
                double safe_z = vertex->z();
                if (safe_z == 0.0)
                    safe_z = std::numeric_limits<double>::min();
                double a = radius - radius * cos(vertex->z()/radius)/safe_z;
                double b = radius * sin(vertex->z()/radius)/safe_z;
                QMatrix4x4 m(1, 0, a, 0,
                             0, 1, 0, 0,
                             0, 0, b, 0,
                             0, 0, 0, 1);
                vertex->setQVector(m*(vertex->toQVector()));
            }
            break;


    }


}

void Transformation::transform(QList<Vertex *> &vertexes)
{
    minZ = maxZ = vertexes.at(1)->z();
    for (int i = 0; i<vertexes.size(); i++) {
        if (vertexes.at(i)->z() > maxZ)
            maxZ = vertexes.at(i)->z();
        if (vertexes.at(i)->z() < minZ)
            minZ = vertexes.at(i)->z();
    }
    for (int i = 0; i<vertexes.size(); i++)
        transform(vertexes.at(i));
}

QString Transformation::toString()
{
    switch(transformationType) {
        case TRANSFORMATION_LINEAR:
            return QString("(%1, %2, %3, %4)\n(%5, %6, %7, %8)\n(%9, %10, %11, %12)\n(%13, %14, %15, %16)")
                .arg(matrix->row(0).w()).arg(matrix->row(0).x()).arg(matrix->row(0).y()).arg(matrix->row(0).z())
                .arg(matrix->row(1).w()).arg(matrix->row(1).x()).arg(matrix->row(1).y()).arg(matrix->row(1).z())
                .arg(matrix->row(2).w()).arg(matrix->row(2).x()).arg(matrix->row(2).y()).arg(matrix->row(2).z())
                .arg(matrix->row(3).w()).arg(matrix->row(3).x()).arg(matrix->row(3).y()).arg(matrix->row(3).z());
        case TRANSFORMATION_TWISTING:
            return QString("Twisting");
        case TRANSFORMATION_TAPPERING:
            return QString("Tappering");
        case TRANSFORMATION_BENDING:
            return QString("Bending");
        default:
            return QString("Unknown transformation type");
    }
}


// Linear
QMatrix4x4 *Transformation::getMatrix()
{
    return matrix;
}

void Transformation::setMatrix(QMatrix4x4 *matrix)
{
    delete Transformation::matrix;
    Transformation::matrix = matrix;
}

// Twisting
double Transformation::getSpins() const
{
    return spins;
}

void Transformation::setSpins(double spins)
{
    Transformation::spins = spins;
}

// Bending
double Transformation::getRadius() const
{
    return radius;
}

void Transformation::setRadius(double radius)
{
    Transformation::radius = radius;
}

