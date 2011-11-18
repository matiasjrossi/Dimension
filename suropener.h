#ifndef SUROPENER_H
#define SUROPENER_H
#include <QString>
#include <QList>
#include <QVector>

class ObjectModel;

class SUROpener
{
public:
    static ObjectModel *openSUR(QString fileName);
private:
    class Parser
    {
    public:
        Parser(QString fileName);
        typedef struct {int first, last;} Group;
        typedef struct {int a, b, c;} Incidence;
        typedef struct {double x, y, z;} Coordinate;
        QList<Group> &getGroups();
        QList<Incidence> &getIncidences();
        QList<Coordinate> &getCoordinates();
    private:
        class LineReader
        {
        public:
            LineReader(QString);
            QList<QString> getWords();
        private:
            QList<QString> words;
        };

        typedef enum {INITIAL = 0, GROUPS, INCIDENCE, COORDINATES, VOLUME} State;
        Parser::State state;
        QList<Group> groups;
        int groupCount;
        QList<Incidence> incidences;
        QList<Coordinate> coordinates;
        int coordinateCount;

        void parseLine(QString);
        void parseHeader(QString);
        bool header(QString);
        void removeUselessBlanks(QString &);
        void dispatch(QString);
        void parseGroup(QString);
        void parseIncidence(QString);
        void parseCoordinate(QString);
        void ignoreLine(QString);
    };
};

#endif // SUROPENER_H
