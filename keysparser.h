#ifndef KEYSPARSER_H
#define KEYSPARSER_H

#include <QFlags>
#include <QString>
#include <QList>
#include <QObject>
#include <QDebug>

class KeysParser
{
public:
    const QStringList possibleModifiers = {
        "control", "shift", "mod1", "mod4", "mod2", "mod3", "lock",
        "ondesktop", "ontoolbar", "onwindow", "ontitlebar", "ontab", "onleftgrip", "onrightgrip", "double",
        "none",
    };

    struct KeyCombo
    {
        QStringList modifiers;
        QString key;
    };

    struct Mapping
    {
        QStringList comments;
        KeyCombo keyCombo;
        QString command;
        bool matchesCombo(const Mapping& other) const;
        bool matchesCommand(const Mapping& other) const;
    };

    QList<Mapping> parse(QString source);
};

QDebug operator<< (QDebug debug, const KeysParser::KeyCombo& mapping);
QDebug operator<< (QDebug debug, const KeysParser::Mapping& mapping);
bool operator== (const KeysParser::KeyCombo& left, const KeysParser::KeyCombo& right);

#endif // KEYSPARSER_H
