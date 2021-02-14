#ifndef CNSDEFINITIONREADER_H
#define CNSDEFINITIONREADER_H

#include "cnsMacroGlobal.h"
#include <QSharedPointer>
#include <QDomElement>
#include "cnsDefaultPropertyParser.h"
#include "cnsComDefinition.h"
#include "cnsdefinitioncreator.h"

CNS_FORWARD_DECL_CLASS(QIODevice);

CNS_FORWARD_DECL_PRIVATE_DATA(CnsDefinitionReader)

class CnsDefaultComFactory;

class CnsDefinitionReader
{
public:
    explicit CnsDefinitionReader(
        CnsDefaultPropertyParserConstPtrRef parser
        , QIODeviceConstPtrRef device);
    explicit CnsDefinitionReader(
        CnsDefaultPropertyParserConstPtrRef parser
        , const QList<QIODevicePtr> &devices);
    virtual ~CnsDefinitionReader();

    CnsDefaultComFactory *registry() const noexcept;

    void readComDefinition(CnsDefaultComFactory *registry) noexcept;

    void doReadComDefinition() noexcept;
    void addCreatorNode(const QString &dirFullPath, CnsFilenameModule * fnModule, const QDomElement &ele, QString comPath="/");

private:
    void readComDefinition(QIODeviceConstPtrRef source) noexcept;
    void readComDefinition(const QString &dirFullPath, const QDomDocument &doc) noexcept;
    void readComDefinition(const QString &dirFullPath, const QDomNodeList &nodes) noexcept;
    void readComDefinition(const QString &comPath, const QDomNodeList &propNodes
         , CnsComDefinitionConstPtrRef def, CnsFilenameModule* fnModule) noexcept;
    void readComDefinitionForProperty(const QString &comPath, const QDomElement &propEle
         , CnsComDefinitionConstPtrRef def, CnsFilenameModule* fnModule) noexcept;

    Qt::ConnectionType connectionTypeStrToEnum(const QString &typeStr) noexcept;

private:
    CNS_DECL_PRIVATE(CnsDefinitionReader)
    CnsDefinitionCreator _creator;
};

CNS_DECL_POINTER(CnsDefinitionReader)

#endif
