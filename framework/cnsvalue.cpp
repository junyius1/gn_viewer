#include "cnsvalue.h"
#include <QVariant>
#include <QDebug>

CnsValue::CnsValue()
{
    _isAutoCreate = false;
}

static QVariant defaultVariant = QVariant(true);

QVariant CnsValue::getValueVariant()
{
    if(!_isAutoCreate)
    {
        return defaultVariant;
    }
    if(_type == "int")
    {
        return _value.toInt();
    } else if(_type == "QString")
    {
        return QVariant::fromValue(_value);
    }

    int typeId = QMetaType::type(_type.toLatin1());
    const QMetaObject *type = QMetaType::metaObjectForType(typeId);
    QObject * vp =type->newInstance();

    QObjectPtr p;
    p.reset(vp);

    QVariant var;
    var.setValue(p);
    QString typeName = QString("%1Ptr").arg(_type);
    if(!var.convert(QMetaType::type(typeName.toLocal8Bit()))) {
        qCritical() << QString("CnsValue failed convert QVariant to '%1'").arg(_type);
        return QVariant();
    }
    return var;
}
