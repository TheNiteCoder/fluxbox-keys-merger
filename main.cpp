#include <QCoreApplication>

#include "keysparser.h"
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCommandLineParser>

enum class MergeFlag
{
#define B(i) (1 << i)
    AddDuplicateCommandMappings = B(0)
#undef B
};

using MergeFlags = QFlags<MergeFlag>;

QList<KeysParser::Mapping> merge(QList<KeysParser::Mapping> current, QList<KeysParser::Mapping> additions, MergeFlags flags)
{
    QList<KeysParser::Mapping> result = current;
    for(const auto& mapping : additions)
    {
        if(std::any_of(current.begin(), current.end(), [mapping](const KeysParser::Mapping& m){
                return m.matchesCombo(mapping);
            }))
        {
            continue;
        }
        else if(std::any_of(current.begin(), current.end(), [mapping](const KeysParser::Mapping& m){
                return !m.matchesCombo(mapping) && m.matchesCommand(mapping);
            }))
        {
            if(flags.testFlag(MergeFlag::AddDuplicateCommandMappings))
            {
                result.append(mapping);
            }
            continue;
        }
        else
        {
            result.append(mapping);
        }
    }
    return result;
}

QString getFileContent(QString filename, bool& ok)
{
    QFile file{filename};
    if(!file.open(QFile::ReadOnly))
    {
        ok = false;
        return {};
    }
    QTextStream stream{&file};
    auto content = stream.readAll();
    file.close();
    return content;
}

int main(int argc, char *argv[])
{
    QCoreApplication app{argc, argv};

    QCommandLineParser commandLineParser{};
    commandLineParser.addHelpOption();
    commandLineParser.setApplicationDescription(QObject::tr("This program merges two fluxbox keys file into one"));
    commandLineParser.addPositionalArgument("original", QObject::tr("File to use as a base for merging"));
    commandLineParser.addPositionalArgument("new", QObject::tr("File with new mappings to merge into original"));
    commandLineParser.addPositionalArgument("output", QObject::tr("Output file"));
    commandLineParser.process(app.arguments());

    if(commandLineParser.positionalArguments().length() < 3)
    {
        commandLineParser.showHelp(1);
    }

    KeysParser parser;

    bool ok = true;
    auto current = parser.parse(getFileContent(commandLineParser.positionalArguments().at(0), ok));
    if(!ok)
    {
        qDebug() << QObject::tr("Failed to open original file:") << commandLineParser.positionalArguments().at(0);
        return 2;
    }
    auto adds = parser.parse(getFileContent(commandLineParser.positionalArguments().at(1), ok));
    if(!ok)
    {
        qDebug() << QObject::tr("Failed to open new file:") << commandLineParser.positionalArguments().at(1);
        return 2;
    }

    auto result = merge(adds, current, MergeFlags{});

    QString output;
#define w(s) output.append(s); output.append("\n")
    for(auto m : result)
    {
        for(auto c : m.comments)
        {
            w(c);
        }
        QString combo = m.keyCombo.modifiers.join(' ') + " " + m.keyCombo.key;
        w(combo + " :" + m.command);
    }
#undef w

    QFile file{commandLineParser.positionalArguments().at(2)};
    if(!file.open(QFile::WriteOnly | QFile::Truncate))
    {
        qDebug() << QObject::tr("Failed to open output file:") << commandLineParser.positionalArguments().at(2);
        return 2;
    }
    QTextStream stream{&file};
    stream << output;
    file.close();

    return 0;
}
