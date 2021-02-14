#ifndef CNSDEFAULTPROPERTYPARSER_H
#define CNSDEFAULTPROPERTYPARSER_H

#include "cnsMacroGlobal.h"
#include <QSharedPointer>
#include <QDomElement>
#include "cnsComDefinition.h"
#include "cnscreatecomdefcontext.h"

class CnsFilenameModule;

class CnsDefaultPropertyParser
{
public:
    explicit CnsDefaultPropertyParser();

    QVariant parse(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept;
    
protected:
    QVariant parseValue(const QDomElement &ele) const noexcept;
//    QVariant parseDef(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept;
    QVariant parseList(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept;
    QVariant parseMap(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept;
    QVariant parseEnum(const QDomElement &ele) const noexcept ;
    QVariant parseRef(const QDomElement &ele, CnsComDefinition *def) const noexcept ;
};

CNS_DECL_POINTER(CnsDefaultPropertyParser)

#endif
