#ifndef CNSCOMPONENT_H
#define CNSCOMPONENT_H

#include <QObject>
#include <QMap>
#include "cnsMacroGlobal.h"
#include <QSharedPointer>
#include "cnsGlobal.h"
#include <QReadWriteLock>
#include <QSet>
#include "cnsComDefinition.h"

class CnsDefaultComFactory;

extern QString EMPTY_STRING;

#define M_EXEC(TYPE, prefix) bool m_##prefix##exec(CnsComponentPtr comPtr, const char *member, \
    const QStringList &nsn = QStringList(), Qt::ConnectionType type = Qt::DirectConnection, \
    const char *combine="combineParams",QGenericReturnArgument ret = QGenericReturnArgument(), \
    TYPE *val1 = nullptr, \
    QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(), \
    QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(), \
    QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(), \
    QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument()) \
{ \
EXECSTACK stack; \
QByteArray *signature = get_signature(combine, ret, Q_ARG(CnsComponentPtr, comPtr), Q_ARG(TYPE&, *val1), val2, val3, val4, val5, val6, val7, val8, val9);\
stack.insert(_objFullPath); \
uint8_t breakoff = 0; \
__##prefix##exec(stack, breakoff, comPtr, combine, signature, nsn, type, QGenericReturnArgument(), \
              Q_ARG(TYPE&, *val1), val2, val3, val4, val5, val6, val7, val8, val9); \
stack.clear(); \
stack.insert(_objFullPath); \
breakoff = 0; \
signature = get_signature(member, ret, Q_ARG(CnsComponentPtr, comPtr), Q_ARG(TYPE&, *val1), val2, val3, val4, val5, val6, val7, val8, val9);\
return __##prefix##exec(stack, breakoff, comPtr, member, signature, nsn, type, QGenericReturnArgument(ret.name(), ret.data()), \
      Q_ARG(TYPE&, *val1), val2, val3, val4, val5, val6, val7, val8, val9); \
}

CNS_FORWARD_DECL_CLASS(CnsComponent)
class CnsComponent: public QObject
{
    Q_OBJECT
    typedef QList<QSharedPointer<CnsComponent>> CHILDRENLIST;
    typedef QHash<QString, QSharedPointer<CnsComponent>> CHILDRENHASH;
public:
//    Q_INVOKABLE CnsComponent(CnsDefaultComFactory* comFactory):_comFactory(comFactory) {}
    Q_INVOKABLE CnsComponent() {}
    virtual ~CnsComponent() {}
    void setSystem(QObject* obj)
    {
        _system = obj;
    }
    inline QObject* getSystem()
    {
        return _system;
    }
    inline void setSorts(const QStringList& sorts)
    {
        _sorts = sorts;
    }
    inline const QString& getNamespace()
    {
        return _sorts.isEmpty()?EMPTY_STRING:_sorts[0];
    }
    inline const QStringList& getSorts()
    {
        return _sorts;
    }
    inline const QStringList& getNsN()
    {
        return getComDef()->getNsN();
    }
//    inline const QStringList& getNss()
//    {
//        return getComDef()->getNss();
//    }
    inline void setName(const QString& name)
    {
        _name = name;
    }
    inline const QString& getName()
    {
        return _name;
    }
    void setObjFullPath(const QString* objFullPath )
    {
        _objFullPath = objFullPath ;
    }
    inline const QString* getObjFullPath()
    {
        return _objFullPath;
    }
//    void setDefFullPath(const QString* defFullPath)
//    {
//        _defFullPath = defFullPath;
//    }
//    const QString* getDefFullPath()
//    {
//        return _defFullPath;
//    }
//    enum ChildHasMethodType{
//        UNINITIALIZED=0,
//        HAS,
//        NONE,
//    };
    //descend exec from high to low level
    bool dexec(CnsComponentPtr comPtr, const char *member, const QStringList &nsn = QStringList(), \
              Qt::ConnectionType type = Qt::DirectConnection, \
              QGenericReturnArgument ret = QGenericReturnArgument(), \
              QGenericArgument val1 = QGenericArgument(), \
              QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(), \
              QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(), \
              QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(), \
              QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument())
    {
        EXECSTACK stack;
        stack.insert(_objFullPath);
        uint8_t breakoff = 0;
        QByteArray *signature = get_signature(member, ret, Q_ARG(CnsComponentPtr, comPtr), val1, val2, val3, val4, val5, val6, val7, val8, val9);
        return __dexec(stack, breakoff, comPtr, member, signature, nsn, type, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                val6, val7, val8, val9);
    }

    //merge or multi-
    M_EXEC(QIVHASH, d)

    typedef bool (CnsComponent::*EXEC)(EXECSTACK &stack, uint8_t &breakoff, CnsComponentPtr comPtr, \
              const char *member, QByteArray *signature, const QStringList &nsn, \
              Qt::ConnectionType type, QGenericReturnArgument ret, \
              QGenericArgument val1, \
              QGenericArgument val2, QGenericArgument val3, \
              QGenericArgument val4, QGenericArgument val5, \
              QGenericArgument val6, QGenericArgument val7, \
              QGenericArgument val8, QGenericArgument val9);

    bool __dexec(EXECSTACK &stack, uint8_t &breakoff, CnsComponentPtr comPtr, \
              const char *member, QByteArray *signature, const QStringList &nsn, \
              Qt::ConnectionType type, QGenericReturnArgument ret, \
              QGenericArgument val1, \
              QGenericArgument val2, QGenericArgument val3, \
              QGenericArgument val4, QGenericArgument val5, \
              QGenericArgument val6, QGenericArgument val7, \
              QGenericArgument val8, QGenericArgument val9);

    inline bool dexec(CnsComponentPtr comPtr, const char *member, QGenericArgument val1,
              const QStringList &nsn = QStringList(), \
              QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(), \
              QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(), \
              QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(), \
              QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument())
    {
        return dexec(comPtr, member, nsn, Qt::DirectConnection, QGenericReturnArgument(), val1, val2, val3, val4, val5, \
              val6, val7, val8, val9);
    }

    bool dexecInNs(CnsComponentPtr comPtr, const char *member, \
              const QString& name = "", QGenericArgument val1 = QGenericArgument(), \
              QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(), \
              QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(), \
              QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(), \
              QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument())
    {
        QStringList ns;
        if(!getSorts().isEmpty())
        {
            ns.push_back(getSorts()[0] + '/' + name);
        }
        return dexec(comPtr, member, ns, Qt::DirectConnection, QGenericReturnArgument(), val1, val2, val3, val4, val5, \
              val6, val7, val8, val9);
    }

    //ascent exec from low to high level
    bool aexec(CnsComponentPtr comPtr, const char *member, const QStringList &nsn = QStringList(), \
              Qt::ConnectionType type = Qt::DirectConnection, \
              QGenericReturnArgument ret = QGenericReturnArgument(), QGenericArgument val1 = QGenericArgument(), \
              QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(), \
              QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(), \
              QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(), \
              QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument())
    {
        EXECSTACK stack;
        stack.insert(_objFullPath);
        uint8_t breakoff = 0;
        QByteArray *signature = get_signature(member, ret, Q_ARG(CnsComponentPtr, comPtr), val1, val2, val3, val4, val5, val6, val7, val8, val9);
        return __aexec(stack, breakoff, comPtr, member, signature, nsn, type, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                val6, val7, val8, val9);
    }

    //merge or multi-
    M_EXEC(QIVHASH, a)

//    bool execChildWithReturnInfo(EXECSTACK &stack, uint8_t &breakoff, const char *member, const QStringList &ns, \
//              const QString& name, Qt::ConnectionType type,
//              EXEC pf, QGenericReturnArgument ret, \
//              QGenericArgument val1, \
//              QGenericArgument val2, QGenericArgument val3, \
//              QGenericArgument val4, QGenericArgument val5, \
//              QGenericArgument val6, QGenericArgument val7, \
//              QGenericArgument val8, QGenericArgument val9);

    bool execChild(EXECSTACK &stack, uint8_t &breakoff, const char *member, QByteArray *signature, \
              const QStringList &nsn, Qt::ConnectionType type,
              EXEC pf, QGenericReturnArgument ret, \
              QGenericArgument val1, \
              QGenericArgument val2, QGenericArgument val3, \
              QGenericArgument val4, QGenericArgument val5, \
              QGenericArgument val6, QGenericArgument val7, \
              QGenericArgument val8, QGenericArgument val9);

    void conntype_exec(CnsComponentPtr comPtr, uint8_t &breakoff, const char *member, Qt::ConnectionType type,
              const MethodData& methodData, QGenericReturnArgument ret, \
              QGenericArgument val1, \
              QGenericArgument val2, QGenericArgument val3, \
              QGenericArgument val4, QGenericArgument val5, \
              QGenericArgument val6, QGenericArgument val7, \
              QGenericArgument val8, QGenericArgument val9);


    bool __aexec(EXECSTACK &stack, uint8_t &breakoff, CnsComponentPtr comPtr, const char *member, QByteArray *signature, \
              const QStringList &nsn, Qt::ConnectionType type,\
              QGenericReturnArgument ret, \
              QGenericArgument val1, \
              QGenericArgument val2, QGenericArgument val3, \
              QGenericArgument val4, QGenericArgument val5, \
              QGenericArgument val6, QGenericArgument val7, \
              QGenericArgument val8, QGenericArgument val9);

    inline bool aexec(CnsComponentPtr comPtr, const char *member, QGenericArgument val1,
               const QStringList &nsn = QStringList(), \
              QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(), \
              QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(), \
              QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(), \
              QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument())
    {
        return aexec(comPtr, member, nsn, Qt::DirectConnection, QGenericReturnArgument(), val1, val2, val3, val4, val5, \
              val6, val7, val8, val9);
    }

    bool aexecInNs(CnsComponentPtr comPtr, const char *member, \
              const QString& name = "", QGenericArgument val1 = QGenericArgument(), \
              QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(), \
              QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(), \
              QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(), \
              QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument())
    {
        QStringList ns;
        if(!getSorts().isEmpty())
        {
            ns.push_back(getSorts()[0] + '/' + name);
        }
        return aexec(comPtr, member, ns, Qt::DirectConnection, QGenericReturnArgument(), val1, val2, val3, val4, val5, \
              val6, val7, val8, val9);
    }

    void getAllProperty(const char *name, QVariantList &allProperty);
    void __getAllProperty(EXECSTACK &stack, const char *name, QVariantList &allProperty);

    bool addRef(const QString &key, QSharedPointer<CnsComponent> ref);
    QVariant property(const QString &propName, const QStringList &ns=QStringList(), const QString &name="", CnsComponentPtr comPtr=nullptr);
    inline QVariant property(const QString &propName, const QString &ns, const QString &name="", CnsComponentPtr comPtr=nullptr)
    {
        return property(propName, QStringList() << ns, name, comPtr);
    }
    QVariant __property(EXECSTACK &stack, const QStringList &ns, const QString &name, const QString &propName, CnsComponentPtr comPtr=nullptr);
//    CnsComponentPtr getComFromNs(CnsComponentPtr comPtr, const char *nameSpace) const;
    QVariant propertyInNs(const char *propName, CnsComponentPtr comPtr=nullptr);
    inline bool setPropertyInNs(const QVariant &value, const QString &propName, const QString &name="")
    {
        return setProperty(value, propName, name, getSorts());
    }
    bool setProperty(const QVariant &value, const QString &propName, const QString &name="", const QStringList &ns=QStringList());
    bool __setProperty(EXECSTACK &stack, const QVariant &value, const QString &propName, const QString &name, const QStringList &ns);
    bool pathContains(const QStringList &nsn);
	bool pathContains(const QStringList &ns, const QString &name);
    const MethodData& getMethodState(QByteArray *member);
    void setMethodState(const QByteArray *member, const MethodData& state);
//    bool setProperty_ts(const char *name, const QVariant &value);//thread-safe
//    QVariant property_ts(const char *propName);//thread-safe
    bool addProperty_ts(const char *name, const QVariant &value)
    {
        auto it = _userData.find(name);
        if(it == _userData.end())
        {
            _userData[name] = value;
            return true;
        }
        _userData[name] = value;
        return false;
    }
    inline void setProperty_ts(const char *name, const QVariant &value)
    {
        _userData[name] = value;
    }
    QVariant property_ts(const char *propName)
    {
        auto it = _userData.find(propName);
        if(it == _userData.end())
        {
            return QVariant();
        }
        return it.value();
    }
    inline QVariantHash& getUserData()
    {
        return _userData;
    }
    inline void setUserData(const QString &key, const QVariant& value)
    {
        _userData[key] = value;
    }

    inline CnsComDefinitionConstPtrRef getComDef()
    {
        return _comDef;
    }
    inline void setComDef(CnsComDefinitionConstPtrRef comDef)
    {
        _comDef = comDef;
    }

    CnsComponentPtr findChildren(const QString &propName)
    {
        EXECSTACK stack;
        return __findChildren(stack, propName);
    }
    CnsComponentPtr __findChildren(EXECSTACK &stack, const QString &propName);

    static QHash<QByteArray, QByteArray *> _methodSignature;
    static QByteArray *getMethodSignature(const QByteArray &methodSignature);
    static QByteArray *registerMethodSignature(const QByteArray &methodSignature);
    static QByteArray *get_signature(const char *member,
               QGenericReturnArgument ret,
               QGenericArgument val0,
               QGenericArgument val1,
               QGenericArgument val2,
               QGenericArgument val3,
               QGenericArgument val4,
               QGenericArgument val5,
               QGenericArgument val6,
               QGenericArgument val7,
               QGenericArgument val8,
               QGenericArgument val9);

private:
    CHILDRENLIST _childrenList;
    CHILDRENHASH _childrenHash;
    QObject* _system;
    QString _name;
    QStringList _sorts;
    QVariantHash _userData;
    const QString *_objFullPath;
//    const QString *_defFullPath;
//    const QString *_dirFullPath;
    METHODDATAHASH _methodHash;
//    QReadWriteLock _methodHashLock;
//    QReadWriteLock _propertyLock;
    CnsComDefinitionPtr _comDef;
};

CNS_DECL_METATYPE(CnsComponent)

#endif // CNSCOMPONENT_H
