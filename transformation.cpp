#include "transformation.h"

Transformation::Transformation() :
    m(new QMatrix4x4())
{ 
}

Transformation::Transformation(QMatrix4x4 *matrix) :
    m(matrix)
{
}

Transformation::~Transformation()
{
    delete m;
}

QMatrix4x4 *Transformation::getMatrix() {
    return m;
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
