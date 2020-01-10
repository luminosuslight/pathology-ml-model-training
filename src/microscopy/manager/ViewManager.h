#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QTimer>
#include <QScopedPointer>
#include <QQuickItem>

class CoreController;
class DataViewBlock;

class ViewManager : public QObject {

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
    const QList<DataViewBlock*>& views() const;
    QList<QObject*> viewsQml() const;
    QList<QObject*> visibleViewsQml() const;

    QStringList availableFeatures() const;

protected:
    void updateViews();

protected:
    CoreController* const m_controller;

    QList<DataViewBlock*> m_views;
};

#endif // VIEWMANAGER_H
