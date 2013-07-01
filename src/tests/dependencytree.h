#include <QtCore/QObject>

class DependencyTreeTest: public QObject
{
    Q_OBJECT
private slots:
    void sort_data();
    void sort();
    void rootNodesOnly_data();
    void rootNodesOnly();
    void impossibleEvaluation();
    void unsatisfiedDependency();
    void unrelated();
    void diamond();
    void invalidData();
};
