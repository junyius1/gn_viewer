#include "cnsComReference.h"
#include <QDebug>
#include "cnsDefaultComFactory.h"

const QString *CnsComReference::getObjFullPath(CnsDefaultComFactory *registry) noexcept
{
    if(_objFullPath)
    {
        return _objFullPath;
    } else{
        _objFullPath = registry->getFullPathFromPathAbbr(*_dirFullPath, _parentFullPath, _objPath);
        return _objFullPath;
    }
}
