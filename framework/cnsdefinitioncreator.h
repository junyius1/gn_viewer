#ifndef CNSDEFINITIONCREATOR_H
#define CNSDEFINITIONCREATOR_H

#include <QMap>
#include <QDomElement>
#include "cnsDefaultPropertyParser.h"
#include "cnscreatecomdefcontext.h"
#include "cnsComReference.h"

CNS_FORWARD_DECL_PRIVATE_DATA(CnsDefinitionCreator)

class CnsDefinitionCreator
{
public:
    struct CnsDefNode
    {
        QDomElement node;
        QString *relativePath=nullptr;
        ~CnsDefNode()
        {
            delete relativePath;
        }
    };

private:
    QMap<QString, CnsDefNode> _nodes;
public:
    CnsDefinitionCreator(CnsDefaultPropertyParserConstPtrRef parser);
    void setRegistry(CnsDefaultComFactory *registry);
    CnsDefNode& getAddNode(const QString &path);
    virtual ~CnsDefinitionCreator();
    void createComDefs();
    bool createComDefFromPath(const QString &path, CnsCreateComDefContext &context, QString dirFullPath="");
    bool createComDef(const QString &dirFullPath, const QDomElement &node, CnsCreateComDefContext &context, const QString &comPath="");
    bool parseComClass(const QDomElement &ele, CnsComDefinitionConstPtrRef def) noexcept;

    void readComAllDefinitionProperties(const QString &comPath, const QDomNodeList &propNodes
         , CnsComDefinitionConstPtrRef def, CnsCreateComDefContext &context) noexcept;
    void readComDefinitionForProperty(const QString &comPath, const QDomElement &propEle
         , CnsComDefinitionConstPtrRef def, CnsCreateComDefContext &context) noexcept;

//    CnsComDefinitionPtr createObjDef(const QString &comPath, const QDomElement &ele
//         , const QString *dirFullPath, CnsCreateComDefContext &context);
    CnsComDefinitionPtr createComDef(const QString &comPath, const QDomElement &ele
         , const QString *dirFullPath, CnsCreateComDefContext &context);

    CnsDefaultComFactory *registry() const noexcept;
    const QString *getAndRegisterFullPath(const QString &dirFullPath, const QString &comPath, const QDomElement &ele);
    const QString combineRelativePath(const QDomElement &ele);
//    void fixMissingNamespace(CnsComDefinitionPtr def, CnsCreateComDefContext &context);
    void fixMissingSystemName(CnsComDefinitionPtr def, CnsCreateComDefContext &context);

    void processMethodData(CnsComDefinitionConstPtrRef def, CnsCreateComDefContext &context);
    void processChildrenMethodData(CnsComReferencePtr ref, CnsCreateComDefContext &context);
    void processChildrenMethodData(CnsComDefinitionConstPtrRef def, CnsCreateComDefContext &context);
private:
    CNS_DECL_PRIVATE(CnsDefinitionCreator)
};

#endif // CNSDEFINITIONCREATOR_H
