#include "cnscreatecomdefcontext.h"
#include "log/cnslogger.h"

CnsCreateComDefContext::CnsCreateComDefContext(CnsDefinitionCreator *creator, CnsDefaultComFactory *factory)
    :_creator(creator), _factory(factory)
{

}

bool PathDefState::checkAndInsertStacks(const QString *fullPath)
{
    if(_stacks.find(fullPath) == _stacks.end())
    {
        return false;
    }
    _stacks.insert(fullPath);
    return true;
}

void PathDefState::mergeWaitingFullPath(PathDefState &pds, CnsCreateComDefContext &context)
{
    QSet<const QString*>& waitingFullPath = pds.getWaitingFullPath();
    for(QSet<const QString*>::iterator it = waitingFullPath.begin(); it != waitingFullPath.end(); it++)
    {
        if(addWaitingFullPath(*it))
        {
            PathDefState & pathDefState = context.getPathDefStates(*it);
            pathDefState.addDeferredMergePathDefState(this);
        }
    }
    QSet<const QString*>& myWaitingFullPath = getWaitingFullPath();
    for(QSet<const QString*>::iterator it = myWaitingFullPath.begin(); it != myWaitingFullPath.end(); it++)
    {
        if(pds.addWaitingFullPath(*it))
        {
            PathDefState & pathDefState = context.getPathDefStates(*it);
            pathDefState.addDeferredMergePathDefState(&pds);
        }
    }
}

void PathDefState::handleMergeAfterCreateDef()
{
    _createFinished = true;
    if(isFinished())
    {
        handleDeferredMergeMethods();
    }
}

void PathDefState::handleMergeParent(const QString *parentFullPath, CnsCreateComDefContext &context)
{
    PathDefState &parentPathDefState = context.getPathDefStates(parentFullPath);
//    if(isWaitingFullPath(mergeFullPath))
//    {
//        _def->mergeMethodDataFromDef(mergePathDefState.getDef().data());
//        eraseWaitingFullPath(mergeFullPath);
//    }

//    if(mergePathDefState.isWaitingFullPath(_def->getDefFullPath())){
//        mergePathDefState.getDef()->mergeMethodDataFromDef(_def.data());
//        mergePathDefState.eraseWaitingFullPath(_def->getDefFullPath());
//    }
    parentPathDefState.getDef()->mergeMethodDataFromDef(getDef().data());
    eraseWaitingFullPath(parentPathDefState.getDef()->getDefFullPath());

    if(isFinished())
    {
        handleDeferredMergeMethods();
    }else {
        if(!parentPathDefState.isFinished())
        {
            mergeWaitingFullPath(parentPathDefState, context);
            if(parentPathDefState.addWaitingFullPath(getDef()->getDefFullPath()))
            {
                addDeferredMergePathDefState(&parentPathDefState);
            }
        }
    }
    if(parentPathDefState.isFinished())
    {
        parentPathDefState.handleDeferredMergeMethods();
    }

}

void PathDefState::handleDeferredMergeMethods()
{
    QSet<PathDefState*> deferRun;
    for(QSet<PathDefState*>::iterator it = _deferredMergePathDefState.begin(); it != _deferredMergePathDefState.end();)
    {
        PathDefState* ps = *it;
        ps->getDef()->mergeMethodDataFromDef(getDef().data());
        ps->eraseWaitingFullPath(getDef()->getDefFullPath());
//        CNSLOG_INFO(QString("handleDeferredMergeMethods=%1====%2").arg(*ps->getDef()->getDefFullPath()).arg(*getDef()->getDefFullPath()));
        if(ps->isFinished())
        {
            deferRun.insert(*it);
            it = _deferredMergePathDefState.erase(it);
        } else it ++;
    }
    for(QSet<PathDefState*>::iterator it = deferRun.begin(); it != deferRun.end(); it++)
    {
        (*it)->handleDeferredMergeMethods();
    }
}

bool PathDefState::addWaitingFullPath(const QString *fullPath)
{
    if(fullPath != _def->getDefFullPath() && _waitingFullPath.find(fullPath) == _waitingFullPath.end())
    {
        _waitingFullPath.insert(fullPath);
        return true;
    }
    return false;

}

bool PathDefState::testStacks(const QString *fullPath)
{
    if(_stacks.find(fullPath) == _stacks.end())
    {
        _stacks.insert(fullPath);
        return false;
    }
    return true;
}

