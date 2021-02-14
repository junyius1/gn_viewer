#include <QPluginLoader>
#include <QMetaObject>
#include <QMetaProperty>
#include <QDebug>

#include "cnsDefaultComFactory.h"
#include "cnsComDefinition.h"
#include "cnsGlobal.h"
#include "cnsComponent.h"
#include "cnsDefinitionReader.h"
#include <QRegularExpressionMatchIterator>

CNS_INIT(CnsComReference)
CNS_REGISTER_COM_FACTORY(CnsComReference)
CNS_INIT_END
CNS_DECL_PRIVATE_DATA(CnsDefaultComFactory)
////full dir name + def name to def full path
//QHash<QString, QList<CnsComDefinitionConstPtrRef>> fdndn2DefFullPathHash;
////full dir name + obj name to obj full path
//QHash<QString, QList<CnsComponentPtr>> fdnon2ObjFullPathHash;
//def full path
QMap<const QString*, CnsComDefinitionPtr> comDefHash;
//obj full path to CnsComDefinitionPtr
//QHash<const QString*, CnsComDefinitionPtr> comNewObjDefHash;
//obj full path
QHash<const QString*, CnsComponentPtr> comObjHash;
//system name
QString _rootDir;
////dir path to filename to dir full path record,
////generate a filename to dir full path record when the filename is used
//QHash<QString, QHash<QString, QString>> dir2Filename2AliasFullPathHash;
//dir full path to CnsFilenameModule
QHash<QString, CnsFilenameModule*> dirFullPath2FnModuleHash;
//calculate dirFullPath2FnModuleHash from cnsDirTree
QHash<QString, CnsDefaultComFactory::DirEntry> cnsDirTree;
CnsDefaultPropertyConverterPtr converter;
QHash<QString, QString *> _comDirDefObjPaths;
CNS_DECL_PRIVATE_DATA_END


//CnsComDefinitionPtr CnsDefaultComFactory::findObjDefinition(const QString *objFullPath)
//{
//    QHash<const QString*, CnsComDefinitionPtr>::iterator it = d->comNewObjDefHash.find(objFullPath);
//    if(it == d->comNewObjDefHash.end())
//    {
//        return nullptr;
//    }
//    return it.value();
//}

const QString &CnsDefaultComFactory::getRootDir()
{
    return d->_rootDir;
}

bool CnsDefaultComFactory::setRootDir(const QString &rootDir)
{
    if(rootDir.isEmpty())
        return false;
    else{
        d->_rootDir = rootDir;
        if(d->_rootDir.endsWith('/'))
        {
            d->_rootDir.chop(1);
        }

        return true;
    }
}

QString *CnsDefaultComFactory::getComDirDefObjPath(const QString &fullPath)
{
    QHash<QString, QString *>::iterator it = d->_comDirDefObjPaths.find(fullPath);
    return it == d->_comDirDefObjPaths.end()? nullptr:it.value();
}

CnsComponentPtr CnsDefaultComFactory::getComObj(const QString &objFullPath)
{
    QString * p = getComDirDefObjPath(objFullPath);
    if(p){
        QHash<const QString*, CnsComponentPtr>::iterator it = d->comObjHash.find(p);
        return it == d->comObjHash.end()?nullptr:it.value();
    }
    return nullptr;
}

bool CnsDefaultComFactory::makeFullNsPathAndFindParent(QString &ns, const QString &name, const QString &dirFullPath, const QString &comPath)
{
    CnsFilenameModule* fnModule= getFilenameModule(dirFullPath);
    if(fnModule){
        const NAME2NS_NS2NAME &name2Ns = fnModule->getName2Ns_Ns2Name(comPath);
        QHASH_DEFNAME2NS::const_iterator cit = name2Ns._name2Ns.find(name);
        if(cit == name2Ns._name2Ns.end()){
            return false;
        } else{
            if(ns.isEmpty()){
                ns = cit.value();
                return true;
            }
            else if( ns!= cit.value())
            {
                return name2Ns._nsName.find(ns+'/'+name) != name2Ns._nsName.end();
            }
            return true;
        }

    }else{
        CNSLOG_ERROR(QString("Can't find module %1 in ref").arg(dirFullPath));
    }
    return false;
}

bool CnsDefaultComFactory::makeFullNsPathAndFindParent(QStringList &nsnl, const QString &dirFullPath, const QString &comPath)
{
    if(nsnl.isEmpty() || nsnl.size() %2) return false;
    QStringList nsl;
    QString path;
    for(int i =0; i < nsnl.size(); i +=2)
    {
        if(i>=2){
            if(i >= 4) path += '/';
            for(int j =i-2; j <i; j ++)
            {
                path += nsnl[j];
                if(j<i-1) path += '/';
            }
        }
        QString out = nsnl[i];
        if(makeFullNsPathAndFindParent(out, nsnl[i+1], dirFullPath, comPath+(path.isEmpty()?(comPath.isEmpty()?"/":""):('/'+path))))
        {
            nsl.push_back(out);
        } else return false;
    }
    int i =0;
    for(QStringList::iterator it = nsl.begin(); it != nsl.end(); it ++)
    {
        nsnl[i] = *it;
        i +=2;
    }
    return true;
}

bool CnsDefaultComFactory::makeFullNsPath(QString &out, QString dirFullPath, const QString *locationFullPath, const QString &defOrObjPath)
{
    QStringList nsnl;
    int k = defOrObjPath.indexOf(DIR_NSN_SEPARATOR);
    if(k >=0){
        dirFullPath = defOrObjPath.mid(0, k);
        nsnl = defOrObjPath.mid(k+3).split('/');
    } else{
        nsnl = defOrObjPath.split('/');
    }
    int pos = dirFullPath.lastIndexOf('/');

    k= locationFullPath->indexOf(DIR_NSN_SEPARATOR);
    if(k < 0)return false;
    QString locationPath = locationFullPath->mid(k+2);
    k = -1;
    k = locationPath.lastIndexOf('/', k);
    if(k==-1)return false;//not has this layer
    int size = locationPath.size();
    k = locationPath.lastIndexOf('/', k-size-1);
    if(k==-1)return false;//not even numbers
    dirFullPath = dirFullPath.mid(0, pos+1);

    while(k >=0)
    {
        QString path = locationPath.mid(0, k);
        if(makeFullNsPathAndFindParent(nsnl, dirFullPath, path.isEmpty()?"":path)){
            out = nsnl.join('/');
            if(path.size())
                out = path.mid(1) +'/'+out;
            return true;
        }
        if(path.isEmpty())break;
        k = locationPath.lastIndexOf('/', k-size-1);
        if(k==-1)return false;
        k = locationPath.lastIndexOf('/', k-size-1);
        if(k==-1)return false;//not even numbers
        dirFullPath = dirFullPath.mid(0, pos+1);
        size = locationPath.size() - k +1;
    }
    return false;
}

bool CnsDefaultComFactory::makeFullNsPath(QString &out, QString dirFullPath, const QString &defOrObjPath)
{
    int count = defOrObjPath.count('/');
    if(count > 2 && defOrObjPath[0]=='/'){
        dirFullPath = defOrObjPath.mid(0, defOrObjPath.indexOf('/', 1));
    }
    int pos = dirFullPath.lastIndexOf('/');
    CnsFilenameModule* fnModule= getFilenameModule(dirFullPath.mid(0, pos+1));
    if(fnModule){
        const QHash<QString, QString> &name2Ns = fnModule->getDefName2Ns("/");
        out = defOrObjPath;
        QRegularExpression rx("//([^/]+)");

        QRegularExpressionMatchIterator i = rx.globalMatch(defOrObjPath);

        int j = 0;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();

            QHash<QString, QString>::const_iterator cit = name2Ns.find(match.captured(1));
            if(cit == name2Ns.end()){
                CNSLOG_ERROR(QString("Can't find namespace %1 in ref").arg(match.captured(1)));
                return false;
            } else{
                int k = match.capturedStart(0)+1 + j;
                out.insert(k, cit.value());
                j += cit.value().size();
//                qDebug() << "===ref 0==="<<match.captured(0);
//                qDebug() << "===ref 1==="<<match.captured(1);
//                qDebug() << "===ref capturedStart 0==="<<match.capturedStart(0);
//                qDebug() << "===ref capturedStart 1==="<<match.capturedStart(1);
            }
        }
        return true;
    }else{
        CNSLOG_ERROR(QString("Can't find module %1 in ref").arg(dirFullPath));
    }
    return false;
}

//objPath or defPath suport these formats below:
//objPath or defPath=/filename/.[/ns]*/namespace/def(Obj)Name
//objPath or defPath=/filename/.[/ns]*//def(Obj)Name
//objPath or defPath=dirFullPath/.[/ns]*/namespace/defName
//objPath or defPath=dirFullPath/.[/ns]*//defName
//objPath or defPath=/.//def(Obj)Name
//objPath or defPath=/.[/ns]*/namespace/def(Obj)Name
//objPath or defPath=objName
//objPath or defPath=[ns/]*namespace/objName
QString* CnsDefaultComFactory::getFullPathFromPathAbbr(const QString &dirFullPath, const QString *locationFullPath, QString defOrObjPath)
{
    if(defOrObjPath.isEmpty())
        return nullptr;
    int count = defOrObjPath.count('/');
    int idx;
    //type==0 have no dir and filename info
    //type==1 have filename or full dir path info
    int type = 0;
    //objPath or defPath=objName
    if(!count)
    {
        defOrObjPath.push_front("/.//");
    }else {
        if(defOrObjPath[0]=='/')
        {
            idx = defOrObjPath.indexOf('/', 2);

            if(idx==2)
            {
                //objPath or defPath=/.//def(Obj)Name
                //objPath or defPath=/.[/ns]*/namespace/def(Obj)Name
                type = 0;
            } else{
                //objPath or defPath=/filename/.[/ns]*/namespace/def(Obj)Name
                //objPath or defPath=/filename/.[/ns]*//def(Obj)Name
                type = idx;
            }

        } else{//objPath or defPath=[ns/]*namespace/objName
//            defOrObjPath.push_front("/./");
            type =-1;
        }
    }
    if(type >0) {
        QString targetDirFullPath = getCalleeDirFullPath(dirFullPath, defOrObjPath.mid(1, type-1));
        QString defFullNsPath;
        if(makeFullNsPath(defFullNsPath, targetDirFullPath, locationFullPath, defOrObjPath)){
            if(defFullNsPath.contains(DIR_NSN_SEPARATOR))
                return registerFullPathString(defFullNsPath);
            return registerFullPathString(targetDirFullPath+DIR_NSN_SEPARATOR+defFullNsPath);
        }
        else return nullptr;
    } else if(type <0)
    {
        QString defFullNsPath;
        if(makeFullNsPath(defFullNsPath, dirFullPath, locationFullPath, defOrObjPath))
            return registerFullPathString(dirFullPath+DIR_NSN_SEPARATOR + defFullNsPath);
        else return nullptr;
    } if(!(type || defOrObjPath.endsWith('/')))
    {
        QString defFullNsPath;
        if(makeFullNsPath(defFullNsPath, dirFullPath, locationFullPath, defOrObjPath.mid(3, defOrObjPath.size()-1)))
            return registerFullPathString(dirFullPath+DIR_NSN_SEPARATOR + defFullNsPath);
        else return nullptr;
    } else{
        CNSLOG_ERROR(QString("ref defPath %1 format error!").arg(defOrObjPath));
    }
    return nullptr;
}

//objPath or defPath suport these formats below:
//objPath or defPath=/filename/.[/ns]*/namespace/def(Obj)Name
//objPath or defPath=/filename/.[/ns]*//def(Obj)Name
//objPath or defPath=dirFullPath/.[/ns]*/namespace/defName
//objPath or defPath=dirFullPath/.[/ns]*//defName
//objPath or defPath=/.//def(Obj)Name
//objPath or defPath=/.[/ns]*/namespace/def(Obj)Name
//objPath or defPath=objName
//objPath or defPath=[ns/]*namespace/objName
QString *CnsDefaultComFactory::getFullPathFromPathAbbr(const QString &dirFullPath, QString defOrObjPath)
{
    if(defOrObjPath.isEmpty())
        return nullptr;
    int count = defOrObjPath.count('/');
    int idx;
    //type==0 have no dir and filename info
    //type==1 have filename or full dir path info
    int type = 0;
    //objPath or defPath=objName
    if(!count)
    {
        defOrObjPath.push_front("/.//");
    }else {
        if(defOrObjPath[0]=='/')
        {
            idx = defOrObjPath.indexOf('/', 2);

            if(idx==2)
            {
                //objPath or defPath=/.//def(Obj)Name
                //objPath or defPath=/.[/ns]*/namespace/def(Obj)Name
                type = 0;
            } else{
                //objPath or defPath=/filename/.[/ns]*/namespace/def(Obj)Name
                //objPath or defPath=/filename/.[/ns]*//def(Obj)Name
                type = idx;
            }

        } else{//objPath or defPath=[ns/]*namespace/objName
            defOrObjPath.push_front("/./");
        }
    }
    if(type >0) {
        QString targetDirFullPath = getCalleeDirFullPath(dirFullPath, defOrObjPath.mid(1, type-1));
        QString defFullNsPath;
        if(makeFullNsPath(defFullNsPath, targetDirFullPath, defOrObjPath)){
            if(defFullNsPath.contains(DIR_NSN_SEPARATOR))
                return registerFullPathString(defFullNsPath);
            return registerFullPathString(targetDirFullPath+DIR_NSN_SEPARATOR+defFullNsPath.mid(type, defFullNsPath.size()-type));
        }
        else return nullptr;
    } else if(!(type || defOrObjPath.endsWith('/')))
    {
        QString defFullNsPath;
        if(makeFullNsPath(defFullNsPath, dirFullPath, defOrObjPath.mid(2, defOrObjPath.size()-1)))
            return registerFullPathString(dirFullPath+"/." + defFullNsPath);
        else return nullptr;
    } else{
        CNSLOG_ERROR(QString("ref defPath %1 format error!").arg(defOrObjPath));
    }
    return nullptr;
}

CnsFilenameModule* CnsDefaultComFactory::getFilenameModule(const QString &dirPath)
{
    QHash<QString, CnsFilenameModule*>::iterator it = d->dirFullPath2FnModuleHash.find(dirPath);
    if(it == d->dirFullPath2FnModuleHash.end())
    {
        return nullptr;
    }
    return it.value();
}

void CnsDefaultComFactory::registerFilenameModule(const QString &dirFullPath, CnsFilenameModule * fnModule)
{
    d->dirFullPath2FnModuleHash[dirFullPath] = fnModule;
}

//const QString* CnsDefaultComFactory::registerFullPathString(QString *fullPath)
//{
//    QHash<QString, QString *>::iterator it = d->_comDirDefObjPaths.find(*fullPath);
//    if(it == d->_comDirDefObjPaths.end()){
//        d->_comDirDefObjPaths[*fullPath] = fullPath;
//        return nullptr;
//    } else{
//        return it.value();
//    }
//}

QString* CnsDefaultComFactory::registerFullPathString(const QString &fullPath)
{
    QHash<QString, QString *>::iterator it = d->_comDirDefObjPaths.find(fullPath);
    if(it == d->_comDirDefObjPaths.end()){
        QString *p = new QString(fullPath);
        d->_comDirDefObjPaths[fullPath] = p;
        return p;
    }
    return it.value();
}

QString getCalleeDirFullPathDownwards(CnsFilenameModule* modules, CnsDefaultComFactory::DirEntry *callerDirEntry, const QString &calleeFilename, QString callerDirPath)
{
    //search downward
    QList<CnsDefaultComFactory::DirTreeEntry> queue;
    queue.push_back(CnsDefaultComFactory::DirTreeEntry(callerDirEntry, callerDirPath));
    while (!queue.isEmpty()) {
        CnsDefaultComFactory::DirTreeEntry& args = queue.front();
        queue.pop_front();
        CnsDefaultComFactory::DirEntry *de = args._dirEntry;
        for(QStringList::iterator it = de->_files.begin(); it != de->_files.end(); it ++)
        {
            if(*it == calleeFilename)
            {
                modules->_fn2DirFullPath[calleeFilename] = args._path + calleeFilename;
                return modules->_fn2DirFullPath[calleeFilename];
            }
        }
        for(QHash<QString, CnsDefaultComFactory::DirEntry>::iterator it = de->_dirs.begin(); it != de->_dirs.end(); it ++)
        {
            queue.push_back(CnsDefaultComFactory::DirTreeEntry(&it.value(), args._path + it.key()));
        }
    }
    return "";
}

QString getCalleeDirFullPathFromCaller(CnsFilenameModule* modules, CnsDefaultComFactory::DirEntry *callerDirEntry, const QString &calleeFilename, QString callerDirPath)
{
    QHash<QString, QString>::iterator it = modules->_fn2DirFullPath.find(calleeFilename);
    if(it == modules->_fn2DirFullPath.end())
    {
        CnsDefaultComFactory::DirEntry *p = callerDirEntry;
        CnsDefaultComFactory::DirEntry *lastP = nullptr;
        QString path = callerDirPath;
        QString ret;
        while(p){
            for(QStringList::iterator it = p->_files.begin(); it != p->_files.end(); it ++)
            {
                if(*it == calleeFilename)
                {
                    modules->_fn2DirFullPath[calleeFilename] = path +'/'+ calleeFilename;
                    return modules->_fn2DirFullPath[calleeFilename];
                }
            }
            for(QHash<QString, CnsDefaultComFactory::DirEntry>::iterator it = p->_dirs.begin(); it != p->_dirs.end(); it ++)
            {
                if(&it.value() != lastP){
                    ret = getCalleeDirFullPathDownwards(modules, p, calleeFilename, callerDirPath);
                    if(!ret.isEmpty())
                    {
                        return ret;
                    }
                }
            }
            //search upward
            lastP = p;
            int pos = path.lastIndexOf('/');
            if(pos <= 1)
            {
                CNSLOG_ERROR(QString("Can't search calleeFilename = %1 in directory and callerDirPath %2 format is error!").arg(calleeFilename).arg(callerDirPath));
                return "";
            }
            path = path.mid(0, pos);
            p = p->_parent;
        }
        CNSLOG_ERROR(QString("Can't search calleeFilename = %1 in directory!").arg(calleeFilename));
        return "";
    } else{
        return it.value();
    }
}

QString CnsDefaultComFactory::getCalleeDirFullPath(const QString &callerDirFullPath, const QString &calleeFilename)
{
    int pos = callerDirFullPath.lastIndexOf('/');
    QString callerDirPath = callerDirFullPath.mid(0, pos+1);
    QHash<QString, CnsFilenameModule*>::iterator it = d->dirFullPath2FnModuleHash.find(callerDirPath);
    if(it == d->dirFullPath2FnModuleHash.end() || !it.value()->hasDirFullPath(calleeFilename))
    {
        QStringList paths = callerDirFullPath.split('/', QString::SkipEmptyParts);
        CnsDefaultComFactory::DirEntry *de = &d->cnsDirTree.begin().value();

        for(QStringList::iterator pit = paths.begin(); pit!= paths.end();pit++)
        {
            bool found = false;
            if(pit+1 == paths.end())
            {
                for(QStringList::iterator jt = de->_files.begin(); jt != de->_files.end(); jt ++)
                {
                    if(*pit == *jt)
                    {
                        found = true;
                        break;
                    }
                }
                if(!found)
                {
                    CNSLOG_ERROR(QString("Can't find file %1 in caller = %2").arg(*pit).arg(callerDirFullPath));
                    return "";
                }
            } else{
                QHash<QString, CnsDefaultComFactory::DirEntry>::iterator jt = de->_dirs.begin();
                for(; jt != de->_dirs.end(); jt ++)
                {
                    if(*pit == jt.key())
                    {
                        de = &jt.value();
                        found = true;
                        break;
                    }
                }
                if(!found)
                {
                    CNSLOG_ERROR(QString("Can't find dir %1 in caller = %2").arg(*pit).arg(callerDirFullPath));
                    return "";
                }
            }
        }
        return getCalleeDirFullPathFromCaller(it.value(), de, calleeFilename, callerDirPath.mid(0, callerDirPath.size()-1));
    } else{
        return it.value()->_fn2DirFullPath[calleeFilename];
    }
}

QHash<QString, CnsDefaultComFactory::DirEntry> &CnsDefaultComFactory::getDirTree()
{
    return d->cnsDirTree;
}

CnsDefaultComFactory::CnsDefaultComFactory(
        CnsDefaultPropertyConverterConstPtrRef converter)
{
    CNS_NEW_PRIVATE_DATA(CnsDefaultComFactory)
    
    d->converter = converter;
}

CnsDefaultComFactory::~CnsDefaultComFactory()
{
}

bool CnsDefaultComFactory::addPaths(CnsComponentConstPtrRef com, const QString *objFullPath
                 , CnsComDefinitionConstPtrRef comDefinition) noexcept
{
    com->setSorts(comDefinition->getSorts());
    com->setName(comDefinition->getName());
    com->setObjFullPath(objFullPath);

    return true;
}

CNS_SYS_REGISTER(SysDummy)

bool CnsDefaultComFactory::addSystemValue(CnsComponentConstPtrRef com
                                          , CnsComDefinitionConstPtrRef comDefinition) noexcept
{
//    auto systemName = comDefinition->getSystemName();
    auto system = getSystem(comDefinition->getSystemName());
    if (!system) {
        qCritical() << QString("the class '%1' has not system %2").arg(comDefinition->getName()).arg(comDefinition->getSystemName());
        return false;
    }
    com->setSystem(system);
    return true;
}

void CnsDefaultComFactory::addMethodsValue(CnsComponentConstPtrRef com
                 , CnsComDefinitionConstPtrRef comDefinition) noexcept
{
    const METHODDATAHASH& mdh = comDefinition->getMethodDataHash();
    for(METHODDATAHASH::const_iterator it = mdh.begin(); it != mdh.end(); it ++)
    {
        com->setMethodState(it.key(), it.value());
    }
}

bool CnsDefaultComFactory::addPropertyValue(CnsComponentPtr com
                    , CnsComDefinitionConstPtrRef comDefinition) noexcept
{
    QVariantMap props = comDefinition->getConstProperties();
    const QString* ps = comDefinition->getInheritFullPath(this);
    while (ps && ps->size()) {
        CnsComDefinitionPtr p = findComDefinition(ps);
        if(p.isNull()){
            qCritical() << QString("Can't find the parent obj %1!!").arg(*ps);
            return false;
        }

        const QVariantMap &inherits = p->getConstProperties();
        for(auto it = inherits.begin(); it != inherits.end(); it++)
        {
            if(props.find(it.key())== props.end())
            {
                props[it.key()] = it.value();
            }
        }
        ps = p->getInheritFullPath(this);
    }

    for (auto itr = props.cbegin(); itr != props.cend(); ++itr) {
        //! get prop
        auto value = itr.value();

        //! interpreter value
        value = d->converter->convert(this, value);
//        if(!value.isValid())
//        {
//            continue;
//        }

        if(value.canConvert<CnsComponentPtr>())
        {
            com->addRef(itr.key(), value.value<CnsComponentPtr>());
        } else{

            com->addProperty_ts(itr.key().toLocal8Bit(), value);
        }
    }
    return true;
}

CnsComponentPtr CnsDefaultComFactory::doCreate(
        const QString *objFullPath,
        CnsComDefinitionConstPtrRef comDefinition) noexcept
{
    CnsComponentPtr com;
    if(comDefinition->getClassName().isEmpty())
    {
        com = CnsComponentPtr::create();
    } else{
        int typeId = QMetaType::type(comDefinition->getClassName().toLatin1());
        const QMetaObject *type = QMetaType::metaObjectForType(typeId);
        CnsComponent * vp =static_cast<CnsComponent *>(type->newInstance());

        com.reset(vp);
    }

    if (!com) {
        qCritical() << QString("can't create CnsComponent!")
            .arg(comDefinition->getName());
        return nullptr;
    }
//    QString *objFullPath = comDefinition->convertObjFullPath(objName);

    registerCnsObject(objFullPath, com);	//save cns obj

    if(!addSystemValue(com, comDefinition))
    {
        qCritical() << QString("failed to init system '%1'").arg(comDefinition->getSystemName());
        return nullptr;
    }

    if (!addPropertyValue(com, comDefinition)) {
        qCritical() << QString("failed to init definition '%1'").arg(com->metaObject()->className());
        return nullptr;
    }

    addMethodsValue(com, comDefinition);

    addPaths(com, objFullPath, comDefinition);
    com->setComDef(comDefinition);

    return com;
}

CnsComponentPtr CnsDefaultComFactory::findRefObj(const QString *objPath)
{
    const QHash<const QString*, CnsComponentPtr>::iterator it = d->comObjHash.find(objPath);
    if(it == d->comObjHash.end())
    {
        return nullptr;
    }
    return it.value();
}

//CnsComDefinitionPtr CnsDefaultComFactory::findObjComDefinition(const QString *defFullPath)
//{
//    QHash<const QString*, CnsComDefinitionPtr>::iterator objIt = d->comNewObjDefHash.find(defFullPath);
//    if(objIt == d->comNewObjDefHash.end())
//    {
//        QHash<const QString*, CnsComDefinitionPtr>::iterator it = d->comDefHash.find(defFullPath);
//        if(it == d->comDefHash.end())
//        {
//            return nullptr;
//        }
//        return it.value();
//    } else return objIt.value();
//}

CnsComDefinitionPtr CnsDefaultComFactory::findComDefinition(const QString *defFullPath)
{
    QMap<const QString*, CnsComDefinitionPtr>::iterator it = d->comDefHash.find(defFullPath);
    if(it == d->comDefHash.end())
    {
        return nullptr;
    }
    return it.value();
}

//defPath suport these formats below:
//defPath=/filename[/ns*/n]*/namespace/defName
//defPath=/filename[/ns*/n]*//defName
QString CnsDefaultComFactory::getDirFullPathFromPath(const QString &defPath)
{
    int pos = defPath.indexOf('/');
    if(pos)
    {
        return "";
    }
    pos = defPath.indexOf('/', 1);
    if(pos ==-1)
    {
        return "";
    }

    QString ret = "";
    QString fn = defPath.mid(1, pos-1);
    return getCalleeDirFullPath("/", fn);
}

//defPath not in cns config xml suport these formats below:
//defPath=/filename[/ns*/n]*/namespace/defName
//defPath=/filename[/ns*/n]*//defName
//defPath=dirFullPath[/ns*/n]*/namespace/defName
//defPath=dirFullPath[/ns*/n]*//defName
//objPath that with defPath suport these formats below:
//objPath=objName
//objPath=namespace/objName
CnsComponentPtr CnsDefaultComFactory::newComObj(const QString &defPath, const QString &objPath)  noexcept
{
    QString dirFullPath = getDirFullPathFromPath(defPath);
    if(dirFullPath.isEmpty()) return nullptr;
    QString *defFullPath = getFullPathFromPathAbbr(dirFullPath, defPath);

    QString *objFullPath = getObjFullPathFromDefFullPath(*defFullPath, objPath);
    if(!objFullPath)
    {
        return nullptr;
    }
    auto autoCreateCom = findRefObj(objFullPath);
    if(autoCreateCom.isNull()){

        return getComObj(defFullPath, objFullPath);
    } else{
        CNSLOG_ERROR(QString("new com obj but has had one with duplicated name"));
        return nullptr;
    }
}

//defPath not in cns config xml suport these formats below:
//defPath=filename[/ns*/n]*/namespace/defName
//defPath=filename[/ns*/n]*//defName
//objPath that with defPath suport these formats below:
//objPath=objName
//objPath=namespace/objName
QString* CnsDefaultComFactory::getObjFullPathFromDefFullPath(const QString &defFullPath, const QString &objPath)  noexcept
{
    int count = objPath.count('/');
    if(!count)
    {
        int idx = defFullPath.lastIndexOf('/');
        if(idx >0){
            QString ret(defFullPath);
            ret.replace(idx+1, defFullPath.size()-idx-1, objPath);
            return registerFullPathString(ret);
        }
    } else if(count ==1)
    {
        int idx = defFullPath.lastIndexOf('/');
        if(idx>0){
            idx = defFullPath.lastIndexOf('/',idx-1);
            if(idx>0)
            {
                QString ret(defFullPath);
                ret.replace(idx+1, defFullPath.size()-idx-1, objPath);
                return registerFullPathString(ret);
            }
        }
    } else{
        CNSLOG_ERROR(QString("This obj full path %1 refer to def full path format not support!").arg(objPath));
        return nullptr;
    }
    CNSLOG_ERROR(QString("This def full path %1 refer to obj full path format not support!").arg(defFullPath));
    return nullptr;
}

//defPath not in cns config xml suport these formats below:
//defPath=/filename[/ns*/n]*/namespace/defName
//defPath=/filename[/ns*/n]*//defName
//objPath that with defPath suport these formats below:
//objPath=objName
//objPath=namespace/objName
CnsComponentPtr CnsDefaultComFactory::getComObjFromPathAbbr(const QString &defPath, const QString &objPath)  noexcept
{
    QString dirFullPath = getDirFullPathFromPath(defPath);
    if(dirFullPath.isEmpty()) return nullptr;
    QString *defFullPath = getFullPathFromPathAbbr(dirFullPath, defPath);

    QString* objFullPath =getObjFullPathFromDefFullPath(*defFullPath, objPath);
    if(!(defFullPath && objFullPath))
    {
        return nullptr;
    }
    return getComObj(defFullPath, objFullPath);
}

CnsComponentPtr CnsDefaultComFactory::getComObjFromPathAbbr(const QString &objPath)  noexcept
{
    QString dirFullPath = getDirFullPathFromPath(objPath);
    if(dirFullPath.isEmpty()) return nullptr;
    QString *objFullPath = getFullPathFromPathAbbr(dirFullPath, objPath);
    return getComObj(objFullPath, objFullPath);
}

//QString* CnsDefaultComFactory::convertDefFullPath(const QString &defPath)
//{
//    QHash<QString, QString *>::iterator it = d->_comDirDefObjPaths.find(isFullPath(defPath)?defPath:getDefFullPath("/", defPath));
//    return it == d->_comDirDefObjPaths.end()?nullptr:it.value();
//}

CnsComponentPtr CnsDefaultComFactory::getComObj(const QString *defFullPath, const QString *objFullPath)  noexcept
{
    auto autoCreateCom = findRefObj(objFullPath);
    if (autoCreateCom.isNull()) {	//!< if autoCreate com not exist
        auto comDefinition = findComDefinition(defFullPath);
        if (comDefinition == nullptr) {
            qCritical() << "No com definition named " << *defFullPath << " is defined";
            return nullptr;
        }
        autoCreateCom = doCreate(objFullPath, comDefinition);	//!< Create
        if (autoCreateCom.isNull()) {
            qWarning() << QString("failed to create obj '%1'").arg(*objFullPath);
            return nullptr;
        }
    }
    return autoCreateCom;
}

CnsComponentPtr CnsDefaultComFactory::resolveComReferenceToQVariant(CnsComReferenceConstPtrRef comRef) noexcept
{
    if (!comRef) {
        qCritical() << "comReference not exists";
        return nullptr;
    }
    QMap<const QString*, CnsComDefinitionPtr>::iterator it = d->comDefHash.find(comRef->getObjFullPath(this));
    if(it == d->comDefHash.end())
    {
        return nullptr;
    }
    return getComObj(it.value()->getDefFullPath(this), comRef->getObjFullPath(this));
}

//void CnsDefaultComFactory::registerNewObjDefinition(const QString *defObjFullPathName,
//        CnsComDefinitionPtr comDefinition) noexcept
//{
//    d->comNewObjDefHash.insert(defObjFullPathName, comDefinition);
//}

void CnsDefaultComFactory::registerComDefinition(
        CnsComDefinitionConstPtrRef def) noexcept
{
    d->comDefHash.insert(def->getDefFullPath(), def);
}

void CnsDefaultComFactory::registerCnsObject(const QString *fullObjPathName, CnsComponentPtr component) noexcept
{
    d->comObjHash.insert(fullObjPathName, component);
}

static QHash<QString, QObject*> *systems=nullptr;
QHash<QString, QObject*> *getSystems()
{
    if(systems)
        return systems;
    else{
        systems = new QHash<QString, QObject*>();
        return systems;
    }
}
void registerSystem(
        const QString &name, QObject* system) noexcept
{
    getSystems()->insert(name, system);
}

QObject* getSystem(const QString &name) noexcept
{
    QHash<QString, QObject*>::iterator ptr = getSystems()->find(name);
    if(ptr == getSystems()->end())
    {
        return nullptr;
    }

    return ptr.value();
}

//bool CnsDefaultComFactory::isContained(const QString &name) noexcept
//{
//    return d->hash.contains(name);
//}

QMap<const QString*, CnsComDefinitionPtr>& CnsDefaultComFactory::getComDefinitions() noexcept
{
    return d->comDefHash;
}
