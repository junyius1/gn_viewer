#ifndef CNSENUM_H
#define CNSENUM_H

#include "cnsMacroGlobal.h"
#include <QString>
#include <QSharedPointer>

class CnsEnum {

public:
    explicit CnsEnum()
    {}
    ~CnsEnum() = default;
    
    QString scope() const noexcept
    { return m_scope; }
    void setScope(const QString &val) noexcept
    { m_scope = val; }
    
    QString type() const noexcept
    { return m_type; }
    void setType(const QString &val) noexcept
    { m_type = val; }
    
    QString value() const noexcept
    { return m_value; }
    void setValue(const QString &val) noexcept
    { m_value = val; }

private:
    QString m_scope;
    QString m_type;
    QString m_value;
};

CNS_DECL_METATYPE(CnsEnum)

#endif
