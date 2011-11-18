#include "objectmodel.h"

#include <QDebug>
#include <QString>
#include <map>

ObjectModel::ObjectModel()
{
}

ObjectModel::ObjectModel(ObjectModel *o)
{
    std::map<Vertex*,Vertex*> vMap;
    for (int i=0; i<o->vertexes.size(); i++)
    {
        Vertex *newVertex = new Vertex(o->vertexes.at(i));
        vertexes.push_back(newVertex);
        vMap[o->vertexes.at(i)] = newVertex;
    }
    for (int i=0; i<o->triangles.size(); i++)
    {
        Triangle *oldTriangle = o->triangles.at(i);
        triangles.push_back(new Triangle(vMap[oldTriangle->a()], vMap[oldTriangle->b()], vMap[oldTriangle->c()]));
    }
}

ObjectModel::~ObjectModel()
{
    while (!triangles.empty()) {
        delete triangles.front();
        triangles.pop_front();
    }

    while (!vertexes.empty()) {
        delete vertexes.front();
        vertexes.pop_front();
    }
}

double min(double a, double b)
{
    return (a<b?a:b);
}

double max(double a, double b)
{
    return (a>b?a:b);
}

void ObjectModel::addVertex(double x, double y, double z)
{
    if (vertexes.empty())
    {
        minX = maxX = x;
        minY = maxY = y;
        minZ = maxZ = z;
    }
    else
    {
        minX = min(minX,x);
        minY = min(minY,y);
        minZ = min(minZ,z);
        maxX = max(maxX,x);
        maxY = max(maxY,y);
        maxZ = max(maxZ,z);
    }
    vertexes.push_back(new Vertex(x, y, z));
}

void ObjectModel::addTriangle(Triangle *t)
{
    triangles.push_back(t);
}

void ObjectModel::addTriangle(int a, int b, int c)
{
    triangles.push_back(new Triangle(vertexes.at(a), vertexes.at(b), vertexes.at(c)));
}

QList<Triangle*> &ObjectModel::getTriangles()
{
    return triangles;
}

QList<Vertex*> &ObjectModel::getVertexes()
{
    return vertexes;
}

double max(double a, double b, double c)
{
    double temp = max(a,b);
    return max(temp,c);
}

void ObjectModel::normalize()
{
    double scaleX = maxX-minX, scaleY = maxY-minY, scaleZ = maxZ-minZ;
    double scale = max(scaleX, scaleY, scaleZ);
    double halfScale = scale / 2;
    double xOffset = minX + scaleX/2, yOffset = minY + scaleY/2, zOffset = minZ + scaleZ/2;

    for (int i=0; i<vertexes.size(); i++)
    {
        vertexes.at(i)->setX((vertexes.at(i)->x() - xOffset)/halfScale);
        vertexes.at(i)->setY((vertexes.at(i)->y() - yOffset)/halfScale);
        vertexes.at(i)->setZ((vertexes.at(i)->z() - zOffset)/halfScale);
    }
}
