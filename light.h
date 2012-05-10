#ifndef LIGHT_H
#define LIGHT_H

#include <QColor>

class Light
{
public:
    Light();
    Light(QColor const &iS, QColor const &iD);
    Light(Light *l);
    void setIS(QColor const &iS);
    void setID(QColor const &iD);
    QColor getIS() const;
    QColor getID() const;
private:
    QColor iS;
    QColor iD;
};

#endif // LIGHT_H


