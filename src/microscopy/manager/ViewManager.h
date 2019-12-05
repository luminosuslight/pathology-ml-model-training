#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QTimer>
#include <QScopedPointer>
#include <QQuickItem>

class CoreController;
class TissueViewBlock;

class ViewManager : public QObject {

    Q_OBJECT

    Q_PROPERTY(QList<QObject*> views READ viewsQml NOTIFY viewsChanged)

public:
    explicit ViewManager(CoreController* controller);

signals:
    void viewsChanged();
    void imageAssignmentChanged();
    void visualizeAssignmentChanged();
    void areaAssignmentChanged();

public slots:
    const QList<TissueViewBlock*>& views() const;
    QList<QObject*> viewsQml() const;

protected:
    void updateViews();

protected:
    CoreController* const m_controller;

    QList<TissueViewBlock*> m_views;
};

#endif // VIEWMANAGER_H
