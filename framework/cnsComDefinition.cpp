#include "cnsComDefinition.h"
#include <QDebug>
#include "cnsDefaultComFactory.h"
#include "cnsDefinitionReader.h"

const QString *CnsComDefinition::getInheritFullPath(CnsDefaultComFactory *registry) noexcept
{
    if(_inheritFullPath)
    {
        return _inheritFullPath;
    } else{
        _inheritFullPath = registry->getFullPathFromPathAbbr(*_dirFullPath, _inheritPath);
        return _inheritFullPath;
    }
}
const QString *CnsComDefinition::getDefFullPath(CnsDefaultComFactory *registry) noexcept
{
    if(_defFullPath)
    {
        return _defFullPath;
    } else{
        _defFullPath = registry->getFullPathFromPathAbbr(*_dirFullPath, DIR_NSN_SEPARATOR+_sorts[0]+'/'+_name);
        return _defFullPath;
    }
}

//enum HasMethodType{
//    UNINIT=-3,
//    HAS_NONE=-2,
//    HAS_BUT_ONLY_CHILD=-1,
//    HAS_ALL=-4,
//};
void CnsComDefinition::mergeMethodDataFromDef(const CnsComDefinition* def)
{
    const METHODDATAHASH& methodData = def->getMethodDataHash();
    for(METHODDATAHASH::const_iterator it = methodData.begin(); it != methodData.end(); it++)
    {
        const QByteArray *name = it.key();
        METHODDATAHASH::iterator jt = _methodHash.find(name);
        if(jt == _methodHash.end()){
            METHODDATAHASH::iterator kt = _methodHash.insert(name, it.value());
//            assert(kt->stateIndex != HasMethodType::UNINIT);
            kt->stateIndex = HAS_BUT_ONLY_CHILD;
        } else{
            if(jt->stateIndex >=0){
                jt->stateIndex = -jt->stateIndex+HAS_ALL;
            }
        }
    }
}
