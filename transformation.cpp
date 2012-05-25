#include "transformation.h"

Transformation::Transformation() :
    m(new QMatrix4x4()),
    transformCoordinates(TRANSFORM_OBJECT)
{ 
}

Transformation::Transformation(QMatrix4x4 *matrix, int transformCoordinates) :
    m(matrix),
    transformCoordinates(transformCoordinates)
{
}

Transformation::~Transformation()
{
    delete m;
}

QMatrix4x4 *Transformation::getMatrix()
{
    return m;
}

int Transformation::getTransformCoordinates() const
{
    return transformCoordinates;
}

void Transformation::setTransformCoordinates(int transformCoordinates)
{
    Transformation::transformCoordinates = transformCoordinates;
}

void Transformation::transform(QList<Vertex *> &vertexes)
{
    for (int i = 0; i<vertexes.size(); i++)
        transform(vertexes.at(i));
}

void Transformation::transform(Vertex *vertex)
{
    vertex->setQVector(m->map(vertex->toQVector()));
}
