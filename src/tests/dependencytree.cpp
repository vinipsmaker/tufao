#include "dependencytree.h"
#include <QtTest/QTest>
#include "../priv/dependencytree.h"

using namespace Tufao;

void DependencyTreeTest::sort_data()
{
    QTest::addColumn<QStringList>("nodes");
    QTest::addColumn<QList<QStringList>>("dependencies");
    QTest::addColumn<QStringList>("evaluationOrder");

    QTest::newRow("empty set")
        << QStringList{}
        << QList<QStringList>{}
        << QStringList{};

    QTest::newRow("A")
        << QStringList{"A"}
        << QList<QStringList>{{}}
        << QStringList{"A"};

    QTest::newRow("A -> B")
        << QStringList{"A", "B"}
        << QList<QStringList>{{"B"}, {}}
        << QStringList{"B", "A"};

    QTest::newRow("A -> B (2)")
        << QStringList{"B", "A"}
        << QList<QStringList>{{}, {"B"}}
        << QStringList{"B", "A"};

    QTest::newRow("A -> {B, C}, C -> B")
        << QStringList{"A", "B", "C"}
        << QList<QStringList>{{"B", "C"}, {}, {"B"}}
        << QStringList{"B", "C", "A"};

    QTest::newRow("A -> {B, C}, C -> B (2)")
        << QStringList{"A", "C", "B"}
        << QList<QStringList>{{"B", "C"}, {"B"}, {}}
        << QStringList{"B", "C", "A"};

    QTest::newRow("A -> {B, C}, C -> B (3)")
        << QStringList{"B", "A", "C"}
        << QList<QStringList>{{}, {"B", "C"}, {"B"}}
        << QStringList{"B", "C", "A"};

    QTest::newRow("A -> {B, C}, C -> B (4)")
        << QStringList{"B", "C", "A"}
        << QList<QStringList>{{}, {"B"}, {"B", "C"}}
        << QStringList{"B", "C", "A"};

    QTest::newRow("A -> {B, C}, C -> B (5)")
        << QStringList{"C", "A", "B"}
        << QList<QStringList>{{"B"}, {"B", "C"}, {}}
        << QStringList{"B", "C", "A"};

    QTest::newRow("A -> {B, C}, C -> B (6)")
        << QStringList{"C", "B", "A"}
        << QList<QStringList>{{"B"}, {}, {"B", "C"}}
        << QStringList{"B", "C", "A"};

    QTest::newRow("A -> C, C -> B")
        << QStringList{"C", "B", "A"}
        << QList<QStringList>{{"B"}, {}, {"C"}}
        << QStringList{"B", "C", "A"};
}

void DependencyTreeTest::sort()
{
    QFETCH(QStringList, nodes);
    QFETCH(QList<QStringList>, dependencies);
    QFETCH(QStringList, evaluationOrder);

    DependencyTree<QString, QStringList> tree;

    for (int i = 0;i != nodes.size();++i) {
        QVERIFY(tree.addNode(nodes[i], dependencies[i]));
        QVERIFY(!tree.addNode(nodes[i]));
        QVERIFY(!tree.addNode(nodes[i], dependencies[i]));
    }

    QVERIFY(!tree.hasUnsatisfiedDependency());
    QCOMPARE(tree.sorted(), evaluationOrder);
}

void DependencyTreeTest::rootNodesOnly_data()
{
    QTest::addColumn<QStringList>("nodes");

    QTest::newRow("A, B, C") << QStringList{"A", "B", "C"};
    QTest::newRow("A, a, B, C") << QStringList{"A", "a", "B", "C"};
}

void DependencyTreeTest::rootNodesOnly()
{
    QFETCH(QStringList, nodes);

    DependencyTree<QString> tree;

    for (const auto &n: nodes) {
        QVERIFY(tree.addNode(n));
        QVERIFY(!tree.addNode(n));
    }

    QCOMPARE(tree.sorted().size(), nodes.size());
}

void DependencyTreeTest::impossibleEvaluation()
{
    DependencyTree<QString, QStringList> tree;

    QVERIFY(tree.addNode("A", QStringList{"B"}));
    QVERIFY(!tree.addNode("A"));
    QVERIFY(!tree.addNode("A", QStringList{"B"}));

    QVERIFY(!tree.addNode("B", QStringList{"A"}));
    QVERIFY(tree.addNode("B", QStringList{"C"}));

    QVERIFY(!tree.addNode("C", QStringList{"B"}));
    QVERIFY(!tree.addNode("C", QStringList{"A"}));
    QVERIFY(tree.addNode("C"));
}

void DependencyTreeTest::unsatisfiedDependency()
{
    DependencyTree<QString, QStringList> tree;

    QVERIFY(!tree.hasUnsatisfiedDependency());

    QVERIFY(tree.addNode("A"));

    QVERIFY(!tree.hasUnsatisfiedDependency());

    QVERIFY(tree.addNode("B", QStringList{"C"}));

    QVERIFY(tree.hasUnsatisfiedDependency());

    QVERIFY(tree.addNode("C"));

    QVERIFY(!tree.hasUnsatisfiedDependency());
    QCOMPARE(tree.sorted().size(), 3);
}

void DependencyTreeTest::unrelated()
{
    {
        DependencyTree<QString, QStringList> tree;

        QVERIFY(tree.addNode("A"));
        QVERIFY(tree.addNode("B", {"A"}));
        QVERIFY(tree.addNode("C", {"A"}));

        bool found = false;

        if (tree.sorted() == QStringList{"A", "B", "C"})
            found = true;
        else if (tree.sorted() == QStringList{"A", "C", "B"})
            found = true;

        QVERIFY(!tree.hasUnsatisfiedDependency());
        QVERIFY(found);
    }
    {
        DependencyTree<QString, QStringList> tree;

        QCOMPARE(tree.sorted().size(), 0);

        QVERIFY(tree.addNode("A"));
        QVERIFY(tree.addNode("B", QStringList{"A"}));
        QVERIFY(tree.addNode("C"));

        bool found = false;

        if (tree.sorted() == QStringList{"A", "B", "C"})
            found = true;
        else if (tree.sorted() == QStringList{"A", "C", "B"})
            found = true;
        else if (tree.sorted() == QStringList{"C", "A", "B"})
            found = true;

        QVERIFY(!tree.hasUnsatisfiedDependency());
        QVERIFY(found);
    }
}

void DependencyTreeTest::diamond()
{
    DependencyTree<QString, QStringList> tree;

    QVERIFY(tree.addNode("A"));
    QVERIFY(tree.addNode("B", QStringList{"A"}));
    QVERIFY(tree.addNode("C", QStringList{"A"}));
    QVERIFY(tree.addNode("D", QStringList{"B", "C"}));

    bool found = false;

    if (tree.sorted() == QStringList{"A", "B", "C", "D"})
        found = true;
    else if (tree.sorted() == QStringList{"A", "C", "B", "D"})
        found = true;

    QVERIFY(!tree.hasUnsatisfiedDependency());
    QVERIFY(found);
}

void DependencyTreeTest::invalidData()
{
    DependencyTree<QString, QStringList> tree;

    QVERIFY(!tree.addNode("A", QStringList{"A"}));
    QVERIFY(tree.addNode("A", QStringList{"B"}));
    QCOMPARE(tree.sorted(), QStringList{});
}

QTEST_APPLESS_MAIN(DependencyTreeTest)
