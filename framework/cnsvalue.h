#ifndef CNSVALUE_H
#define CNSVALUE_H

#include <QString>
#include "cnsComponent.h"

class CnsValue
{
private:
    QString _type;
    QString _value;
    bool _isAutoCreate;
public:
    CnsValue();
    void setType(const QString& type)
    {
        _type = type;
    }
    void getValue(const QString& value)
    {
        _value = value;
    }
    void setValue(const QString& value)
    {
        _value = value;
    }

    bool isAutoCreate() const noexcept
    { return _isAutoCreate; }
    void setAutoCreate(bool val) noexcept
    { _isAutoCreate = val; }

    QVariant getValueVariant();
};

CNS_DECL_METATYPE(CnsValue)
#endif // CNSVALUE_H
