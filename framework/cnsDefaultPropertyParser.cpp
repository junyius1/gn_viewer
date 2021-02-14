#include "cnsDefaultPropertyParser.h"
#include "cnsComReference.h"
#include <QCoreApplication>
#include <QDomElement>
#include <QLibrary>
#include <QDir>
#include <QDebug>
#include "cnsEnum.h"
#include "cnsvalue.h"
#include "cnsDefinitionReader.h"
#include "cnsDefaultComFactory.h"
#include "cnsdefinitioncreator.h"

CnsDefaultPropertyParser::CnsDefaultPropertyParser()
{
}

QVariant CnsDefaultPropertyParser::parse(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept
{
    QDomElement childEle;
    if((childEle = ele).tagName() == "list"
             || !(childEle = ele.firstChildElement("list")).isNull()) {

        return parseList(childEle, def, context);
    } else if((childEle = ele).tagName() == "map"
             || !(childEle = ele.firstChildElement("map")).isNull()) {

        return parseMap(childEle, def, context);
    } else if((childEle = ele).tagName() == "enum"
              || !(childEle = ele.firstChildElement("enum")).isNull()) {

         return parseEnum(childEle);
     } else if((childEle = ele).tagName() == "value"
             || (childEle = ele).hasAttribute("value")
             || !(childEle = ele.firstChildElement("value")).isNull()) {

         return parseValue(childEle);
    } else if((childEle = ele).tagName() == CNS_REF
             || (childEle = ele).hasAttribute(CNS_REF)
             || !(childEle = ele.firstChildElement(CNS_REF)).isNull()) {

        return parseRef(childEle, def);
    } /*else if((childEle = ele).tagName() == CNS_INHERIT
              || (childEle = ele).hasAttribute(CNS_INHERIT)
              || !(childEle = ele.firstChildElement(CNS_INHERIT)).isNull()) {

         return parseDef(childEle, def, context);
     } */else{
        return parseValue(ele);
    }
}

QVariant CnsDefaultPropertyParser::parseValue(const QDomElement &ele) const noexcept
{
    QString value;
    if(ele.hasAttribute(CNS_VALUE)) {
        value = ele.attribute(CNS_VALUE);
    }else{
        value = ele.text();
    }

    QString type;
    if(ele.hasAttribute(CNS_TYPE)) {
        type = ele.attribute(CNS_TYPE);
    }else{
        type = "QString";
    }

    bool autoCreate = true;
    if(ele.hasAttribute(CNS_AUTO_CREATE)) {
        autoCreate = !(ele.attribute(CNS_AUTO_CREATE)=="false");
    }

    CnsValuePtr val = CnsValuePtr::create();
    val->setType(type);
    val->setValue(value);
    val->setAutoCreate(autoCreate);
    return QVariant::fromValue(val);
}

QVariant CnsDefaultPropertyParser::parseRef(const QDomElement &ele, CnsComDefinition *def) const noexcept
{
    CnsComReferencePtr ref = CnsComReferencePtr::create(def->getDirFullPath());
    if(ele.hasAttribute(CNS_REF)) {
        ref->setObjPath(ele.attribute(CNS_REF));
    }else{
        ref->setObjPath(ele.text());
    }
    ref->setParentFullPath(def->getDefFullPath());
    return QVariant::fromValue(ref);
}

//QVariant CnsDefaultPropertyParser::parseDef(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept
//{
//    CnsComDefinitionPtr de = CnsComDefinitionPtr::create(def->getDirFullPath());
//    if(ele.hasAttribute(CNS_INHERIT)) {
//        de->setInheritPath(ele.attribute(CNS_INHERIT));
//    }else{
//        de->setInheritPath(ele.text());
//    }
//    if(ele.hasAttribute(CNS_OBJ_NAME)) {
//        de->setName(ele.attribute(CNS_OBJ_NAME));
//    }else{
//        if(ele.hasAttribute(CNS_OBJ_NAME_PREF))
//        {
//            de->setName(ele.attribute(CNS_OBJ_NAME_PREF));
//            de->setNew(true);
//        } else
//            de->setName(ele.text());
//    }
//    if(ele.hasAttribute(CNS_NAMESPACE)) {
//        de->setNamespace(ele.attribute(CNS_NAMESPACE));
//    }else{
//        de->setNamespace(ele.text());
//    }

//    const QString *inherit = de->getInheritFullPath(context.getDefaultComFactory());
//    if(!inherit && !de->getInheritPath().isEmpty())
//    {
//        CNSLOG_ERROR(QString("Error inherited format!"));
//    }
//    const QString* ps = inherit;
//    CnsComDefinitionPtr lastp = de;
//    while (ps && ps->size()) {
//        CnsComDefinitionPtr p = context.getDefaultComFactory()->findComDefinition(ps);
//        if(p.isNull()){
////            qCritical() << QString("Can't find the parent obj %1! Please put parent com before the inheritors!").arg(*ps);
//            context.getDefinitionCreator()->createObjOrComFromPath(*ps, false, *lastp->getDirFullPath());
//            p = context.getDefaultComFactory()->findComDefinition(ps);
//        }
//        if(p->getNamespace().isEmpty()){
//            ps = p->getInheritFullPath(context.getDefaultComFactory());
//            if(!ps && !p->getInheritPath().isEmpty())
//            {
//                CNSLOG_ERROR(QString("Error inherited format!"));
//            }
//            lastp = p;
//        } else{
//            de->setNamespace(p->getNamespace());
//            break;
//        }
//    }

//    context.getFilenameModule()->setObjName2Ns(de->getName(), de->getNamespace());
//    return QVariant::fromValue(de);
//}

QVariant CnsDefaultPropertyParser::parseList(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept
{
    QVariantList list;
    
    auto childNodes = ele.childNodes();
    for(int i = 0; i < childNodes.size(); ++i) {
		auto childEle = childNodes.at(i).toElement();
        if (childEle.isNull()) {
            continue;
        }
        //! 递归解析
        auto value = parse(childEle, def, context);
        if(!value.isValid()) {
            qWarning("in list tag of '%s' cannot parse!!!\n", qPrintable(childEle.tagName()));
            continue;
        }
        list << value;
	}
    
    return list;
}

QVariant CnsDefaultPropertyParser::parseMap(const QDomElement &ele, CnsComDefinition *def, CnsCreateComDefContext &context) const noexcept
{
    QMap<QVariant, QVariant> map;
    
	auto childNodes = ele.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto ele = childNodes.at(i).toElement();
		if (ele.isNull() || ele.tagName() != "entry")
			continue;
        QVariant mapKey, mapValue;
        if(ele.hasAttribute("key")) {
            mapKey = ele.attribute("key");
        }
        if(ele.hasAttribute("value")) {
            mapValue = ele.attribute("value");
        }
        
        auto entryChildEle = ele.firstChildElement();
        while(!entryChildEle.isNull()) {
            if(entryChildEle.tagName() == "key") {
                auto keyChildEle = entryChildEle.firstChildElement();
                if(keyChildEle.isNull()) {
                    mapKey = entryChildEle.text();
                }else{
                    //! 递归解析
                    mapKey = parse(keyChildEle, def, context);
                }
            }else if(entryChildEle.tagName() == "value") {
                auto valueChildEle = entryChildEle.firstChildElement();
                if(valueChildEle.isNull()) {
                    mapValue = entryChildEle.text();
                }else{
                    //! 递归解析
                    mapValue = parse(valueChildEle, def, context);
                }
            }
            
            entryChildEle = entryChildEle.nextSiblingElement();
        }
        
        if(mapKey.isValid() && mapValue.isValid()) {
            map.insert(mapKey, mapValue);
        }
	}
    
    return QVariant::fromValue(map);
}

QVariant CnsDefaultPropertyParser::parseEnum(const QDomElement &ele) const noexcept
{
    CnsEnumPtr e = CnsEnumPtr::create();
    if(ele.hasAttribute("scope")) {
        e->setScope(ele.attribute("scope"));
    }
    if(ele.hasAttribute("type")) {
        e->setType(ele.attribute("type"));
    }
    if(ele.hasAttribute("value")) {
        e->setValue(ele.attribute("value"));
    }
    auto childNodes = ele.childNodes();
    for(int i = 0; i < childNodes.length(); ++i) {
        auto childNode = childNodes.at(i);
        auto childEle = childNode.toElement();
        if(childEle.isNull()) {
            continue;
        }
        if(childEle.tagName() == "scope") {
            if(childEle.hasAttribute("name")) {
                e->setScope(childEle.attribute("name"));
            }
            auto text = childEle.text().simplified();
            if(!text.isEmpty()) {
                e->setScope(text);
            }
        } else if(childEle.tagName() == "type") {
            if(childEle.hasAttribute("name")) {
                e->setType(childEle.attribute("name"));
            }
            auto text = childEle.text().simplified();
            if(!text.isEmpty()) {
                e->setType(text);
            }
        } else if(childEle.tagName() == "value") {
            if(childEle.hasAttribute("name")) {
                e->setValue(childEle.attribute("name"));
            }
            auto text = childEle.text().simplified();
            if(!text.isEmpty()) {
                e->setValue(text);
            }
        }
    }
    return QVariant::fromValue(e);
}
