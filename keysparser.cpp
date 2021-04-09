#include "keysparser.h"

QList<KeysParser::Mapping> KeysParser::parse(QString source)
{
    QStringList currentComments;

    QList<Mapping> result = {};

    for(auto line : source.split('\n'))
    {
        line = line.trimmed();
        if(line.startsWith('#') || line.startsWith('!') || line.startsWith('-'))
        {
            currentComments.append(line);
        }
        else if(line.isEmpty())
        {
            currentComments.append("");
            //currentComments.clear();
        }
        else
        {
            KeyCombo combo;
            while(true)
            {
                line = line.trimmed();
                bool isModifer = false;
                for(const auto& mod : possibleModifiers)
                {
                    if(line.startsWith(mod, Qt::CaseInsensitive))
                        isModifer = true;
                }
                QString string = line.mid(0, line.indexOf(" "));
                line = line.mid(line.indexOf(" "));
                if(isModifer)
                {
                    combo.modifiers.append(string);
                }
                else
                {
                    combo.key = string;
                    break;
                }
            }
            Mapping mapping;
            mapping.comments = currentComments;
            currentComments.clear();
            line = line.mid(line.indexOf(":") + 1);
            mapping.command = line.trimmed();
            mapping.keyCombo = combo;
            result.append(mapping);

        }
    }
    return result;
}

bool operator== (const KeysParser::KeyCombo& left, const KeysParser::KeyCombo& right)
{
    return left.key == right.key && left.modifiers == right.modifiers;
}


bool KeysParser::Mapping::matchesCombo(const KeysParser::Mapping &other) const
{
    return other.keyCombo == keyCombo;
}

bool KeysParser::Mapping::matchesCommand(const KeysParser::Mapping &other) const
{
    return other.command == command;
}

QDebug operator<< (QDebug debug, const KeysParser::KeyCombo& mapping)
{
    QString mods = mapping.modifiers.join(' ');
    debug.nospace().noquote() << mods << " " << mapping.key;
    return debug;
}

QDebug operator<< (QDebug debug, const KeysParser::Mapping& mapping)
{
    debug.nospace().noquote() << mapping.keyCombo << ": " << mapping.command;
    return debug;
}

