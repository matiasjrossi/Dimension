#ifndef LIGHT_H
#define LIGHT_H

#include <QColor>
#include "objectmodel.h"

class Light
{
public:
    Light();
    Light(QColor const &iS, QColor const &iD, const Vertex &pos);
    Light(Light *l);
    void setIS(const QColor &iS);
    void setID(const QColor &iD);
    void setPos(const Vertex &pos);
    QColor getIS() const;
    QColor getID() const;
    Vertex getPos() const;
private:
    QColor iS;
    QColor iD;
    Vertex pos;
};

#endif // LIGHT_H


