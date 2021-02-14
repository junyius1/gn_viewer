#include "framework.h"
#include "cnsDefaultPropertyConverter.h"
#include <QSharedPointer>
#include <QFile>
#include <QDebug>
#include "cnsDefinitionReader.h"
#include "cnsComDefinition.h"
#include <QDir>

QT_BEGIN_NAMESPACE
CNS_FORWARD_DECL_CLASS(QIODevice);
QT_END_NAMESPACE

void genDirTree(QString path, QString cnsDir, QHash<QString, CnsDefaultComFactory::DirEntry> &dirTree)
{
    QDir dir = QDir(path);
    QFileInfoList l = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name);
    CnsDefaultComFactory::DirEntry &de = dirTree[cnsDir];
    for(QFileInfoList::iterator it = l.begin(); it != l.end(); it ++)
    {
        if(it->isFile())
        {
            int pos = it->fileName().lastIndexOf('.');
            de._files.push_back(it->fileName().mid(0, pos));
        } else if(it->isDir())
        {
            genDirTree(path + it->fileName(), cnsDir + it->fileName(), de._dirs);
        }
    }
}

void Framework::readComDefinition(const QString &path)
{
    QList<QIODevicePtr> devices;
    QHash<QString, CnsDefaultComFactory::DirEntry> &dirTree = _comFactory->getDirTree();
    if(!_comFactory->setRootDir(path))
        return;
    genDirTree(_comFactory->getRootDir()+'/', "/", dirTree);

    QList<CnsDefaultComFactory::DirTreeEntry> queue;
    queue.push_back(CnsDefaultComFactory::DirTreeEntry(&dirTree.begin().value(), path));
    QStringList locations;
    while (!queue.isEmpty()) {
        CnsDefaultComFactory::DirTreeEntry args = queue.front();
        queue.pop_front();
        CnsDefaultComFactory::DirEntry *de = args._dirEntry;
        for(QStringList::iterator it = de->_files.begin(); it != de->_files.end(); it ++)
        {
            locations.append(args._path +'/'+ *it+".xml");
        }
        for(QHash<QString, CnsDefaultComFactory::DirEntry>::iterator it = de->_dirs.begin(); it != de->_dirs.end(); it ++)
        {
            queue.push_back(CnsDefaultComFactory::DirTreeEntry(&it.value(), args._path + it.key()));
        }
    }

    for(auto location : locations) {
        if(!QFile::exists(location)) {
            qCritical() << "file '" << location << "' not exists";
            continue;
        }
        auto device = QSharedPointer<QFile>::create((location));
        if (!device->open(QIODevice::ReadOnly)) {
            qCritical() << "open file '" << location << "' failure."
                        << "error code:" << device->error()
                        << "error msg:" << device->errorString();
            continue;
        }
        devices.append(QIODevicePtr(device));
    }
    CnsDefinitionReaderPtr reader = CnsDefinitionReaderPtr::create(
                      CnsDefaultPropertyParserPtr::create(), devices);
    reader->readComDefinition(_comFactory.data());
}

Framework::Framework()
{
    gCnsLogger = new CnsLogger();
    CnsDefaultPropertyConverterPtr converter = CnsDefaultPropertyConverterPtr::create();
    _comFactory = CnsDefaultComFactoryPtr::create(converter);
}

Framework::~Framework()
{
    delete gCnsLogger;
}

void Framework::registerSystem(const QString &name, QObject* system)
{
    ::registerSystem(name, system);
}

CnsComponentPtr Framework::getComponent(const QString &objName, const QString &defPath)
{
    if(defPath.isEmpty())
        return _comFactory->getComObjFromPathAbbr(objName);
    return _comFactory->getComObjFromPathAbbr(defPath, objName);
}

CnsComponentPtr Framework::newComponent(const QString &objName, const QString &defPath)
{
    return _comFactory->newComObj(defPath, objName);
}

void Framework::autoCreateComponent(const QString &path)
{
    readComDefinition(path);

    QMap<const QString*, CnsComDefinitionPtr>& comDefinitions = _comFactory->getComDefinitions();
    for(QMap<const QString*, CnsComDefinitionPtr>::iterator it = comDefinitions.begin(); it != comDefinitions.end(); it ++)
    {
        CnsComDefinitionPtr ptr = it.value();
        if(ptr->isCreate()){
            const QString *str = ptr->getDefFullPath(_comFactory.data());
            _comFactory->getComObj(str, str);
        }
    }
}
