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

    connect(this, &ViewManager::viewsChanged,
            this, &ViewManager::visibleViewsChanged);
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

QList<QObject*> ViewManager::visibleViewsQml() const {
    QList<QObject*> views;
    for (auto view: m_views) {
        if (qobject_cast<BoolAttribute*>(view->attr("visible"))->getValue()) {
            views.append(view);
        }
    }
    return views;
}

QStringList ViewManager::availableFeatures() const {
    QStringList features;
    auto dbs = m_controller->blockManager()->getBlocksByType<CellDatabaseBlock>();
    for (auto db: dbs) {
        for (auto feature: db->features()) {
            if (!features.contains(feature)) {
                features.append(feature);
            }
        }
    }
    return features;
}

void ViewManager::updateViews() {
    QList<DataViewBlock*> views = m_controller->blockManager()->getBlocksByType<DataViewBlock>();
    for (auto view: views) {
        if (!m_views.contains(view)) {
            connect(qobject_cast<BoolAttribute*>(view->attr("visible")), &BoolAttribute::valueChanged,
                    this, &ViewManager::visibleViewsChanged);
        }
    }
    if (views != m_views) {
        m_views = views;
        emit viewsChanged();
    }
}
