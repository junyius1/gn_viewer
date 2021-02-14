#ifndef CNSMACROGLOBAL_H
#define CNSMACROGLOBAL_H

//#include <QtCore/qglobal.h>

#ifndef QT_STATIC
# if defined(CNS_LIBRARY)
#  define CNS_EXPORT Q_DECL_EXPORT
# else
#  define CNS_EXPORT Q_DECL_IMPORT
# endif
#else
# define CNS_EXPORT
#endif

#define CNS_DECL_METATYPE(Class) \
    CNS_DECL_POINTER(Class) \
    Q_DECLARE_METATYPE(Class##Ptr) \
    Q_DECLARE_METATYPE(Class*)

#define CNS_DECL_POINTER(Class) \
    using Class##Ptr = QSharedPointer<Class>; \
    using Class##ConstPtrRef = const QSharedPointer<Class> &;

#define CNS_FORWARD_DECL_CLASS(Class) \
    class Class; \
    CNS_DECL_POINTER(Class)

#define CNS_DECL_PRIVATE_DATA(Class) \
    struct Class##Data {

#define CNS_DECL_PRIVATE_DATA_END };

#define CNS_DECL_PRIVATE(Class) \
    QScopedPointer<Class##Data> d; \
    friend struct Class##Data;

#define CNS_FORWARD_DECL_PRIVATE_DATA(Class) \
    struct Class##Data;

#define CNS_NEW_PRIVATE_DATA(Class) \
    d.reset(new Class##Data());

#define CNS_SYS_REGISTER(Class) \
    const int Class##_Static_Init = []() -> int {\
    registerSystem(#Class, new Class());\
    return 0;}();

#define CNS_INIT(Class) \
    const int Class##_Static_Init = []() -> int {

#define CNS_INIT_END \
    return 0;}();

#define CNS_MACRO_STR(m) #m
#define DIR_NSN_SEPARATOR "/./"

# define CNS_FIRST_TYPE_NAME(Class, ...) CNS_MACRO_STR(Class)
# define CNS_FIRST_TYPE_NAME_CONST_REF(Class, ...) CNS_MACRO_STR(Class##ConstPtrRef)
# define CNS_REGISTER_COM_FACTORY2(Class1, Class2) CNS_REGISTER_COM_FACTORY_IMPL2(Class1, Class2)
# define CNS_REGISTER_COM_FACTORY_IMPL2(Class1, Class2) \
    cnsRegisterComFactory<Class1, Class2>(CNS_FIRST_TYPE_NAME(Class1), CNS_FIRST_TYPE_NAME_CONST_REF(Class1));
# define CNS_REGISTER_COM_FACTORY(Class1) CNS_REGISTER_COM_FACTORY_IMPL(Class1)
# define CNS_REGISTER_COM_FACTORY_IMPL(Class1) \
    cnsRegisterComFactory<Class1>(CNS_FIRST_TYPE_NAME(Class1), CNS_FIRST_TYPE_NAME_CONST_REF(Class1));

//! Q_CLASSINFO
#define CNS_AUTO_CREATE "autoCreate"
#define CNS_SYSTEM "system"
//!< Q_CLASSINFO

#define CNS_CNS "cns"
#define CNS_COM "com"
#define CNS_PROPERTY "property"
#define CNS_CONNECT "connect"
#define CNS_CREATE "create"
#define CNS_NAME "name"
#define CNS_CLASS "class"
#define CNS_OBJ_NAME_PREF "objNamePref"
#define CNS_TYPE "type"
#define CNS_VALUE "value"
#define CNS_REF "ref"
#define CNS_INHERIT "inherit"
#define CNS_NAMESPACE "namespace"

#endif
