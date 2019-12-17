#include "ViewManager.h"

#include "core/CoreController.h"
#include "core/manager/BlockManager.h"
#include "microscopy/blocks/basic/DataViewBlock.h"


ViewManager::ViewManager(CoreController* controller)
    : QObject(controller)
    , m_controller(controller)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    qmlRegisterType<DataViewBlock>();

    connect(m_controller->blockManager(), &BlockManager::blockInstanceCountChanged,
            this, &ViewManager::updateViews);
    updateViews();
}

const QList<DataViewBlock*>& ViewManager::views() const {
    return m_views;
}

QList<QObject*> ViewManager::viewsQml() const {
    QList<QObject*> views;
    for (auto view: m_views) {
        views.append(view);
    }
    return views;
}

void ViewManager::updateViews() {
    QList<DataViewBlock*> views = m_controller->blockManager()->getBlocksByType<DataViewBlock>();
    if (views != m_views) {
        m_views = views;
        emit viewsChanged();
    }
}
