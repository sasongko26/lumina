#ifndef LTHEMEENGINE_H
#define LTHEMEENGINE_H

#define LTHEMEENGINE_VERSION_MAJOR 0
#define LTHEMEENGINE_VERSION_MINOR 33

#define LTHEMEENGINE_TOSTRING(s) #s
#define LTHEMEENGINE_STRINGIFY(s)         LTHEMEENGINE_TOSTRING(s)

#define LTHEMEENGINE_VERSION_INT (LTHEMEENGINE_VERSION_MAJOR<<8 | LTHEMEENGINE_VERSION_MINOR)
#define LTHEMEENGINE_VERSION_STR LTHEMEENGINE_STRINGIFY(LTHEMEENGINE_VERSION_MAJOR.LTHEMEENGINE_VERSION_MINOR)

#include <QString>
#include <QStringList>

class lthemeengine
{
public:
    static QString configPath();
    static QString configFile();
    static QStringList iconPaths();
    static QString userStyleSheetPath();
    static QStringList sharedStyleSheetPath();
    static QString userColorSchemePath();
    static QStringList sharedColorSchemePath();
    static QString systemLanguageID();

private:
    lthemeengine() {}
};

#endif // LTHEMEENGINE_H
