#ifndef CNSGLOBAL_H
#define CNSGLOBAL_H

#include <functional>

#include <QSharedPointer>
#include <QObject>
#include <QEvent>

#include "cnsMacroGlobal.h"
#include "log/cnslogger.h"

CNS_DECL_POINTER(QObject);
Q_DECLARE_METATYPE(QObjectPtr);
namespace CnsPrivate {

    template<typename From, typename To>
    struct CnsRegisterConverterHelper
    {
        using FromPtr = QSharedPointer<From>;
        using ToPtr = QSharedPointer<To>;
        static void registerConverter()
        {
            if (!QMetaType::hasRegisteredConverterFunction<FromPtr, ToPtr>()) {
                QMetaType::registerConverter<FromPtr, ToPtr>();
            }
        }
    };
}

template<typename From, typename To>
To cnsConverterQSharedPointerObject(const From &from)
{
    return from.template objectCast<typename To::Type>();
}

template<typename T>
void cnsRegisterComFactory(const char *typeName, const char *constRefTypeName)
{
    Q_ASSERT(typeName != nullptr && constRefTypeName != nullptr);
    using TPtr = QSharedPointer<T>;
    using QObjectPtr = QSharedPointer<QObject>;
    Q_STATIC_ASSERT_X(!std::is_pointer<T>::value, "cnsRegisterComFactory's template type must not be a pointer type");
    if (!QMetaType::hasRegisteredConverterFunction<TPtr, QObjectPtr>()) {
        QMetaType::registerConverter<TPtr, QObjectPtr>();
    }
    if (!QMetaType::hasRegisteredConverterFunction<QObjectPtr, TPtr>()) {
        QMetaType::registerConverter<QObjectPtr, TPtr>(cnsConverterQSharedPointerObject<QObjectPtr, TPtr>);
    }
    qRegisterMetaType<T*>(typeName);
    qRegisterMetaType<TPtr>(constRefTypeName);
}

template<typename From, typename To>
void cnsRegisterComFactory(const char *typeName, const char *constRefTypeName)
{
    cnsRegisterComFactory<From>(typeName, constRefTypeName);
    Q_STATIC_ASSERT_X(!std::is_pointer<To>::value, "cnsRegisterComFactory's template type must not be a pointer type");
    CnsPrivate::CnsRegisterConverterHelper<From, To>::registerConverter();
    using FromPtr = QSharedPointer<From>;
    using ToPtr = QSharedPointer<To>;
    if (!QMetaType::hasRegisteredConverterFunction<ToPtr, FromPtr>()) {
        QMetaType::registerConverter<ToPtr, FromPtr>(cnsConverterQSharedPointerObject<ToPtr, FromPtr>);
    }
}

struct MethodData
{
    MethodData();
    int stateIndex;
    uint8_t hasBreakoff;
};

enum HasMethodType{
    HAS_BUT_ONLY_CHILD=-1,
    UNINIT=-2,
    HAS_ALL=-3
};

typedef QHash<const QByteArray*, MethodData> METHODDATAHASH;
typedef QHash<int, QVariant> QIVHASH;
typedef QSet<const QString*> EXECSTACK;

#endif
