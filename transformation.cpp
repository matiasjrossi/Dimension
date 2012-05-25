#include "transformation.h"

Transformation::Transformation() :
    m()
{ 
}

Transformation::Transformation(QMatrix4x4 matrix) :
    m(matrix)
{
}

void Transformation::transform(QList<Vertex *> &vertexes)
{
    for (int i = 0; i<vertexes.size(); i++)
        transform(vertexes.at(i));
}

void Transformation::transform(Vertex *vertex)
{
    vertex->setQVector(m.map(vertex->toQVector()));
}
