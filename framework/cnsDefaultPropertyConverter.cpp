#include "cnsDefaultPropertyConverter.h"

#include <QMetaEnum>
#include <QDebug>
#include "cnsEnum.h"
#include "cnsComReference.h"
#include "cnsDefaultComFactory.h"
#include "cnsvalue.h"

CNS_DECL_PRIVATE_DATA(CnsDefaultPropertyConverter)
CnsDefaultComFactory *resolver;
CNS_DECL_PRIVATE_DATA_END

CnsDefaultPropertyConverter::CnsDefaultPropertyConverter()
{
    CNS_NEW_PRIVATE_DATA(CnsDefaultPropertyConverter)
}

//QVariant CnsDefaultPropertyConverter::convertDef(const QVariant &value) const noexcept
//{
//    static uint index =0;
//    auto def = value.value<CnsComDefinitionPtr>();
//    if (!def) {

//        qCritical() << "cannot inject beanReference";
//        return QVariant();
//    }

//    CnsComponentPtr objVar;

//    if(def->isNew()){
//        objVar = resolver()->newComObj(*def->getInheritFullPath(resolver()), def->getName()+QString().setNum(index++));
//    } else
//        objVar = resolver()->getComObj(def->getInheritFullPath(resolver()), def->getDefFullPath(resolver()));
//    if (objVar.isNull()) {

//        qCritical() << "cannot get obj from obj reference";
//        return QVariant();
//    }

//    QVariant var;
//    var.setValue(objVar);
//    QString typeName = QString("%1Ptr").arg(objVar->metaObject()->className());
//    if(!var.convert(QMetaType::type(typeName.toLocal8Bit()))) {
//        qCritical() << QString("failed convert QObjectPtr to '%1'").arg(typeName);
//        return QVariant();
//    }
//    return var;
//}

QVariant CnsDefaultPropertyConverter::convertRef(const QVariant &value) const noexcept
{
    auto ref = value.value<CnsComReferencePtr>();
    if (!ref) {

        CNSLOG_ERROR(QString("cannot inject beanReference"));
        return QVariant();
	}

    auto objVar = resolver()->resolveComReferenceToQVariant(ref);
    if (objVar.isNull()) {

        CNSLOG_ERROR(QString("cannot get obj from obj reference %1").arg(*ref->getObjFullPath(resolver())));
        return QVariant();
    }

    QVariant var;
    var.setValue(objVar);
    QString typeName = QString("%1Ptr").arg(objVar->metaObject()->className());
    if(!var.convert(QMetaType::type(typeName.toLocal8Bit()))) {
        CNSLOG_ERROR(QString("failed convert QObjectPtr to '%1'").arg(typeName));
        return QVariant();
    }
    return var;
}

QVariant CnsDefaultPropertyConverter::convertEnum(const QVariant &value) const noexcept
{
    auto e = value.value<CnsEnumPtr>();
    if(e.isNull()) {
        qCritical() << "cannot inject enum";
        return QVariant();
    }
    const QMetaObject *mo = nullptr;
    if(e->scope() == "Qt") {
        mo = qt_getQtMetaObject();
    } else {
        mo = QMetaType::metaObjectForType(QMetaType::type(qPrintable(e->scope())));
    }
    if(mo == nullptr) {
        qCritical("cannot get meta object for type: %s\n", qPrintable(e->scope()));
        return QVariant();
    }
    auto enumIndex = mo->indexOfEnumerator(qPrintable(e->type()));
    if(enumIndex == -1) {
        qCritical("not exists enum '%s' for type '%s'", qPrintable(e->type()), qPrintable(e->scope()));
        return QVariant();
    }
    auto me = mo->enumerator(enumIndex);
    return QVariant(me.keysToValue(qPrintable(e->value())));
}

QVariant CnsDefaultPropertyConverter::convertList(const QVariant &value) const noexcept
{
    QVariantList result;
    
    auto list = value.value<QVariantList>();
    
    for(const auto &var : list) {
        result << convert(resolver(), var);
    }
    
    return result;
}

QVariant CnsDefaultPropertyConverter::convertMap(const QVariant &value) const noexcept
{
    QMap<QVariant, QVariant> result;
    
    auto map = value.value<QMap<QVariant, QVariant>>();
    
    QMapIterator<decltype (map)::key_type, decltype (map)::mapped_type> itr(map);
    while(itr.hasNext()) {
        auto item = itr.next();
        auto key = item.key();
        auto value = item.value();
        
        QVariant resultKey, resultValue;
        
        resultKey = convert(resolver(), key);
        resultValue = convert(resolver(), value);
        
        if(resultKey.isValid() && resultValue.isValid()) {
            result.insert(resultKey, resultValue);
        }
    }
    
    return QVariant::fromValue(result);
}

QVariant CnsDefaultPropertyConverter::convertValue(const QVariant &value) const noexcept
{
    auto var = value.value<CnsValuePtr>();
    if (!var) {

        qCritical() << "cannot convert to CnsValuePtr";
        return QVariant();
    }
    return var->getValueVariant();
}

CnsDefaultComFactory *CnsDefaultPropertyConverter::resolver() const noexcept
{
    return d->resolver;
}

QVariant CnsDefaultPropertyConverter::convert(CnsDefaultComFactory *resolver
                                              , const QVariant &value) const noexcept
{
    d->resolver = resolver;
    if(value.canConvert<CnsValuePtr>()) {
        return convertValue(value);
    } else if(value.canConvert<CnsComReferencePtr>()) {
        return convertRef(value);
    } else if(value.canConvert<CnsEnumPtr>()) {
        return convertEnum(value);
    } else if(value.canConvert<QVariantList>()) {
        return convertList(value);
    } else if(value.canConvert<QMap<QVariant, QVariant>>()) {
        return convertMap(value);
    } else{
        //default is QString ""
        return value;
    }
}
