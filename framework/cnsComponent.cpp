#include "cnsComponent.h"
#include <QVariant>
#include <QDebug>
#include "cnsDefaultComFactory.h"
#include <QThread>

CNS_INIT(CnsComponent)
CNS_REGISTER_COM_FACTORY(CnsComponent)
CNS_INIT_END

QString EMPTY_STRING;

QHash<QByteArray, QByteArray *> CnsComponent::_methodSignature;

MethodData::MethodData(): stateIndex(HasMethodType::UNINIT), hasBreakoff(0)
{}

bool testObjFullPath(EXECSTACK &stack, const QString *objFullPath)
{
    if(stack.find(objFullPath) == stack.end())
    {
        stack.insert(objFullPath);
        return true;
    }
    return false;
}

//bool CnsComponent::execChildWithReturnInfo(EXECSTACK &stack, uint8_t &breakoff, const MEMBERORSIGNATURE &mors, const QStringList &ns, \
//          const QString& name, Qt::ConnectionType type,
//          EXEC pf, QGenericReturnArgument ret, \
//          QGenericArgument val1, \
//          QGenericArgument val2, QGenericArgument val3, \
//          QGenericArgument val4, QGenericArgument val5, \
//          QGenericArgument val6, QGenericArgument val7, \
//          QGenericArgument val8, QGenericArgument val9)
//{
//    bool rt = false;
//    for(CHILDRENLIST::iterator it = _childrenList.begin(); it != _childrenList.end(); it ++ )
//    {
//        //break ref ref cycle, no same name namespace nearby(may optimize) and simplify interface
//        QSharedPointer<CnsComponent> p = *it;
//        if(testObjFullPath(stack, p->getObjFullPath())){
//            rt = (p.data()->*pf)(stack, breakoff, p, mors, ns, name, type, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, val6, \
//                                                 val7, val8, val9) || rt;
//        }
//    }
//    return rt;
//}

bool CnsComponent::execChild(EXECSTACK &stack, uint8_t &breakoff, const char *member, QByteArray *signature, \
          const QStringList &nsn, Qt::ConnectionType type,
          EXEC pf, QGenericReturnArgument ret, \
          QGenericArgument val1, \
          QGenericArgument val2, QGenericArgument val3, \
          QGenericArgument val4, QGenericArgument val5, \
          QGenericArgument val6, QGenericArgument val7, \
          QGenericArgument val8, QGenericArgument val9)
{
    for(CHILDRENLIST::iterator it = _childrenList.begin(); it != _childrenList.end(); it ++ )
    {
        //break ref ref cycle, no same name namespace nearby(may optimize) and simplify interface
        QSharedPointer<CnsComponent> p = *it;
        if(testObjFullPath(stack, p->getObjFullPath())){
            (p.data()->*pf)(stack, breakoff, p, member, signature, nsn, type, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, val6, \
                                                 val7, val8, val9);
            if(breakoff)
            {
                break;
            }
        }
    }
    return true;
}

QByteArray* CnsComponent::get_signature(const char *member,
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
           QGenericArgument val9)
{
    QVarLengthArray<char, 512> sig;
    uint len = qstrlen(member);
    if (len <= 0)
        return nullptr;
    sig.append(member, static_cast<int>(len));
    sig.append('(');

    const char *typeNames[] = {ret.name(), val0.name(), val1.name(), val2.name(), val3.name(),
                               val4.name(), val5.name(), val6.name(), val7.name(), val8.name(),
                               val9.name()};

    int paramCount;
    enum { MaximumParamCount = 11 }; // up to 10 arguments + 1 return value
    for (paramCount = 1; paramCount < MaximumParamCount; ++paramCount) {
        len = qstrlen(typeNames[paramCount]);
        if (len <= 0)
            break;
        sig.append(typeNames[paramCount], static_cast<int>(len));
        sig.append(',');
    }
    if (paramCount == 1)
        sig.append(')'); // no parameters
    else
        sig[sig.size() - 1] = ')';
    sig.append('\0');

    QByteArray norm = QMetaObject::normalizedSignature(sig.constData());
    return CnsComponent::getMethodSignature(norm);
}

inline int interpretStateIndex(int stateIndex)
{
    return stateIndex<=HAS_ALL?-stateIndex+HAS_ALL:stateIndex;
}

void CnsComponent::conntype_exec(CnsComponentPtr comPtr, uint8_t &breakoff, const char *member, Qt::ConnectionType type,
          const MethodData& methodData, QGenericReturnArgument ret, \
          QGenericArgument val1, \
          QGenericArgument val2, QGenericArgument val3, \
          QGenericArgument val4, QGenericArgument val5, \
          QGenericArgument val6, QGenericArgument val7, \
          QGenericArgument val8, QGenericArgument val9)
{
    if (type == Qt::AutoConnection) {
        type = QThread::currentThread() == thread()
                         ? Qt::DirectConnection
                         : Qt::QueuedConnection;
    }
    if(type == Qt::DirectConnection)
    {
        void *param[11];
        param[0] = QGenericReturnArgument(ret.name(), ret.data()).data();
        if(methodData.hasBreakoff){
            param[1] = Q_ARG(uint8_t&, breakoff).data();
            param[2] = Q_ARG(CnsComponentPtr, comPtr).data();
            param[3] = val1.data();
            param[4] = val2.data();
            param[5] = val3.data();
            param[6] = val4.data();
            param[7] = val5.data();
            param[8] = val6.data();
            param[9] = val7.data();
        } else {
            param[1] = Q_ARG(CnsComponentPtr, comPtr).data();
            param[2] = val1.data();
            param[3] = val2.data();
            param[4] = val3.data();
            param[5] = val4.data();
            param[6] = val5.data();
            param[7] = val6.data();
            param[8] = val7.data();
            param[9] = val8.data();
        }
        getSystem()->metaObject()->d.static_metacall(getSystem(), QMetaObject::InvokeMetaMethod, interpretStateIndex(methodData.stateIndex), param);
    } else{
        if(methodData.hasBreakoff){
            QMetaObject::invokeMethod(getSystem(), member, type, QGenericReturnArgument(ret.name(), ret.data()), Q_ARG(uint8_t&, breakoff),
                       Q_ARG(CnsComponentPtr, comPtr), val1, val2, val3, val4, val5, val6, \
                                            val7, val8);
        } else{
            QMetaObject::invokeMethod(getSystem(), member, type, QGenericReturnArgument(ret.name(), ret.data()), Q_ARG(CnsComponentPtr, comPtr), val1, val2, val3, val4, val5, val6, \
                                            val7, val8, val9);
        }
    }
}

bool CnsComponent::__aexec(EXECSTACK &stack, uint8_t &breakoff, CnsComponentPtr comPtr, const char* member, QByteArray *signature, \
          const QStringList &nsn, Qt::ConnectionType type, QGenericReturnArgument ret, \
          QGenericArgument val1, QGenericArgument val2, QGenericArgument val3, \
          QGenericArgument val4, QGenericArgument val5, QGenericArgument val6, QGenericArgument val7, \
          QGenericArgument val8, QGenericArgument val9)
{
    const MethodData& test = getMethodState(signature);
    if(breakoff) return true;
    if(test.stateIndex == HAS_BUT_ONLY_CHILD)
    {
        execChild(stack, breakoff, member, signature, nsn, type, &CnsComponent::__aexec, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                  val6, val7, val8, val9);
        return true;
    } else if(test.stateIndex <= HAS_ALL)
    {
        execChild(stack, breakoff, member, signature, nsn, type, &CnsComponent::__aexec, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                  val6, val7, val8, val9);
        if(breakoff || !pathContains(nsn)) return true;
        conntype_exec(comPtr, breakoff, member, type, test, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                      val6, val7, val8, val9);
//        QMetaObject::invokeMethod(getSystem(), member, type, QGenericReturnArgument(ret.name(), ret.data()), Q_ARG(CnsComponentPtr, comPtr), val1, val2, val3, val4, val5, val6, \
//                                             val7, val8, val9);
        return true;
    } else if(test.stateIndex >=0)
    {
        if(pathContains(nsn)){
            conntype_exec(comPtr, breakoff, member, type, test, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                          val6, val7, val8, val9);
        }
        return true;
    }
    return false;
}

bool CnsComponent::__dexec(EXECSTACK &stack, uint8_t &breakoff, CnsComponentPtr comPtr, const char* member, QByteArray *signature,\
         const QStringList &nsn, Qt::ConnectionType type, QGenericReturnArgument ret, \
         QGenericArgument val1, QGenericArgument val2, QGenericArgument val3, \
         QGenericArgument val4, QGenericArgument val5, QGenericArgument val6, QGenericArgument val7, \
         QGenericArgument val8, QGenericArgument val9)
{
    const MethodData& test = getMethodState(signature);
    if(breakoff) return true;
    if(test.stateIndex == HAS_BUT_ONLY_CHILD)
    {
        execChild(stack, breakoff, member, signature, nsn, type, &CnsComponent::__dexec, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                  val6, val7, val8, val9);
        return true;
    } else if(test.stateIndex <= HAS_ALL)
    {
//        QMetaObject::invokeMethod(getSystem(), member, type, QGenericReturnArgument(ret.name(), ret.data()), Q_ARG(CnsComponentPtr, comPtr), val1, val2, val3, val4, val5, val6, \
//                                         val7, val8, val9);
        if(pathContains(nsn)){
            conntype_exec(comPtr, breakoff, member, type, test, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                          val6, val7, val8, val9);
        }
        if(breakoff) return true;
        execChild(stack, breakoff, member, signature, nsn, type, &CnsComponent::__dexec, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                  val6, val7, val8, val9);
        return true;
    } else if(test.stateIndex >=0)
    {
        if(pathContains(nsn)){
            conntype_exec(comPtr, breakoff, member, type, test, QGenericReturnArgument(ret.name(), ret.data()), val1, val2, val3, val4, val5, \
                          val6, val7, val8, val9);
        }
        return true;
    }
    return false;
}

QVariant CnsComponent::property(const QString &propName, const QStringList &ns, const QString &name, CnsComponentPtr comPtr)
{
    EXECSTACK stack;
    stack.insert(_objFullPath);

    return __property(stack, ns, name, propName, comPtr);
}

bool has_ns(const QStringList &nss, const QString &ns)
{
    for(QStringList::const_iterator it = nss.begin(); it != nss.end(); it++)
    {
        if(*it == ns)
        {
            return true;
        }
    }
    return false;
}

//bool CnsComponent::setProperty_ts(const char *name, const QVariant &value)//thread-safe
//{
//    QWriteLocker lock(&_propertyLock);
//    return QObject::setProperty(name, value);
//}
//QVariant CnsComponent::property_ts(const char *propName)//thread-safe
//{
//    QReadLocker lock(&_propertyLock);
//    return QObject::property(propName);
//}

void CnsComponent::setMethodState(const QByteArray *member, const MethodData& state)
{
//    QWriteLocker lock(&_methodHashLock);
    _methodHash[member] = state;
}

static MethodData dummyMethodData;

const MethodData& CnsComponent::getMethodState(QByteArray *member)
{
    if(_methodHash.find(member) == _methodHash.end())
    {
        return dummyMethodData;
    }
//    QReadLocker lock(&_methodHashLock);
    return _methodHash[member];
}

bool CnsComponent::pathContains(const QStringList &ns, const QString &name)
{
    for(QStringList::const_iterator it = ns.begin(); it != ns.end(); it++)
    {
//        QString s = *it;
//        if(s.isEmpty())
//        {
//            continue;
//        } else{
//            if(s[0]=='/')
//            {
//                QStringList sl = s.split('/', QString::SkipEmptyParts);
//                for(QStringList::iterator jt = sl.begin(); jt != sl.end(); jt ++)
//                {
//                    if(!has_ns(getSorts(), *jt))
//                    {
//                        return false;
//                    }
//                }
//                return name.isEmpty() || getName() == name;
//            }
//        }
        if(has_ns(getSorts(), *it))
        {
            return name.isEmpty() || getName() == name;
        }
    }

    return ns.isEmpty() && (name.isEmpty() || getName() == name);
}

bool CnsComponent::pathContains(const QStringList &nsn)
{
    for(QStringList::const_iterator it = nsn.begin(); it != nsn.end(); it++)
    {
        const QStringList &sl = getComDef()->getNsN();
        for(QStringList::const_iterator jt = sl.begin(); jt != sl.end(); jt ++)
        {
            if(jt->contains(*it))
            {
                return true;
            }
        }
    }

    return nsn.isEmpty();
}

QVariant CnsComponent::__property(EXECSTACK &stack, const QStringList &ns, const QString &name, const QString &propName, CnsComponentPtr comPtr)
{
    QVariant v;
    if(pathContains(ns, name)){
        if(propName.isEmpty())
        {
            assert(this == comPtr.data());
            return QVariant::fromValue(comPtr);
        } else{
            v = property_ts(propName.toLatin1());
            if(v.isValid())
            {
                return v;
            }
        }
    }
    for(CHILDRENLIST::iterator it = _childrenList.begin(); it != _childrenList.end(); it ++ )
    {
        QSharedPointer<CnsComponent> &p = *it;
        if(testObjFullPath(stack, p->getObjFullPath())){
            if(propName.isEmpty())
            {
                if(p->pathContains(ns, name)){
                    assert(this == p.data());
                    return QVariant::fromValue(p);
                }
            }
            v = p->__property(stack, ns, name, propName, p);
            if(v.isValid())
            {
                return v;
            }
        }
    }
    return v;
}

//CnsComponentPtr CnsComponent::getComFromNs(CnsComponentPtr comPtr, const char *nameSpace) const
//{
//    if(_nameSpace == nameSpace)
//    {
//        return comPtr;
//    }
//    for(CHILDRENLIST::const_iterator it = _childrenList.begin(); it != _childrenList.end(); it ++ )
//    {
//        if((*it)->getNameSpace()==nameSpace)
//        {
//            return *it;
//        }
//    }
//    return nullptr;
//}

void CnsComponent::getAllProperty(const char *name, QVariantList &allProperty)
{
    EXECSTACK stack;
    stack.insert(_objFullPath);
    __getAllProperty(stack, name, allProperty);
}

void CnsComponent::__getAllProperty(EXECSTACK &stack, const char *name, QVariantList &allProperty)
{
    QVariant v = property_ts(name);
    if(v.isValid())
    {
        allProperty.push_back(v);
    }
    for(CHILDRENLIST::const_iterator it = _childrenList.begin(); it != _childrenList.end(); it ++ )
    {
        QSharedPointer<CnsComponent> p = *it;
        if(testObjFullPath(stack, p->getObjFullPath())){
            (*it)->__getAllProperty(stack, name, allProperty);
        }
    }
}

bool CnsComponent::setProperty(const QVariant &value, const QString &propName, const QString &name, const QStringList &ns)
{
    EXECSTACK stack;
    stack.insert(_objFullPath);
    return __setProperty(stack, value, propName, name, ns);
}

bool CnsComponent::__setProperty(EXECSTACK &stack, const QVariant &value, const QString &propName, const QString &name, const QStringList &ns)
{
    assert(!propName.isEmpty());
    if(pathContains(ns, name))
    {
        if(_userData.find(propName.toLatin1())!= _userData.end())
        {
            setProperty_ts(propName.toLatin1(), value);
            return true;
        } else{
        }
    }

    for(CHILDRENLIST::iterator it = _childrenList.begin(); it != _childrenList.end(); it ++ )
    {
        QSharedPointer<CnsComponent> &p = *it;
        if(testObjFullPath(stack, p->getObjFullPath())){
            if(p->__setProperty(stack, value, propName, name, ns))
            {
                return true;
            }
        }
    }

    return false;
}

//bool CnsComponent::setProperty(const QVariant &value, CnsComponentPtr comPtr, const char *name, const char *nameSpace, ...)
//{
//    if(nameSpace){
//        va_list vaList; //定义一个具有va_list型的变量，这个变量是指向参数的指针。
//        va_start(vaList, nameSpace);//第一个参数指向可变列表的地址,地址自动增加，第二个参数位固定值
////        qDebug() << name << "====" << nameSpace;
//        CnsComponentPtr com;
//        while (nameSpace != nullptr)
//        {
//            com = getComFromNs(comPtr, nameSpace);
//            if(com.isNull())
//            {
//                qDebug()<< QString("can't find namespace %1").arg(nameSpace);
//                return false;
//            }
//            nameSpace = va_arg(vaList, const char *);
//        }

//        va_end(vaList);//结束可变参数列表

//        com->QObject::setProperty(name, value);
//    } else{
//        comPtr->QObject::setProperty(name, value);
//    }
//    return true;
//}

QVariant CnsComponent::propertyInNs(const char *propName, CnsComponentPtr comPtr)
{
    EXECSTACK stack;
    stack.insert(_objFullPath);
    return __property(stack, getSorts(), "", propName, comPtr);
}

//QVariant CnsComponent::property(CnsComponentPtr comPtr, const char *name, const char *nameSpace, ...) const
//{
//    va_list vaList; //定义一个具有va_list型的变量，这个变量是指向参数的指针。
//    va_start(vaList, nameSpace);//第一个参数指向可变列表的地址,地址自动增加，第二个参数位固定值
////    qDebug() << name << "====" << nameSpace;
//    CnsComponentPtr com;
//    while (nameSpace != nullptr)
//    {
//        com = getComFromNs(comPtr, nameSpace);
//        if(com.isNull())
//        {
//            qDebug()<< QString("can't find namespace %1").arg(nameSpace);
//            return false;
//        }
//        nameSpace = va_arg(vaList, const char *);
//    }
//    va_end(vaList);//结束可变参数列表
//    EXECSTACK stack;
//    stack.push_back(_objFullPath);
//    return com->__property(stack,name);
//}

CnsComponentPtr CnsComponent::__findChildren(EXECSTACK &stack, const QString &propName)
{
    if(testObjFullPath(stack, _objFullPath))
    {
        CHILDRENHASH::iterator it = _childrenHash.find(propName);
        if(it == _childrenHash.end())
        {
            for(CHILDRENLIST::iterator it = _childrenList.begin(); it != _childrenList.end(); it ++)
            {
                CnsComponentPtr p = (*it)->__findChildren(stack, propName);
                if(p.data()) return p;
            }
            return nullptr;
        }
        return *it;
    }
    return nullptr;
}

bool CnsComponent::addRef(const QString &key, CnsComponentPtr ref)
{
    CHILDRENHASH::iterator it = _childrenHash.find(key);
    if(it == _childrenHash.end())
    {
        _childrenList.push_back(ref);
        _childrenHash[key] = ref;
        return true;
    } else{
        _childrenList.removeAll(_childrenHash[key]);
        _childrenList.push_back(ref);
        _childrenHash[key] = ref;
    }
    return false;

}

QByteArray *CnsComponent::getMethodSignature(const QByteArray &methodSignature)
{
    QHash<QByteArray, QByteArray *>::iterator it = _methodSignature.find(methodSignature);
    return it == _methodSignature.end()? nullptr:it.value();
}
QByteArray *CnsComponent::registerMethodSignature(const QByteArray &methodSignature)
{
    QHash<QByteArray, QByteArray *>::iterator it = _methodSignature.find(methodSignature);
    if(it == _methodSignature.end()){
        QByteArray* p = new QByteArray(methodSignature);
        _methodSignature[methodSignature] = p;
        return p;
    }
    return it.value();
}
