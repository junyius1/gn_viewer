#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <qdom.h>
#include <QLibrary>
#include <QDebug>

#include "cnsDefinitionReader.h"
#include "cnsComReference.h"
#include "cnsDefaultPropertyParser.h"
#include "cnsComDefinition.h"
#include "cnsDefaultComFactory.h"

CNS_DECL_PRIVATE_DATA(CnsDefinitionReader)
QList<QIODevicePtr> devices;
CnsDefaultComFactory *registry{nullptr};
CNS_DECL_PRIVATE_DATA_END

CnsDefinitionReader::CnsDefinitionReader(
        CnsDefaultPropertyParserConstPtrRef parser
        , QIODeviceConstPtrRef device)
    : CnsDefinitionReader(parser, QList<QIODevicePtr>() << device)
{
}

CnsDefinitionReader::CnsDefinitionReader(
        CnsDefaultPropertyParserConstPtrRef parser
        , const QList<QIODevicePtr> &devices):_creator(parser)
{
    CNS_NEW_PRIVATE_DATA(CnsDefinitionReader)
    
    d->devices = devices;
}

CnsDefinitionReader::~CnsDefinitionReader()
{
}

void CnsDefinitionReader::doReadComDefinition() noexcept
{
    for(auto device : d->devices) {
        device->seek(0);
        readComDefinition(device);
    }
    _creator.createComDefs();
}

void CnsDefinitionReader::readComDefinition(QIODeviceConstPtrRef source) noexcept
{
	QDomDocument doc;
	QString errorMsg;
	int errorLine;
	int errorCol;
	if (!doc.setContent(source.data(), &errorMsg, &errorLine, &errorCol)) {
        qCritical() << "error occured. Line:" << errorLine
                    << "Column:" << errorCol
                    << "Error msg:" << errorMsg;
		return;
	}
    QFile * file = static_cast<QFile*>(source.data());
    readComDefinition(file->fileName(), doc);
}

void CnsDefinitionReader::readComDefinition(const QString &dirFullPath, const QDomDocument &doc) noexcept
{
	QDomElement root = doc.documentElement();
	if (root.isNull()) {
		qCritical() << "XML Error: Root element is NULL.";
		return;
	}
    if (root.tagName() != CNS_CNS) {
		qCritical() << "XML Error: Root element is not beans.";
		return;
	}
    readComDefinition(dirFullPath, root.childNodes());
}

void CnsDefinitionReader::addCreatorNode(const QString &dirFullPath, CnsFilenameModule * fnModule, const QDomElement &ele, QString comPath)
{
    QString defName = ele.attribute(CNS_NAME);
    QString defNs = ele.attribute(CNS_NAMESPACE);
    if(defNs.size())
        defNs = defNs.split('/')[0];
//    bool hasRelativePath = comPath.size() >0;

    fnModule->setDefName2Ns(defName, defNs, comPath);
//    CNSLOG_INFO(QString("addCreatorNode setDefName2Ns dirFullPath = %1, defName = %2, defNs = %3 comPath = %4")
//                .arg(dirFullPath)
//                .arg(comPath+defName)
//                .arg(defNs).arg(comPath.isEmpty()?"/":comPath));
    QString myComPath = comPath + (comPath.size()==1?"":"/") + defNs+'/'+defName;
//    if(comPath.contains("patrolItems"))
//    {
//        int jj=1;
//    }
    CnsDefinitionCreator::CnsDefNode& n = _creator.getAddNode(dirFullPath+myComPath);
    n.node = ele;
//    myComPath += '/';
    if(comPath.size() >1)
    {
        n.relativePath = new QString(comPath);
    }
//    CNSLOG_INFO(QString("addCreatorNode CnsDefNode = %1, relativePath = %2")
//                .arg(n.node.attribute("namespace")+'/'+n.node.attribute("name"))
//                .arg(n.relativePath?*n.relativePath:""));
    const QDomNodeList &nodes = ele.childNodes();
    for (int i = 0; i < nodes.size(); ++i) {
        QDomElement propEle = nodes.at(i).toElement();
        if(propEle.tagName() == CNS_COM)
            addCreatorNode(dirFullPath, fnModule, propEle, myComPath);
    }
}

void CnsDefinitionReader::readComDefinition(const QString &xmlFileFullPath, const QDomNodeList &nodes) noexcept
{
    QString dirFullPath;
    CnsDefaultComFactory *reg = registry();
    int size = reg->getRootDir().size();
    if(xmlFileFullPath.left(size)== reg->getRootDir())
    {
        dirFullPath = xmlFileFullPath.mid(size, xmlFileFullPath.size()-size-4);//remove .xml also
    } else{
        CNSLOG_ERROR(QString("cns xml file %s format error").arg(xmlFileFullPath));
        return;
    }

    int pos = dirFullPath.lastIndexOf('/');
    const QString& dirPath = dirFullPath.mid(0, pos+1);

    CnsFilenameModule * fnModule = registry()->getFilenameModule(dirPath);
    if(!fnModule){
        fnModule = new CnsFilenameModule();
        registry()->registerFilenameModule(dirPath, fnModule);
    }

    dirFullPath += "/.";

	for (int i = 0; i < nodes.size(); ++i) {
        const QDomElement ele = nodes.at(i).toElement();
        addCreatorNode(dirFullPath, fnModule, ele);
	}
}

CnsDefaultComFactory *CnsDefinitionReader::registry() const noexcept
{
    return  d->registry;
}

void CnsDefinitionReader::readComDefinition(CnsDefaultComFactory *registry) noexcept
{
    d->registry = registry;
    _creator.setRegistry(registry);
    doReadComDefinition();
}
