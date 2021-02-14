#ifndef CNSDEFAULTCOMFACTORY_H
#define CNSDEFAULTCOMFACTORY_H

#include "cnsMacroGlobal.h"
#include "cnsDefaultPropertyConverter.h"
#include "cnsComDefinition.h"
#include "cnsComReference.h"
#include "cnsGlobal.h"
#include "cnsComponent.h"

CNS_FORWARD_DECL_PRIVATE_DATA(CnsDefaultComFactory);

CNS_FORWARD_DECL_CLASS(CnsDefaultPropertyConverter);

class CnsFilenameModule;

class CnsDefaultComFactory
{
public:
    struct DirEntry{
        QStringList _files;
        QHash<QString, DirEntry> _dirs;
        DirEntry *_parent;
    };
    struct DirTreeEntry
    {
        CnsDefaultComFactory::DirEntry *_dirEntry;
        QString _path;
        DirTreeEntry(CnsDefaultComFactory::DirEntry *dirEntry, QString path):
                _dirEntry(dirEntry), _path(path)
        {
        }
    };
    explicit CnsDefaultComFactory(
            CnsDefaultPropertyConverterConstPtrRef converter);
    ~CnsDefaultComFactory();
    
protected:

    CnsComponentPtr doCreate(const QString *objFullPath,
                             CnsComDefinitionConstPtrRef def) noexcept;
    
public:
    CnsComponentPtr resolveComReferenceToQVariant(CnsComReferenceConstPtrRef comRef) noexcept;
//    void registerNewObjDefinition(const QString *defObjFullPathName,
//            CnsComDefinitionPtr comDefinition) noexcept;
    void registerComDefinition(
            CnsComDefinitionConstPtrRef comDefinition) noexcept;
    void registerCnsObject(
            const QString *fullObjPathName, CnsComponentPtr component) noexcept;
//    bool isContained(const QString &name) noexcept;

    CnsComponentPtr getComObj(const QString *defFullPath, const QString *objFullPath)  noexcept;

    CnsComponentPtr newComObj(const QString &defPath, const QString &objPath)  noexcept;
    CnsComponentPtr getComObjFromPathAbbr(const QString &defPath, const QString &objPath)  noexcept;

    CnsComponentPtr getComObjFromPathAbbr(const QString &objPath)  noexcept;

    QMap<const QString*, CnsComDefinitionPtr>& getComDefinitions() noexcept;

    bool addPropertyValue(CnsComponentPtr com
                     , CnsComDefinitionConstPtrRef comDefinition) noexcept;

    bool addSystemValue(CnsComponentConstPtrRef com
                     , CnsComDefinitionConstPtrRef comDefinition) noexcept;

    void addMethodsValue(CnsComponentConstPtrRef com
                     , CnsComDefinitionConstPtrRef comDefinition) noexcept;

    bool addPaths(CnsComponentConstPtrRef com, const QString *objFullPath
                     , CnsComDefinitionConstPtrRef comDefinition) noexcept;

    CnsComponentPtr findRefObj(const QString *objPath);
    CnsComDefinitionPtr findComDefinition(const QString *defFullPath);
//    CnsComDefinitionPtr findObjDefinition(const QString *objFullPath);
//    CnsComDefinitionPtr findObjComDefinition(const QString *defFullPath);

    QHash<QString, DirEntry> &getDirTree();
    QString getCalleeDirFullPath(const QString &callerDirFullPath, const QString &calleeFilename);
//    const QString* registerFullPathString(QString *fullPath);
    QString* registerFullPathString(const QString &fullPath);
    void registerFilenameModule(const QString &dirFullPath, CnsFilenameModule * fnModule);
    CnsFilenameModule* getFilenameModule(const QString &dirPath);

//    QString getDefFullPath(const QString &dirFullPath, const QString &defPath);
//    QString* convertDefFullPath(const QString &defPath);
    QString getDirFullPathFromPath(const QString &defPath);
    QString* getFullPathFromPathAbbr(const QString &dirFullPath, QString defOrObjPath);
    QString* getFullPathFromPathAbbr(const QString &dirFullPath, const QString *locationFullPath, QString defOrObjPath);
//    QString makeDefFullNsPath(const QString &dirFullPath, const QString &defPath);
    //def = true, obj = false;
    bool makeFullNsPath(QString &out, QString dirFullPath, const QString *locationFullPath, const QString &defOrObjPath);
    bool makeFullNsPathAndFindParent(QStringList &nsnl, const QString &dirFullPath, const QString &comPath);
    bool makeFullNsPathAndFindParent(QString &ns, const QString &name, const QString &dirFullPath, const QString &comPath);
    bool makeFullNsPath(QString &out, QString dirFullPath, const QString &defOrObjPath);
    CnsComponentPtr getComObj(const QString &objFullPath);
    QString *getComDirDefObjPath(const QString &fullPath);
    QString *getObjFullPathFromDefFullPath(const QString &defFullPath, const QString &objPath)  noexcept;
    bool setRootDir(const QString &rootDir);
    const QString &getRootDir();

private:
    CNS_DECL_PRIVATE(CnsDefaultComFactory)
};

class SysDummy : public QObject
{
    Q_OBJECT
public:
    SysDummy(){}
};

extern void registerSystem(
        const QString &name, QObject* system) noexcept;

extern QObject* getSystem(const QString &name) noexcept;

CNS_DECL_POINTER(CnsDefaultComFactory)

#endif
