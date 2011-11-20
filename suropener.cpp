#include "suropener.h"
#include "objectmodel.h"
#include <QFile>
#include <QDebug>

ObjectModel *SUROpener::openSUR(QString fileName)
{
    Parser parser(fileName);
    ObjectModel *om = new ObjectModel();
    for (int i=0; i<parser.getCoordinates().size(); i++)
    {
        Parser::Coordinate c = parser.getCoordinates().at(i);
        om->addVertex(c.x, c.y, c.z);
    }
    for (int i=0; i<parser.getIncidences().size(); i++)
    {
        Parser::Incidence in= parser.getIncidences().at(i);
        om->addTriangle(in.a-1, in.b-1, in.c-1);
    }
    om->normalize();
    return om;
}

SUROpener::Parser::Parser(QString fileName) :
        groupCount(-1),
        coordinateCount(-1)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
            parseLine(file.readLine());
        file.close();
    }
}

QList<SUROpener::Parser::Group> &SUROpener::Parser::getGroups()
{
    return this->groups;
}

QList<SUROpener::Parser::Incidence> &SUROpener::Parser::getIncidences()
{
    return this->incidences;
}

QList<SUROpener::Parser::Coordinate> &SUROpener::Parser::getCoordinates()
{
    return this->coordinates;
}

void SUROpener::Parser::parseLine(QString line)
{
    removeUselessBlanks(line);
    if (line.size() == 0) return;
    if (header(line)) parseHeader(line);
    else dispatch(line);
}

void SUROpener::Parser::removeUselessBlanks(QString &line)
{
    //Remove leading blanks
    while (line.size() !=0 && (line.startsWith(' ') || line.startsWith('\n')))
        line.remove(0,1);

    //Remove tail
    while (line.size() != 0 && (line.endsWith(' ') || line.endsWith('\n')))
        line.remove(line.size()-1, 1);
}

bool SUROpener::Parser::header(QString line)
{
    return (line.at(0) == '*');
}

void SUROpener::Parser::parseHeader(QString line)
{
    if (line.startsWith("*ELEMENT GROUPS", Qt::CaseInsensitive))
        state = GROUPS;
    else if (line.startsWith("*COORDINATES", Qt::CaseInsensitive))
        state = COORDINATES;
    else if (line.startsWith("*INCIDENCE", Qt::CaseInsensitive))
        state = INCIDENCE;
    else if (line.startsWith("*FRONT_VOLUME", Qt::CaseInsensitive))
        state = VOLUME;
    else
        ignoreLine(line);
}

void SUROpener::Parser::ignoreLine(QString line)
{
    qDebug() << QString("[SUROpener::Parser] Ignoring line: ") << line;
}

void SUROpener::Parser::dispatch(QString line)
{
    switch (state)
    {
    case GROUPS:
        parseGroup(line);
        break;
    case INCIDENCE:
        parseIncidence(line);
        break;
    case COORDINATES:
        parseCoordinate(line);
        break;
    case VOLUME:
        ignoreLine(line);
        break;
    default:
        ignoreLine(line);
    }
}

void SUROpener::Parser::parseGroup(QString line)
{
    LineReader lr(line);
    /*
     * The first line of the "ELEMENT GROUPS" block indicates how many
     * groups are contained within the section
     */
    if (groupCount == -1)
    {
        if (lr.getWords().size() < 1) {
            qDebug() << "[SUROpener::Parser::parseGroup] ERROR: Ignoring malformed group count line:" << line;
        } else {
            if (lr.getWords().size() > 1) {
                qDebug() << "[SUROpener::Parser::parseGroup] WARNING: Malformed group count line:" << line << "- Discarding extra values.";
            }
            groupCount = lr.getWords().at(0).toInt();
        }
    }
    else
    {
        if (lr.getWords().size() < 3) {
            qDebug() << "[SUROpener::Parser::parseGroup] ERROR: Ignoring malformed group line:" << line;
        } else {
            if (lr.getWords().size() > 3) {
                qDebug() << "[SUROpener::Parser::parseGroup] WARNING: Malformed group line:" << line << "- Discarding extra values.";
            }
            Group g;
            g.first = lr.getWords().at(0).toInt();
            g.last = lr.getWords().at(1).toInt();
            groups.push_back(g);
        }
    }
}

void SUROpener::Parser::parseIncidence(QString line)
{
    LineReader lr(line);
    if (lr.getWords().size() < 3) {
        qDebug() << "[SUROpener::Parser::parseIncidence] ERROR: Ignoring malformed incidence line:" << line;
    } else {
        if (lr.getWords().size() > 3) {
            qDebug() << "[SUROpener::Parser::parseIncidence] WARNING: Malformed incidence line:" << line << "- Discarding extra values.";
        }
        Incidence i;
        i.a = lr.getWords().at(0).toInt();
        i.b = lr.getWords().at(1).toInt();
        i.c = lr.getWords().at(2).toInt();
        incidences.push_back(i);
    }
}

void SUROpener::Parser::parseCoordinate(QString line)
{
    LineReader lr(line);
    /*
     * The first line of the "COORDINATES" block indicates how many
     * coordinates are contained within the section
     */
    if (coordinateCount == -1)
    {
        if (lr.getWords().size() < 1) {
            qDebug() << QString("[SUROpener::Parser::parseCoordinate] ERROR: Ignoring malformed coordinate count line: ") << line;
        } else {
            if (lr.getWords().size() > 1) {
                qDebug() << QString("[SUROpener::Parser::parseCoordinate] WARNING: Malformed coordinate count line: ") << line << QString(" - Discarding extra values.");
            }
            coordinateCount = lr.getWords().at(0).toInt();
        }
    }
    else
    {
        if (lr.getWords().size() < 4) {
            qDebug() << QString("[SUROpener::Parser::parseCoordinate] ERROR: Ignoring malformed coordinate line: ") << line;
        } else {
            if (lr.getWords().size() > 4) {
                qDebug() << QString("[SUROpener::Parser::parseCoordinate] WARNING: Malformed coordinate line: ") << line << QString(" - Discarding extra values.");
            }
            Coordinate c;
            c.x = lr.getWords().at(1).toDouble();
            c.y = lr.getWords().at(2).toDouble();
            c.z = lr.getWords().at(3).toDouble();
            coordinates.push_back(c);
        }
    }
}

SUROpener::Parser::LineReader::LineReader(QString line)
{
    int currentPosition = 0;
    while(currentPosition < line.length())
    {
        int wordSize = line.indexOf(' ', currentPosition) - currentPosition;
        if (wordSize < 0) wordSize = line.size() - currentPosition;
        words.push_back(line.mid(currentPosition, wordSize));
        currentPosition += wordSize + 1;
    }
}

QList<QString> SUROpener::Parser::LineReader::getWords()
{
    return words;
}
