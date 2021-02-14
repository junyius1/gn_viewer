#ifndef CNSCOMDEFINITION_H
#define CNSCOMDEFINITION_H

#include "cnsMacroGlobal.h"
#include "cnsGlobal.h"
#include <QVariant>
class CnsDefaultComFactory;

class CnsComDefinition
{
public:
    CnsComDefinition(): _dirFullPath(nullptr)
      , _defFullPath(nullptr), _inheritFullPath(nullptr){
    }
    CnsComDefinition(const QString *dirFullPath): _dirFullPath(dirFullPath)
      , _defFullPath(nullptr), _inheritFullPath(nullptr)
    {}

    QString getName() const noexcept
    { return _name; }
    void setName(const QString &name) noexcept
    {
        _name = name;
    }

    bool isCreate() const noexcept
    { return _create; }
    void setCreate(const bool &create) noexcept
    {
        _create = create;
    }

    inline QString getSystemName() const noexcept
    { return _systemName; }
    void setSystemName(const QString &systemName) noexcept
    { _systemName = systemName; }

    inline const QStringList& getSorts() const noexcept
    { return _sorts; }
    inline void setSorts(const QStringList &sorts) noexcept
    { _sorts = sorts; }

    const QString* getDefFullPath() const noexcept
    { return _defFullPath; }
    void setDefFullPath(const QString *defFullPath) noexcept
    { _defFullPath = defFullPath; }

    const QString* getDirFullPath() const noexcept
    { return _dirFullPath; }
    void setDirFullPath(const QString *dirFullPath) noexcept
    { _dirFullPath = dirFullPath; }

    QVariantMap &getProperties() noexcept
    { return _properties; }
    const QVariantMap &getConstProperties() const noexcept
    { return _properties; }
    void addProperty(const QString &name, const QVariant &value) noexcept
    { _properties.insert(name, value); }
    
    QVariantList getChildren() const noexcept
    { return _children; }
    void addChildren(const QVariant &val) noexcept
    { _children.append(val); }

    //defPath suport these formats below:
    //defPath=filename/namespace/defName
    //defPath=filename//defName
    void setInheritPath(const QString &inheritPath)
    {
        _inheritPath = inheritPath;
    }

    void setInheritFullPath(const QString *inheritFullPath)
    {
        _inheritFullPath = inheritFullPath;
    }

    const QString& getInheritPath()
    {
        return _inheritPath;
    }

    const QString *getInheritFullPath(CnsDefaultComFactory *registry) noexcept;

    QString getClassName() const noexcept
    { return _className; }
    void setClassName(const QString &className) noexcept
    { _className = className; }

    const METHODDATAHASH& getMethodDataHash() const noexcept
    { return _methodHash; }
    inline void initMyMethodData(const QByteArray *signure, const MethodData& methodData) noexcept
    {
        _methodHash[signure] = methodData;
    }
    void mergeMethodDataFromDef(const CnsComDefinition* def);
    const QString *getDefFullPath(CnsDefaultComFactory *registry) noexcept;
    inline const QStringList &getNsN() const noexcept
    { return _nsn; }
    inline void setNsN(const QStringList &nsn) noexcept
    { _nsn = nsn; }
//    inline const QStringList &getNss() const noexcept
//    { return _nss; }
//    inline void setNss(const QStringList &nss) noexcept
//    { _nss = nss; }

protected:
    QString _name;
    QString _systemName;
    QVariantMap _properties;
    QVariantList _children;
    QStringList _sorts;
    QString _inheritPath;
    QString _className;
    const QString *_dirFullPath;
    const QString *_defFullPath;
    const QString *_inheritFullPath;
    bool _create = true;
    METHODDATAHASH _methodHash;
    QStringList _nsn;
//    QStringList _nss;
};

CNS_DECL_POINTER(CnsComDefinition)

#endif
