#ifndef CLASSESLISTBLOCK_H
#define CLASSESLISTBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class ClassesListBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Classes List";
        info.nameInUi = "List of Classes";
        //info.category << "Basic";
        info.helpText = "Shows all classes of a specific feature and allows to name them.";
        info.qmlFile = "qrc:/microscopy/blocks/basic/ClassesListBlock.qml";
        info.orderHint = 1000 + 6;
        info.complete<ClassesListBlock>();
        return info;
    }

    explicit ClassesListBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QStringList availableFeatures() const;

    void updateClasses();

    void setClassName(int classId, QString name);

    void autoLabel();

protected:
    StringAttribute m_feature;

    // runtime:
    VariantListAttribute m_availableClasses;

};

#endif // CLASSESLISTBLOCK_H
