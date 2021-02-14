#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "cnsDefaultComFactory.h"

class Framework
{
protected:
    void readComDefinition(const QString &path);
public:
    Framework();
    ~Framework();

    void registerSystem(const QString &name, QObject* system);
    void autoCreateComponent(const QString &path);
    CnsComponentPtr getComponent(const QString& objName, const QString& defPath="");
    CnsComponentPtr newComponent(const QString& objName, const QString& defPath="");

private:

    CnsDefaultComFactoryPtr _comFactory;
};

#endif // FRAMEWORK_H
                         
