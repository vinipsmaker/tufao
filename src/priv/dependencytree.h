#ifndef TUFAO_DEPENDENCYTREE_H
#define TUFAO_DEPENDENCYTREE_H

#include <QtCore/QSet>
#include <QtCore/QList>
#include <QtCore/QPair>

#include <algorithm>

namespace Tufao {

// If an exception is throw, there will be no memory leaks, but the object state
// may become invalid
template<class T, class Container = QList<T> >
class DependencyTree
{
public:
    DependencyTree() = default;
    DependencyTree(const DependencyTree &) = delete;
    DependencyTree &operator =(const DependencyTree &) = delete;

    bool addNode(const T &value, const Container &dependencies = QStringList{})
    {
        QSet<T> uniqueDependencies;

        // fill uniqueDependencies
        for (const auto &d: dependencies) {
            if (d != T())
                uniqueDependencies.insert(d);
        }

        if (uniqueDependencies.empty()) {
            if (rootNodes.contains(value))
                return false;

            unsatisfiedDependencies.remove(value);
            rootNodes.insert(value);
            return true;
        }

        auto &edges = this->edges;
        auto &rootNodes = this->rootNodes;

        auto contains = [&edges,&rootNodes](const T &value) -> bool {
            if (rootNodes.contains(value))
                return true;

            for (const auto &edge: edges) {
                if (edge.first == value || edge.second == value)
                    return true;
            }

            return false;
        };

        auto depends = [&edges](const T &first, const T &second) -> bool {
            QList<T> remaining{first};

            for (QSet<T> visited;remaining.size();) {
                T current = remaining.front();

                if (current == second)
                    return true;

                remaining.pop_front();
                visited.insert(current);

                std::for_each(std::lower_bound(edges.begin(), edges.end(),
                                               qMakePair(current, T())),
                              std::upper_bound(edges.begin(), edges.end(),
                                               qMakePair(current, T())),
                              [&visited,&remaining](const QPair<T, T> &value) {
                    if (!visited.contains(value.second))
                        remaining.push_back(value.second);
                });
            }

            return false;
        };

        // Node already was added
        if (contains(value)) {
            // Node metadata already was added
            if (!unsatisfiedDependencies.contains(value))
                return false;

            for (const auto &d: uniqueDependencies) {
                // Circular dependency
                if (depends(d, value))
                    return false;
            }

            unsatisfiedDependencies.remove(value);
        }

        for (const auto &d: uniqueDependencies) {
            if (!contains(d))
                unsatisfiedDependencies.insert(d);

            auto edge = qMakePair(value, d);
            edges.insert(std::lower_bound(edges.begin(), edges.end(), edge),
                         edge);
        }

        return true;
    }

    bool hasUnsatisfiedDependency() const
    {
        return unsatisfiedDependencies.size();
    }

    Container sorted() const
    {
        Container sorted;

        auto current = edges;

        for (const auto &n: rootNodes) {
            sorted.push_back(n);

            for (typename QList<QPair<T, T>>::size_type i = 0
                 ;i != current.size();++i) {
                if (current[i].second == n)
                    current.removeAt(i--);
            }
        }

        auto hasDependencies = [&current](const T &value) -> bool {
            auto it = std::lower_bound(current.begin(), current.end(),
                                       qMakePair(value, T()));

            if (it != current.end() && it->first == value)
                return true;

            return false;
        };

        auto findSink = [&current,&hasDependencies]() -> T {
            for (const auto &e: current) {
                QSet<T> visited;

                if (visited.contains(e.second))
                    continue;

                if (!hasDependencies(e.second))
                    return e.second;

                visited.insert(e.second);
            }

            return T();
        };

        while (current.size()) {
            auto sink = findSink();
            sorted.push_back(sink);

            for (typename QList<QPair<T, T>>::size_type i = 0
                 ;i != current.size();++i) {
                if (current[i].second == sink)
                    current.removeAt(i--);
            }
        }

        return sorted;
    }

private:
    QList<QPair<T, T>> edges;
    // Nodes referenced, but not added explicitly by addNode
    QSet<T> unsatisfiedDependencies;
    // Nodes without dependencies
    QSet<T> rootNodes;
};

} // namespace Tufao

#endif // TUFAO_DEPENDENCYTREE_H
