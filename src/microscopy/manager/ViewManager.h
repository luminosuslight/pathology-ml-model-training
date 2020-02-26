#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "core/helpers/ObjectWithAttributes.h"
#include "core/helpers/SmartAttribute.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QTimer>
#include <QScopedPointer>
#include <QQuickItem>

class CoreController;
class DataViewBlock;

class ViewManager : public QObject, public ObjectWithAttributes {

    Q_OBJECT

    Q_PROPERTY(QList<QObject*> views READ viewsQml NOTIFY viewsChanged)
    Q_PROPERTY(QList<QObject*> visibleViews READ visibleViewsQml NOTIFY visibleViewsChanged)

public:
    explicit ViewManager(CoreController* controller);

signals:
    void viewsChanged();
    void visibleViewsChanged();
    void imageAssignmentChanged();
    void visualizeAssignmentChanged();
    void areaAssignmentChanged();

public slots:
    QObject* attr(QString name);

    const QList<DataViewBlock*>& views() const;
    QList<QObject*> viewsQml() const;
    QList<QObject*> visibleViewsQml() const;

    QStringList availableFeatures() const;

protected:
    void updateViews();

protected:
    CoreController* const m_controller;

    QList<DataViewBlock*> m_views;

    IntegerAttribute m_dataViewHeight;
};

#endif // VIEWMANAGER_H
