#ifndef CNSCREATECOMDEFCONTEXT_H
#define CNSCREATECOMDEFCONTEXT_H

#include "cnsGlobal.h"
#include "cnsComDefinition.h"
#include <QSet>

struct NAME2NS_NS2NAME
{
    QHash<QString, QString> _name2Ns;
    QSet<QString> _nsName;
};

typedef QHash<QString, QString> QHASH_DEFNAME2NS;

class CnsFilenameModule
{
public:
    //filename to dirFullPath
    QHash<QString, QString> _fn2DirFullPath;
    //defName to namespace
    QHash<QString, NAME2NS_NS2NAME> _defName2Ns;

    bool hasDirFullPath(const QString &fn)
    {
        return _fn2DirFullPath.find(fn) != _fn2DirFullPath.end();
    }
    void setDirFullPath(const QString &fn, const QString &dirFullPath)
    {
        _fn2DirFullPath[fn] = dirFullPath;
    }
    void setDefName2Ns(const QString &defName, const QString &ns, const QString &path)
    {
        NAME2NS_NS2NAME &d2ns =_defName2Ns[path];
        d2ns._name2Ns[defName] = ns;
        d2ns._nsName.insert(ns+'/'+defName);
    }

    inline const NAME2NS_NS2NAME &getName2Ns_Ns2Name(const QString &path)
    {
        return _defName2Ns[path];
    }
    inline const QHash<QString, QString> &getDefName2Ns(const QString &path)
    {
        return _defName2Ns[path]._name2Ns;
    }
};

class CnsDefinitionCreator;
class CnsCreateComDefContext;

class PathDefState
{
private:
    EXECSTACK _stacks;
    QSet<PathDefState*> _deferredMergePathDefState;
    QSet<const QString*> _waitingFullPath;
    CnsComDefinitionPtr _def;
    bool _createFinished=false;
public:
//    EXECSTACK &getStacks()
//    {
//        return _stacks;
//    }
    inline CnsComDefinitionPtr& getDef()
    {
        return _def;
    }
    inline void setDef(CnsComDefinitionPtr def)
    {
        _def = def;
    }
    inline QSet<const QString*>& getWaitingFullPath()
    {
        return _waitingFullPath;
    }
    inline void addDeferredMergePathDefState(PathDefState *p)
    {
        if(p != this)
        _deferredMergePathDefState.insert(p);
    }
    bool addWaitingFullPath(const QString *fullPath);
    inline bool isWaitingFullPath(const QString *fullPath)
    {
        return _waitingFullPath.find(fullPath) != _waitingFullPath.end();
    }
    inline bool isWaitingFullPathEmpty()
    {
        return _waitingFullPath.isEmpty();
    }
    inline bool eraseWaitingFullPath(const QString *fullPath)
    {
        return _waitingFullPath.remove(fullPath);
    }
    bool checkAndInsertStacks(const QString *fullPath);
    inline bool isFinished()
    {
        return _createFinished && _waitingFullPath.isEmpty();
    }
    bool testStacks(const QString *fullPath);
    void handleMergeParent(const QString *parentFullPath, CnsCreateComDefContext &context);
    void handleMergeAfterCreateDef();
    void handleDeferredMergeMethods();
    void mergeWaitingFullPath(PathDefState &pds, CnsCreateComDefContext &context);
};

class CnsCreateComDefContext
{
private:
    QHash<const QString *, PathDefState> _pathDefStates;
    CnsDefinitionCreator *_creator;
    CnsDefaultComFactory *_factory;
public:
    CnsCreateComDefContext(CnsDefinitionCreator *creator, CnsDefaultComFactory *factory);

    inline CnsDefinitionCreator *getDefinitionCreator()
    {
        return _creator;
    }
    inline CnsDefaultComFactory *getDefaultComFactory()
    {
        return _factory;
    }
    inline PathDefState &getPathDefStates(const QString * fullPath)
    {
        return _pathDefStates[fullPath];
    }
};

#endif // CNSCREATECOMDEFCONTEXT_H
