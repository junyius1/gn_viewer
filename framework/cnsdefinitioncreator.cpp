#include "cnsdefinitioncreator.h"
#include "log/cnslogger.h"
#include "cnsDefaultComFactory.h"
#include <QRegularExpression>
#include <QMetaMethod>

CNS_DECL_PRIVATE_DATA(CnsDefinitionCreator)
CnsDefaultPropertyParserPtr  parser;
CnsDefaultComFactory *registry{nullptr};
CNS_DECL_PRIVATE_DATA_END

CnsDefinitionCreator::~CnsDefinitionCreator()
{
}

CnsDefinitionCreator::CnsDefinitionCreator(CnsDefaultPropertyParserConstPtrRef parser)
{
    CNS_NEW_PRIVATE_DATA(CnsDefinitionCreator)

    d->parser = parser;
}

void CnsDefinitionCreator::setRegistry(CnsDefaultComFactory *registry)
{
    d->registry = registry;
}

CnsDefinitionCreator::CnsDefNode& CnsDefinitionCreator::getAddNode(const QString &path)
{
    if(_nodes.find(path) == _nodes.end()){
        return _nodes[path];
    }
    else{
        CNSLOG_WARN(QString("Duplicated path %1 in definition creator addNode").arg(path));
        return _nodes[path];
    }
}

void CnsDefinitionCreator::createComDefs()
{
    CnsCreateComDefContext context(this, registry());
    for(auto it = _nodes.begin(); it != _nodes.end(); it++)
    {
        const QString &k = it.key();
        int index = k.indexOf('.');
        const CnsDefNode & cdn = it.value();
//        CNSLOG_INFO(QString("createComDef CnsDefNode = %1, nsn = %2, relativePath = %3")
//                    .arg(k.left(index-1))
//                    .arg(cdn.node.attribute("namespace")+'/'+cdn.node.attribute("name"))
//                    .arg(cdn.relativePath?*cdn.relativePath:""));
        createComDef(k.left(index-1), cdn.node, context, cdn.relativePath?*cdn.relativePath:"");
    }
}

bool CnsDefinitionCreator::createComDefFromPath(const QString &path, CnsCreateComDefContext &context, QString dirFullPath)
{
    int dirPathIndex = path.indexOf(DIR_NSN_SEPARATOR);
    if(dirPathIndex == -1 && dirFullPath.isEmpty())
    {
        CNSLOG_ERROR(QString("Invalid input path %1 and dirFullPath %2 in createObjOrComFromPath").arg(path).arg(dirFullPath));
        return false;
    }
    QString format = dirPathIndex == -1?dirFullPath+DIR_NSN_SEPARATOR+path:path;
    if(dirFullPath!=-1)
    {
        dirFullPath = path.mid(0, dirPathIndex);
    }

    format.replace("//", "/[^./]*/");
    QRegularExpression rx('^'+format+'$');
    QMap<QString, CnsDefNode>::iterator it = _nodes.begin();
    for(; it != _nodes.end(); it++)
    {
        QRegularExpressionMatch i = rx.match(it.key());
        if(i.hasMatch())
            break;
    }

    if(it == _nodes.end()) return false;
    const CnsDefNode & cdn = it.value();
    return createComDef(dirFullPath, cdn.node, context, cdn.relativePath?*cdn.relativePath:"");
}

bool CnsDefinitionCreator::createComDef(const QString &dirFullPath, const QDomElement &ele, CnsCreateComDefContext &context, const QString &comPath)
{
    QString *pDirFullPath = registry()->registerFullPathString(dirFullPath);
    if(ele.isNull()) {
        return false;
    }

    QString defName = ele.attribute(CNS_NAME);

    if((ele.tagName() != CNS_COM) || defName.isEmpty())
    {
        CNSLOG_ERROR(QString("node name must be 'com'"));
        return false;
    }

    createComDef(comPath, ele, pDirFullPath, context);
    return true;
}

bool CnsDefinitionCreator::parseComClass(const QDomElement &ele, CnsComDefinitionConstPtrRef def) noexcept
{
    if (ele.hasAttribute(CNS_NAME))

        def->setName(ele.attribute(CNS_NAME));
    else{
        CNSLOG_ERROR(QString("component must has name, please check!!!"));
        return false;
    }

    if (ele.hasAttribute(CNS_CLASS))

        def->setClassName(ele.attribute(CNS_CLASS));

    if (ele.hasAttribute(CNS_CREATE))

        def->setCreate(ele.attribute(CNS_CREATE).toInt());


    if (ele.hasAttribute(CNS_INHERIT))

        def->setInheritPath(ele.attribute(CNS_INHERIT));


    if(ele.hasAttribute(CNS_SYSTEM)){
        def->setSystemName(ele.attribute(CNS_SYSTEM));
        auto system = getSystem(ele.attribute(CNS_SYSTEM));
        if (!system) {
            CNSLOG_ERROR(QString("the class '%1' has not system").arg(def->getSystemName()));
            return false;
        }
    }

    if (ele.hasAttribute(CNS_NAMESPACE)){
        QString nameSpace = ele.attribute(CNS_NAMESPACE);
        def->setSorts(nameSpace.split('/', QString::SkipEmptyParts));
    }

    QStringList nsn;
    for(auto it = def->getSorts().begin(); it != def->getSorts().end(); it ++)
        nsn.push_back(*it + '/'+def->getName());
    def->setNsN(nsn);

    return true;
}

CnsDefaultComFactory *CnsDefinitionCreator::registry() const noexcept
{
    return  d->registry;
}


void CnsDefinitionCreator::readComAllDefinitionProperties(
        const QString &comPath, const QDomNodeList &propNodes
        , CnsComDefinitionConstPtrRef def, CnsCreateComDefContext &context) noexcept
{
    for (int i = 0; i < propNodes.size(); ++i) {
        const QDomElement &propEle = propNodes.at(i).toElement();
        if (propEle.isNull())
            continue;
        readComDefinitionForProperty(comPath, propEle, def, context);
    }
}

void CnsDefinitionCreator::readComDefinitionForProperty(
        const QString &comPath, const QDomElement &propEle
        ,CnsComDefinitionConstPtrRef comDefinition, CnsCreateComDefContext &context) noexcept
{

    QString propName;

    QVariant value;
    //ref a object
    if(propEle.tagName() == CNS_COM){
        CnsComReferencePtr ref = CnsComReferencePtr::create(comDefinition->getDirFullPath());
        CnsComDefinitionPtr def = createComDef(comPath, propEle
        , comDefinition->getDirFullPath(), context);
        ref->setObjFullPath(def->getDefFullPath());
        value.setValue(ref);
        propName = *def->getDefFullPath();
    }
    else {
        propName = propEle.attribute("name");
        if (propName.isEmpty()) {
            CNSLOG_ERROR(QString("property name not be able null!!"));
            return;
        }
        value = d->parser->parse(propEle, comDefinition.data(), context);
    }
    if(value.isValid())
        comDefinition->addProperty(propName, value);
    else
        CNSLOG_ERROR(QString("the named '%1' property's value is invalid!!").arg(propName));
}

const QString *CnsDefinitionCreator::getAndRegisterFullPath(const QString &dirFullPath, const QString &comPath, const QDomElement &ele)
{
    const QString &ns = ele.attribute(CNS_NAMESPACE);
    return registry()->registerFullPathString(dirFullPath + "/." +comPath + '/'\
          + (ns.isEmpty()?"":ns.split('/')[0]) + '/' + ele.attribute(CNS_NAME));
}

const QString CnsDefinitionCreator::combineRelativePath(const QDomElement &ele)
{
    const QString &ns = ele.attribute(CNS_NAMESPACE);
    return '/' + (ns.isEmpty()?"":ns.split('/')[0]) + '/' + ele.attribute(CNS_NAME);
}

//void CnsDefinitionCreator::fixMissingNamespace(CnsComDefinitionPtr def, CnsCreateComDefContext &context)
//{
//    if(def->getNamespace().isEmpty()){

//        const QString *inherit = def->getInheritFullPath(context.getDefaultComFactory());
//        if(!inherit && !def->getInheritPath().isEmpty())
//        {
//            CNSLOG_ERROR(QString("Error inherited format that can't find namespace!"));
//            return;
//        }
//        const QString* ps = inherit;
//        CnsComDefinitionPtr lastp = def;
//        while (ps && ps->size()) {
//            CnsComDefinitionPtr p = context.getDefaultComFactory()->findComDefinition(ps);
//            if(p.isNull()){
//                context.getDefinitionCreator()->createObjOrComFromPath(*ps, context, false, *lastp->getDirFullPath());
//                p = context.getDefaultComFactory()->findComDefinition(ps);
//            }
//            if(p->getNamespace().isEmpty()){
//                ps = p->getInheritFullPath(context.getDefaultComFactory());
//                if(!ps && !p->getInheritPath().isEmpty())
//                {
//                    CNSLOG_ERROR(QString("Error inherited format that can't find namespace!!"));
//                    return;
//                }
//                lastp = p;
//            } else{
//                def->setNamespace(p->getNamespace());
//                break;
//            }
//        }
//    }
//}

void CnsDefinitionCreator::fixMissingSystemName(CnsComDefinitionPtr def, CnsCreateComDefContext &context)
{
    if(def->getSystemName().isEmpty()){
        CnsComDefinitionPtr p = def;
        const QString * inherit = p->getInheritFullPath(context.getDefaultComFactory());
        while (inherit) {
            CnsComDefinitionPtr lastp = p;
            p = context.getDefaultComFactory()->findComDefinition(inherit);
            if(p.isNull()){
                context.getDefinitionCreator()->createComDefFromPath(*inherit, context, *lastp->getDirFullPath());
                p = context.getDefaultComFactory()->findComDefinition(inherit);
                if(p.isNull()){
                    CNSLOG_ERROR(QString("Can't find the parent obj %1!!").arg(*inherit));
                    def->setSystemName("SysDummy");
                    return;
                }
            }
            if(p->getSystemName().size()){
                def->setSystemName(p->getSystemName());
                return;
            }
            inherit = p->getInheritFullPath(context.getDefaultComFactory());
        }
        def->setSystemName("SysDummy");
    }
}

CnsComDefinitionPtr CnsDefinitionCreator::createComDef(const QString &comPath, const QDomElement &ele
     , const QString *dirFullPath, CnsCreateComDefContext &context)
{
    const QString * p = getAndRegisterFullPath(*dirFullPath, comPath, ele);

    PathDefState& pds = context.getPathDefStates(p);
    if(pds.testStacks(p)) {
        return context.getDefaultComFactory()->findComDefinition(p);
    }

    CnsComDefinitionPtr def = CnsComDefinitionPtr::create(dirFullPath);

    def->setDefFullPath(p);
    if(!parseComClass(ele, def)) {
        return nullptr;
    }

    registry()->registerComDefinition(def);
    context.getPathDefStates(p).setDef(def);
//    fixMissingNamespace(def, context);
    fixMissingSystemName(def, context);

    readComAllDefinitionProperties(comPath+combineRelativePath(ele), ele.childNodes(), def, context);

    processMethodData(def, context);
    pds.handleMergeAfterCreateDef();
    return def.staticCast<CnsComDefinition>();
}

//CnsComDefinitionPtr CnsDefinitionCreator::createComDef(const QString &comPath, const QDomElement &ele
//     , const QString *dirFullPath, CnsCreateComDefContext &context){
//    CnsComDefinitionPtr comDefinition = CnsComDefinitionPtr::create();

//    const QString * p = getAndRegisterFullPath(*dirFullPath, comPath, ele);
//    comDefinition->setDefFullPath(p);

//    comDefinition->setDirFullPath(dirFullPath);

//    if(!parseComClass(ele, comDefinition)) {
//        return nullptr;
//    }

//    registry()->registerComDefinition(comDefinition);
//    context.getPathDefStates(p).setDef(comDefinition);
//    readComAllDefinitionProperties(comPath, ele.childNodes(), comDefinition, context);

//    processMethodData(comDefinition, context);
//    return comDefinition;
//}

void CnsDefinitionCreator::processMethodData(CnsComDefinitionConstPtrRef def, CnsCreateComDefContext &context)
{
    assert(!def->getSystemName().isEmpty());
    QObject* s = getSystem(def->getSystemName());
    const QMetaObject *meta = s->metaObject();
    assert(def->getMethodDataHash().isEmpty());
    int j=0;
    for(int i = meta->methodOffset(); i < meta->methodCount(); i++)
    {
        QMetaMethod method = meta->method(i);
        QByteArray signature = method.methodSignature();
        MethodData md;
        md.stateIndex = j++;
        md.hasBreakoff = signature.indexOf("(uint8_t&,")!=-1;
        signature.replace("(uint8_t&,", "(");
        def->initMyMethodData(CnsComponent::registerMethodSignature(signature), md);
    }

    // handle inherit
    const QString *inherit = def->getInheritFullPath(context.getDefaultComFactory());
    if(!inherit && !def->getInheritPath().isEmpty())
    {
        CNSLOG_ERROR(QString("Error inherited format that can't find system name!!"));
        return;
    }
    const QString* ps = inherit;
    CnsComDefinitionPtr lastp = def;
    while (ps && ps->size()) {
        CnsComDefinitionPtr p = context.getDefaultComFactory()->findComDefinition(ps);
        if(p.isNull()){
            context.getDefinitionCreator()->createComDefFromPath(*ps, context, *lastp->getDirFullPath());
            p = context.getDefaultComFactory()->findComDefinition(ps);
            if(p.isNull())
            {
                assert(false);
            }
        }

        def->mergeMethodDataFromDef(p.data());
        lastp = p;
        ps = p->getInheritFullPath(context.getDefaultComFactory());
    }
    processChildrenMethodData(def, context);
}

void CnsDefinitionCreator::processChildrenMethodData(CnsComReferencePtr ref, CnsCreateComDefContext &context)
{
    CnsComDefinitionConstPtrRef def = context.getDefaultComFactory()->findComDefinition(ref->getObjFullPath(context.getDefaultComFactory()));
    processChildrenMethodData(def, context);
}

void CnsDefinitionCreator::processChildrenMethodData(CnsComDefinitionConstPtrRef def, CnsCreateComDefContext &context)
{
    // handle children
    QList<CnsComDefinitionPtr> inherits;
    if(def->getProperties().size())
        inherits.push_back(def);
    const QString* ps = def->getInheritFullPath(context.getDefaultComFactory());
    while (ps && ps->size()) {
        CnsComDefinitionPtr p = context.getDefaultComFactory()->findComDefinition(ps);
        if(p.isNull()){
            CNSLOG_ERROR(QString("Can't find the parent obj %1!!").arg(*ps));
            return;
        }
        inherits.push_front(p);
        ps = p->getInheritFullPath(context.getDefaultComFactory());
    }

    for(QList<CnsComDefinitionPtr>::const_iterator it = inherits.begin(); it != inherits.end(); it++)
    {
        //! 循环给定 com 的属性集合
        auto props = (*it)->getProperties();
        for (auto itr = props.cbegin(); itr != props.cend(); ++itr) {
            //! 获取定义的属性中的对象
            auto value = itr.value();
            if(!qstrcmp(value.typeName(), "CnsComReferencePtr"))
            {
                CnsComReferencePtr ref = value.value<CnsComReferencePtr>();
                const QString* refObjFullPath = ref->getObjFullPath(context.getDefaultComFactory());
                CnsComDefinitionPtr p = context.getDefaultComFactory()->findComDefinition(refObjFullPath);
                if(p.isNull()){
                    context.getDefinitionCreator()->createComDefFromPath(*refObjFullPath, context, *ref->getDirFullPath());
                    p = context.getDefaultComFactory()->findComDefinition(refObjFullPath);
                    if(p.isNull())
                    {
                        assert(false);
                    }
                }
                context.getPathDefStates(refObjFullPath).handleMergeParent(def->getDefFullPath(), context);

            } else if(!qstrcmp(value.typeName(), "QVariantList"))
            {
                QVariantList l = value.toList();
                for(QVariantList::iterator it = l.begin(); it != l.end(); it++)
                {
                    if(!qstrcmp(value.typeName(), "CnsComReferencePtr"))
                    {
                        processChildrenMethodData(it->value<CnsComReferencePtr>(), context);
                    }
                }
            } else if(!qstrcmp(value.typeName(), "QMap<QVariant, QVariant>"))
            {
                QMap<QVariant, QVariant> m = value.value<QMap<QVariant, QVariant>>();
                for(QMap<QVariant, QVariant>::iterator it = m.begin(); it != m.end(); it++)
                {
                    if(!qstrcmp(it.key().typeName(), "CnsComReferencePtr"))
                    {
                        processChildrenMethodData(it.key().value<CnsComReferencePtr>(), context);
                    }
                    if(!qstrcmp(it.value().typeName(), "CnsComReferencePtr"))
                    {
                        processChildrenMethodData(it.value().value<CnsComReferencePtr>(), context);
                    }
                }
            }
        }
    }
}
