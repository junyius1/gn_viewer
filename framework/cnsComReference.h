#ifndef CNSCOMREFERENCE_H
#define CNSCOMREFERENCE_H

#include "cnsMacroGlobal.h"
#include <QString>
#include <QSharedPointer>

class CnsDefaultComFactory;

//defPath suport these formats below:
//defPath=filename/namespace/defName objName=nullptr
//defPath=filename//defName objName=nullptr
//defPath=filename/namespace/defName objName=objName
//defPath=filename//defName objName=objName
class CnsComReference : public QObject
{
    Q_OBJECT
public:
    const QString *getObjFullPath(CnsDefaultComFactory *registry) noexcept;
    //objPath suport these formats below:
    //objPath=filename/namespace/defName
    //objPath=filename//defName
    //objPath=//defName
    //objPath=/namespace/defName
    void setObjPath(const QString &objPath) noexcept
    {
        _objPath = objPath;
    }
    void setObjFullPath(const QString *objFullPath) noexcept
    {
        _objFullPath = objFullPath;
    }
    CnsComReference(const QString *dirFullPath):_objFullPath(nullptr),
        _dirFullPath(dirFullPath)
    {
    }
    const QString *getDirFullPath() const noexcept
    { return _dirFullPath; }

    const QString* getParentFullPath() const noexcept
    { return _parentFullPath; }
    void setParentFullPath(const QString *parentFullPath) noexcept
    { _parentFullPath = parentFullPath; }

private:
//    QString _objName;
    const QString *_objFullPath;
    const QString *_dirFullPath;
    const QString *_parentFullPath;
    QString _objPath;

};

CNS_DECL_METATYPE(CnsComReference)

#endif
