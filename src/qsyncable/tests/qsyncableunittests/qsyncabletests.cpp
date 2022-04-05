#include <QVariantList>
#include <QTest>
#include <QSDiffRunner>
#include <QSListModel>
#include "qsyncabletests.h"
#include "priv/qstree.h"
#include "math.h"

static QStringList convert(const QVariantList& list) {
    QStringList res;

    for (int i = 0 ; i < list.size() ; i++) {
        res << list.at(i).toMap()["id"].toString();
    }

    return res;
}

static QVariantList convert(const QStringList& list) {
    QVariantList res;

    for (int i = 0 ; i < list.size() ; i++) {
        QVariantMap item;
        item["id"] = list.at(i);
        res << item;
    }

    return res;

}

void run() {
    QFETCH(QString, from);
    QFETCH(QString, to);

    QVariantList fList = convert(from.split(","));
    QVariantList tList = convert(to.split(","));


    QSListModel listModel;

    listModel.setStorage(fList);

    QSDiffRunner runner;
    runner.setKeyField("id");

    QList<QSPatch> patches = runner.compare(fList, tList);
    runner.patch(&listModel, patches);

    if (tList != listModel.storage()) {
        qDebug() << "from" << from;
        qDebug() << "to" << to;
        qDebug() << "actual" << convert(listModel.storage()).join(",");
        qDebug() << patches;
    }

    QVERIFY(tList == listModel.storage());
    qDebug() << patches;

}

QSyncableTests::QSyncableTests(QObject *parent) : QObject(parent)
{

     qsrand( QDateTime::currentDateTime().toTime_t());
}

void QSyncableTests::patch()
{
    QSPatch c1,c2;

    c1.setType(QSPatch::Insert);
    c2.setType(QSPatch::Move);

    QCOMPARE(c1 == c2, false);

    c1.setType(QSPatch::Move);

    QCOMPARE(c1 == c2, true);

    QVariantMap d1;
    d1["key"] = "1";

    c1.setData(d1);

    QCOMPARE(c1 == c2, false);
}

void QSyncableTests::patch_merge()
{
    QSPatch c1,c2,c3,c4;
    QVariantMap a,b,c;
    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";

    QVariantList data;

    c1 = QSPatch(QSPatch::Move);
    c2 = QSPatch(QSPatch::Insert);

    QVERIFY(!c1.canMerge(c2));

    c1 = QSPatch(QSPatch::Remove,0,0);
    c2 = QSPatch(QSPatch::Remove,1,1);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(c2.canMerge(c1));

    c3 = c1.merged(c2);
    c4 = c2.merged(c1);

    QCOMPARE(c3.type(), QSPatch::Remove);
    QCOMPARE(c3.from(), 0);
    QCOMPARE(c3.to(), 1);

    QVERIFY(c3 == c4);

    c2 = QSPatch(QSPatch::Remove,2,2);

    QVERIFY(!c1.canMerge(c2));
    QVERIFY(!c2.canMerge(c1));

    /* Merge move */
    c1 = QSPatch(QSPatch::Move, 1, 0, 1);
    c2 = QSPatch(QSPatch::Move, 2, 1, 1);
    QVERIFY(c1.canMerge(c2));
    QVERIFY(!c2.canMerge(c1));

    c3 = c1.merged(c2);
    c4 = c2.merged(c1);

    QCOMPARE(c3.type(), QSPatch::Move);
    QCOMPARE(c3.from(), 1);
    QCOMPARE(c3.to(), 0);
    QCOMPARE(c3.count(), 2);

    QCOMPARE(c4.type(), QSPatch::Null);

    /* Merge seq of insert */
    data.clear();
    data << a << b;

    c1 = QSPatch(QSPatch::Insert, 0, 0, 1, a);
    c2 = QSPatch(QSPatch::Insert, 1, 1, 1, b);
    c3 = QSPatch(QSPatch::Insert, 2, 2, 1, c);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(!c2.canMerge(c1));

    c4 = c1.merged(c2);

    QCOMPARE(c4.type(), QSPatch::Insert);
    QCOMPARE(c4.from(), 0);
    QCOMPARE(c4.to(), 1);
    QCOMPARE(c4.count(), 2);
    QVERIFY(c4.data() == data);

    QVERIFY(c4.canMerge(c3));
    c4 = c4.merged(c3);


    QCOMPARE(c4.type(), QSPatch::Insert);
    QCOMPARE(c4.from(), 0);
    QCOMPARE(c4.to(), 2);
    QCOMPARE(c4.count(), 3);

    /* Merge insert at same position */

    c1 = QSPatch(QSPatch::Insert, 0, 0, 1, a);
    c2 = QSPatch(QSPatch::Insert, 0, 0, 1, b);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(c2.canMerge(c1));

    c3 = c1.merged(c2);

    QCOMPARE(c3.type(), QSPatch::Insert);
    QCOMPARE(c3.from(), 0);
    QCOMPARE(c3.to(), 1);
    QCOMPARE(c3.count(), 2);

}

void QSyncableTests::tree()
{
    QSTree tree;
    QVERIFY(tree.min() == 0);
    QVERIFY(tree.max() == 0);
    QVERIFY(tree.height() == 0);
    QVERIFY(tree.sum() == 0);
    QVERIFY(tree.root() == 0);

    // 8(10)
    QSTreeNode* node = tree.insert(8,10);

    QVERIFY(tree.root() == node);
    QCOMPARE(tree.min() , 8);
    QVERIFY(tree.max() == 8);
    QCOMPARE(tree.sum() , 10);
    QVERIFY(tree.height() == 1);

    // 8(10)
    //     9(12)

    node = tree.insert(9,12);

    QVERIFY(node->height() == 1);
    QCOMPARE(tree.sum(), 22);
    QVERIFY(tree.height() == 2);

    //     8(10)
    // 7(5)     9(12)

    tree.insert(7,5);
    QVERIFY(tree.sum() == 27);

    //      8(10)
    //  7(5)     9(12)
    //6(5)

    tree.insert(6,5);
    QVERIFY(tree.sum() == 32);
    QVERIFY(tree.height() == 3);

    //      8(10)
    //  7(5)     9(12)
    //6(5)         10(6)

    tree.insert(10,6);
    QVERIFY(tree.sum() == 38);
    QVERIFY(tree.height() == 3);

    QCOMPARE(tree.countLessThan(10), 32);
    QCOMPARE(tree.countLessThan(9), 20);
    QCOMPARE(tree.countLessThan(8), 10);
    QCOMPARE(tree.countLessThan(7), 5);
    QCOMPARE(tree.countLessThan(6), 0);

    tree.remove(7);
    QCOMPARE(tree.sum(), 33); // Only 7 are removed.
    QVERIFY(tree.height() == 3);

    //     8(10
    // 6(5)    9(12)

    tree.remove(10);
    QCOMPARE(tree.sum(), 27);
    QVERIFY(tree.height() == 2);

    tree.remove(8);
    QCOMPARE(tree.sum(), 17);
    QVERIFY(tree.height() == 2);

}

void QSyncableTests::tree_insert()
{
    QSTree tree;

    tree.insert(100);
    tree.insert(99);
    QCOMPARE(tree.height(), 2);

    tree.insert(98);
    QCOMPARE(tree.height(), 2);
    QCOMPARE(tree.root()->key(), 99);

    tree.insert(97);
    tree.insert(96);
    QCOMPARE(tree.height(), 3);

    tree.insert(95);
    QCOMPARE(tree.height(), 3);
    QCOMPARE(tree.root()->key(), 97);
}

void QSyncableTests::tree_updateMin()
{
    QSTree tree;

    tree.insert(8,1);
    tree.insert(6,1);
    tree.insert(7,1);
    tree.insert(5,1);
    tree.insert(10,1);

    QList<int> list;

    list << 5 << 6 << 7 << 8 << 10;

    for (int i = 0 ; i < list.size() - 1 ; i++) {
        int min = list.at(i);
        QCOMPARE(tree.min() , min);
        tree.remove(min);
    }

}

void QSyncableTests::tree_balance()
{
    QList<int> src;
    QList<int> insert;
    QList<int> remove;

    for (int i = 0 ; i < 100;i++) {
        src << i;
    }

    insert = src;
    QSTree tree;

    while (insert.size() > 0) {
        int i = qrand() % insert.size();
        int v = insert.takeAt(i);
        int inserted = src.size() - insert.size();

        tree.insert(v);

        QCOMPARE(tree.sum() , inserted);

        int height = qFloor(log2(inserted));

        bool valid = QSTree::validate(tree.root());

        if (!valid) {
            qDebug() << tree;
            qDebug() << tree.height() << height;
        }

        QVERIFY(valid);
    }

    remove = src;
    while (remove.size() > 0) {
        int i = qrand() % remove.size();
        int v = remove.takeAt(i);

        tree.remove(v);
        QCOMPARE(tree.sum(), remove.size());

        int height = qFloor(log2(remove.size()));

        if (tree.isNull()) {
            break;
        }

        bool valid = QSTree::validate(tree.root());

        if (!valid) {
            qDebug() << tree;
            qDebug() << tree.height() << height;
        }

        QVERIFY(valid);
    }


}

void QSyncableTests::tree_remove()
{
    QSTree tree;
    tree.insert(3,1);
    tree.insert(6,1);

    QCOMPARE(tree.min(), 3);
    tree.remove(3);

    QCOMPARE(tree.min(), 6);
    tree.remove(6);
}

void QSyncableTests::diffRunner()
{
    QFETCH(QVariantList, previous);
    QFETCH(QVariantList, current);
    QFETCH(QString, keyField);
    QFETCH(QList<QSPatch>, changes);

    QList<QSPatch> expectedChanges = changes;

    QSDiffRunner runner;
    runner.setKeyField(keyField);

    QList<QSPatch> result = runner.compare(previous, current);

    if (changes.size() != result.size()) {
        qDebug() << expectedChanges;
        qDebug() << result;
    }

    QCOMPARE(expectedChanges.size(), result.size());

    for (int i = 0; i < changes.size(); i++) {
        QSPatch expected = changes.at(i);
        QSPatch real = result.at(i);
        if (!(expected == real))  {
            qDebug() << expectedChanges;
            qDebug() << result;
        }
        QVERIFY(expected == real);
    }

    QSListModel* model = new QSListModel();

    model->setStorage(previous);
    QVERIFY(runner.patch(model, result));
    QVariantList modelList = model->storage();;

    if (modelList != current) {
        qDebug() << modelList;
        qDebug() << current;
    }
    QVERIFY(modelList == current);

    delete model;
}

void QSyncableTests::diffRunner_data()
{
    QTest::addColumn<QVariantList>("previous");
    QTest::addColumn<QVariantList>("current");
    QTest::addColumn<QString>("keyField");
    QTest::addColumn<QList<QSPatch> >("changes");

    QVariantList previous;
    QVariantList current;
    QList<QSPatch> changes;
    QSPatch c1;

    QVariantMap a,b,c,d,e,f;
    QVariantMap tmp;

    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";
    e["id"] = "e";
    f["id"] = "f";

    previous << a << b << c;
    current << a << b << c;

    QTest::newRow("No Changes") << previous << current << "id" << changes;

    /* Remove first element */
    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 0, 1);
    QTest::newRow("Remove first element") << previous << current << "id" << changes;

    /* Remove all element */
    previous.clear();current.clear();changes.clear();
    c1 = QSPatch();
    previous << a << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 2, 3);
    QTest::newRow("Remove all element") << previous << current << "id" << changes;

    /* Remove two elements from different position*/
    previous.clear();current.clear();changes.clear();
    previous << a << b << c << d << e;
    current << a << c << e;
    changes << QSPatch(QSPatch::Remove, 1, 1, 1)
            << QSPatch(QSPatch::Remove, 2, 2, 1);
    QTest::newRow("Remove two elements from different position") << previous << current << "id" << changes;

    /* Add 3 elements to empty list*/
    previous.clear();current.clear();changes.clear();

    current << a << b << c;
    changes << QSPatch(QSPatch::Insert,0,2,3, QVariantList() << a << b << c);
    QTest::newRow("Add 3 elements to empty list") << previous << current << "id" << changes;

    /* Add new element to end */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << b << c << d;
    changes << QSPatch(QSPatch::Insert, 3, 3, 1, d);
    QTest::newRow("Add new element to end") << previous << current << "id" << changes;

    /* Add 2 elements to middle */

    previous.clear();current.clear();changes.clear();
    previous << a << b;
    current << a << c << d << b;
    changes << QSPatch(QSPatch::Insert, 1, 2, 2, QVariantList() << c << d);
    QTest::newRow("Add 2 elements to middle") << previous << current << "id" << changes;

    /* Add 2 elements to differnt block */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << d << b << e << c;
    changes << QSPatch(QSPatch::Insert, 1, 1, 1, d)
            << QSPatch(QSPatch::Insert, 3, 3, 1, e);
    QTest::newRow("Add 2 elements to differnt block") << previous << current << "id" << changes;

    /* Move from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move from last to first") << previous << current << "id" << changes;

    /* Move 2 from first to last */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move 2 from first to last") << previous << current << "id" << changes;


    /* Move 2 from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << b << c << a;
    changes.clear();
    changes << QSPatch(QSPatch::Move,1,0,2);

    QTest::newRow("Move 2 from last to first") << previous << current << "id" << changes;

    /* Move 2 from last to middle */
    previous.clear();
    previous << a << b << c << d;
    current.clear();
    current << a << c << d << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,1,2);

    QTest::newRow("Move 2 from last to middle") << previous << current << "id" << changes;


    /* Update 2 elements*/
    previous.clear();
    previous << a << b << c;
    a["value"] = "a";
    b["value"] = "b";
    current.clear();
    current << a << b << c;

    tmp.clear();
    tmp["value"] = "a";

    changes.clear();
    changes << QSPatch(QSPatch::Update,0,0,1,tmp);

    tmp["value"] = "b";
    changes  << QSPatch(QSPatch::Update,1,1,1,tmp);

    QTest::newRow("Update 2 elements") << previous << current << "id" << changes;

    /* Remove, Insert, Move */
    previous.clear();
    current.clear();
    changes.clear();

    previous << a << b << c << d << e;
    current << a << f << d << e << c;
    changes << QSPatch(QSPatch::Remove,1,1,1)
            << QSPatch(QSPatch::Insert,1,1,1,f)
            << QSPatch(QSPatch::Move,3,2,2);
    QTest::newRow("Remove, Insert, Move") << previous << current << "id" << changes;

}

void QSyncableTests::diffRunner_move()
{
    run();

}

void QSyncableTests::diffRunner_move_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("to");

    QTest::newRow("S1") << "1,2,3,4,5,6,7"
                        << "4,1,7,2,3,5,6";

    QTest::newRow("S2") << "1,2,3,4,5,6,7"
                        << "4,7,1,2,3,5,6";

    QTest::newRow("S3") << "1,2,3,4,5,6,7"
                        << "3,6,1,7,2,4,5";

    QTest::newRow("S4") << "1,2,3,4,5,6,7"
                        << "7,1,5,2,3,4,6";


    QTest::newRow("S6") << "1,2,3,4,5,6,7"
                        << "7,2,1,5,3,4,6";

    QTest::newRow("S7") << "1,2,3,4,5,6,7"
                        << "7,6,5,4,3,2,1";


    QTest::newRow("S9") << "1,2,3,4,5,6,7"
                        << "7,3,5,1,2,4,6";

    QTest::newRow("S10") << "1,2,3,4,5,6,7,8,9"
                        << "7,3,5,1,8,2,4,6,9";

}

void QSyncableTests::diffRunner_failedCase()
{
    run();
}

void QSyncableTests::diffRunner_failedCase_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("to");

    QTest::newRow("1") << "1,2,3,4,5,6,7"
                       << "3,1,2,4,5,6,7";

    QTest::newRow("2") << "0,1,2,3,4,5,6,7,8,9"
                       << "1,11,2,3,12,4,5,6,10,7,8,0,9";

    QTest::newRow("3") << "0,1,2,3,4,5,6,7,8,9"
                       << "1,3,7,2,10,8,5,9";

    QTest::newRow("4") << "0,1,2,3,4,5,6,7,8,9"
                       << "1,12,6,4,10,5,11,8,9";

    QTest::newRow("5") << "0,1,2,3,4,5,6,7,8,9"
                       << "0,3,8,11,7,9,5,10,1";
    QTest::newRow("6") << "0,1,2,3,4,5,6,7,8,9"
                       << "1,4,7,10,8";

}

void QSyncableTests::diffRunner_noKeyField()
{
    QFETCH(QVariantList, from);
    QFETCH(QVariantList, to);

    QSDiffRunner runner;
    QList<QSPatch> patches = runner.compare(from, to);


    QSListModel listModel;
    listModel.setStorage(from);
    runner.patch(&listModel, patches);

    QVERIFY(listModel.storage() == to);
}

void QSyncableTests::diffRunner_noKeyField_data()
{
    QTest::addColumn<QVariantList>("from");
    QTest::addColumn<QVariantList>("to");

    QVariantMap a,b,c,d,e;

    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";
    e["id"] = "e";

    QTest::newRow("Shifted")
           << (QVariantList() << a << b << c << d)
           << (QVariantList() << b << c << d << a);

    QTest::newRow("Remove")
           << (QVariantList() << a << b << c << d)
           << (QVariantList() << a << b << c);

    QTest::newRow("Add")
           << (QVariantList() << a << b << c << d)
           << (QVariantList() << a << b << c << d << e);
}

void QSyncableTests::diffRunner_invalidKey()
{
    QVariantMap a,b,c,d,e;

    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";
    e["id"] = "e";

    QVariantList from,to;
    from << a << b << c << d;
    to << d << b << c << a;

    QSDiffRunner runner;
    runner.setKeyField("uuid"); // Set a wrong key field
    QList<QSPatch> patches = runner.compare(from, to);
    QCOMPARE(patches.size() , 2);

    QSListModel listModel;
    listModel.setStorage(from);
    runner.patch(&listModel, patches);

    QVERIFY(listModel.storage() == to);
}

void QSyncableTests::diffRunner_random()
{
    QVariantList from;
    int count = 10;
    for (int i = 0 ; i < count;i++) {
        QVariantMap item;
        item["id"] = i;
        item["value"] = i;
        from << item;
    }

    QVariantList to = from;
    int nextId = count;
    QVariantMap item;

    for (int i = 0 ; i < 10 ;i++) {
        int type = qrand()  % 4;
        int f = qrand() % to.size();
        int t = qrand() % to.size();
        switch (type) {
        case 0:
            item = to[t].toMap();
            item["value"] = item["value"].toInt() + 1;
            to[t] = item;
            break;
        case 1:
            to.removeAt(f);
            break;
        case 2:
            item = QVariantMap();
            item["id"] = nextId++;
            item["value"] = nextId;
            to.insert(f,item);
        case 3:
            to.move(f,t);
            break;
        }
    }

    qDebug() << "from" << convert(from).join(",");
    qDebug() << "to" << convert(to).join(",");

    QSListModel listModel;

    listModel.setStorage(from);

    QSDiffRunner runner;
    runner.setKeyField("id");

    QList<QSPatch> patches = runner.compare(from, to);
    runner.patch(&listModel, patches);

    QList<QSPatch> patches2 = runner.compare(to, listModel.storage());

    if (patches2.size() > 0) {
        qDebug() << "actual" << convert(listModel.storage()).join(",");
        qDebug() << patches;

        qDebug() << "Result";
        qDebug() << to;
        qDebug() << listModel.storage();
        qDebug() << patches2;
    }

    QVERIFY(patches2.size() == 0);
}

void QSyncableTests::diffRunner_randomMove()
{
    QVariantList from;
    int count = 30;
    for (int i = 0 ; i < count;i++) {
        QVariantMap item;
        item["id"] = i;
        item["value"] = i;
        from << item;
    }

    QVariantList to = from;

    for (int i = 0 ; i < count ;i++) {
        int f = qrand() % to.size();
        int t = qrand() % to.size();
        to.move(f,t);
    }

    qDebug() << "from" << convert(from).join(",");
    qDebug() << "to" << convert(to).join(",");

    QSListModel listModel;

    listModel.setStorage(from);

    QSDiffRunner runner;
    runner.setKeyField("id");

    QList<QSPatch> patches = runner.compare(from, to);
    runner.patch(&listModel, patches);

    QList<QSPatch> patches2 = runner.compare(to, listModel.storage());

    if (patches2.size() > 0) {
        qDebug() << "actual" << convert(listModel.storage()).join(",");
        qDebug() << patches;

        qDebug() << "Result";
        qDebug() << to;
        qDebug() << listModel.storage();
        qDebug() << patches2;
    }

    QVERIFY(patches2.size() == 0);

}

void QSyncableTests::diffRunner_complex()
{
    QFETCH(QStringList, from);
    QFETCH(QStringList, to);

    QVariantList fromList, toList;

    fromList = convert(from);
    toList = convert(to);

    QSListModel listModel;
    listModel.setStorage(fromList);

    QSDiffRunner runner;
    runner.setKeyField("id");

    QList<QSPatch> patches = runner.compare(fromList, toList);

    runner.patch(&listModel, patches);

    QList<QSPatch> patches2 = runner.compare(toList, listModel.storage());

    if (patches2.size() > 0) {
        qDebug() << "from" << from;
        qDebug() << "toList" << to;
        qDebug() << "actual" << convert(listModel.storage()).join(",");
        qDebug() << patches;

        qDebug() << "Result";
        qDebug() << to;
        qDebug() << listModel.storage();
        qDebug() << patches2;
    }

    QVERIFY(patches2.size() == 0);
}

void QSyncableTests::diffRunner_complex_data()
{
    QTest::addColumn<QStringList >("from");
    QTest::addColumn<QStringList >("to");
    QStringList from, to;

    // Data set 1 - Move and remove last item //
    from = QString("0,1,2,3,4,5,6,7,8,9").split(",");
    to = QString("3,0,1,2,4,5,6,8,11,10").split(",");
    QTest::newRow("Data1") << from << to;

}

void QSyncableTests::listModel_insert()
{
    QSListModel* model = new QSListModel();

    QVariantList expected;
    QVariantMap a,b,c,d;
    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";

    model->setStorage(QVariantList() << a << b);

    model->insert(1, QVariantList() << c << d);

    expected << a << c << d << b;

    QVERIFY(expected == model->storage());


    delete model;
}

void QSyncableTests::listModel_roleNames()
{
    QSListModel* model = new QSListModel();
    QCOMPARE(model->roleNames().size(), 0);
    delete model;

    QVariantMap item;
    item["id"] = "test";
    item["value"] = "test";

    QVariantMap item2;
    item2 = item;
    item2["order"] = 3;

    model = new QSListModel();
    model->append(item);
    QCOMPARE(model->roleNames().size(), 2);
    delete model;

    model = new QSListModel();
    model->insert(0, item);
    QCOMPARE(model->roleNames().size(), 2);
    delete model;

    model = new QSListModel();
    model->insert(0, QVariantList() << item << item);
    QCOMPARE(model->roleNames().size(), 2);
    delete model;

    QVariantList list;
    list.append(item);
    model = new QSListModel();
    model->setStorage(list);
    QCOMPARE(model->roleNames().size(), 2);

    list.clear();
    list.append(item2);
    model->setStorage(list);
    QCOMPARE(model->roleNames().size(), 2);

    delete model;
}

