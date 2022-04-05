/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/

#pragma once
#include <QtCore>
#include <QSharedDataPointer>
#include <QVariantMap>

class QSPatchPriv;

class QSPatch
{
public:
    enum Type {
        Null,
        Insert,
        Remove,
        Update,
        Move
    };

    QSPatch();
    QSPatch(Type type,int from = 0, int to = 0, int count = 0);
    QSPatch(Type type,int from, int to, int count, const QVariantMap& data);
    QSPatch(Type type,int from, int to, int count, const QVariantList& data);

    QSPatch(const QSPatch &);
    QSPatch &operator=(const QSPatch &);
    ~QSPatch();

    QSPatch::Type type() const;
    void setType(const QSPatch::Type &type);

    QVariantList data() const;

    void setData(const QVariantList &data);
    void setData(const QVariantMap& data);

    bool operator==(const QSPatch& rhs) const;

    int from() const;
    void setFrom(int from);

    int to() const;
    void setTo(int to);

    bool isNull() const;

    int count() const;
    void setCount(int count);

    bool canMerge(const QSPatch& other) const;

    QSPatch merged(const QSPatch &other) const;

    QSPatch& merge(const QSPatch &other);

    static QSPatch createUpdate(int index, const QVariantMap& diff);
    static QSPatch createRemove(int from,int to);

signals:

public slots:

private:
    QSharedDataPointer<QSPatchPriv> d;
};

QDebug operator<<(QDebug dbg, const QSPatch& change);

typedef QList<QSPatch> QSPatchSet;

Q_DECLARE_METATYPE(QSPatch)
