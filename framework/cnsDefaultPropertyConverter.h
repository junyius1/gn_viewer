#ifndef CNSDEFAULTPROPERTYCONVERTER_H
#define CNSDEFAULTPROPERTYCONVERTER_H

#include <QVariant>
#include "cnsMacroGlobal.h"

class CnsDefaultComFactory;

CNS_FORWARD_DECL_PRIVATE_DATA(CnsDefaultPropertyConverter)

class CnsDefaultPropertyConverter : public QObject
{
    Q_OBJECT
public:
    explicit CnsDefaultPropertyConverter();
    
protected:
    QVariant convertRef(const QVariant &value) const noexcept;
//    QVariant convertDef(const QVariant &value) const noexcept;
    QVariant convertValue(const QVariant &value) const noexcept;
    QVariant convertEnum(const QVariant &value) const noexcept;
    QVariant convertList(const QVariant &value) const noexcept;
    QVariant convertMap(const QVariant &value) const noexcept;

    CnsDefaultComFactory *resolver() const noexcept;
 public:
    QVariant convert(CnsDefaultComFactory *resolver, const QVariant &value) const noexcept;

private:
    CNS_DECL_PRIVATE(CnsDefaultPropertyConverter)
};

CNS_DECL_POINTER(CnsDefaultPropertyConverter)

#endif
