#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H
#include <QList>
#include <QVector4D>
#include <cmath>

class Vertex
{
public:
    Vertex(double x, double y, double z) :
            _x(x),
            _y(y),
            _z(z)
    {};
    Vertex(Vertex *o) :
            _x(o->_x),
            _y(o->_y),
            _z(o->_z)
    {};
    double x() { return _x; };
    double y() { return _y; };
    double z() { return _z; };

    void setX(double x){ _x=x; };
    void setY(double y){ _y=y; };
    void setZ(double z){ _z=z; };

    QVector4D toQVector(){ return QVector4D(_x,_y,_z,0); };
    void setQVector(QVector4D v){ _x=v.x(); _y=v.y(); _z=v.z(); };

    Vertex operator+(Vertex o){return Vertex(_x+o._x,_y+o._y,_z+o._z);};
    Vertex operator/(double r){return Vertex(_x/r, _y/r, _z/r);};

    double operator*(Vertex o){return _x*o._x+_y*o._y+_z*o._z; };
    static Vertex crossProduct(Vertex &a,Vertex &b,Vertex &c){
        double ax = b.x() - a.x(),
        ay = b.y() - a.y(),
        az = b.z() - a.z(),
        bx = c.x() - a.x(),
        by = c.y() - a.y(),
        bz = c.z() - a.z();
        return Vertex(bz*ay-az*by,
                      bx*az-ax*bz,
                      ax*by-bx*ay);
    }
    Vertex getNormalized(){ double norm = sqrt(pow(_x,2)+pow(_y,2)+pow(_z,2)); return Vertex(_x/norm, _y/norm, _z/norm); };

private:
    double _x, _y, _z;
};

class Triangle
{
public:
    Triangle(Vertex *a, Vertex *b, Vertex *c) {
        _a = a;
        _b = b;
        _c = c;
        _normal = NULL;
    };
    ~Triangle(){
        if  (_normal != NULL)
            delete _normal;
    }

    Vertex *a() { return _a; };
    Vertex *b() { return _b; };
    Vertex *c() { return _c; };
    Vertex normal() {
        if (_normal == NULL)
            _normal = new Vertex(Vertex::crossProduct(*_a, *_b, *_c).getNormalized());
        return *_normal;
    };

    Vertex orthoCenter() {
        return (*_a+*_b+*_c)/3;
    }

private:
    Vertex *_a, *_b, *_c, *_normal;
};

class ObjectModel
{
public:
    ObjectModel();
    ObjectModel(ObjectModel *o);
    ~ObjectModel();
    void addVertex(double x, double y, double z);
    void addTriangle(Triangle*);
    void addTriangle(int a, int b, int c);
    QList<Triangle*> &getTriangles();
    QList<Vertex*> &getVertexes();
    void normalize();
private:
    QList<Triangle*> triangles;
    QList<Vertex*> vertexes;
    double minX, maxX, minY, maxY, minZ, maxZ;
};

#endif // OBJECTMODEL_H
