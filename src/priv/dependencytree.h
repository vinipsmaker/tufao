/*  This file is part of the Tufão project
    Copyright (C) 2013 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_DEPENDENCYTREE_H
#define TUFAO_DEPENDENCYTREE_H

#include <QtCore/QSet>
#include <QtCore/QList>
#include <QtCore/QPair>

#include <algorithm>

namespace Tufao {

// If an exception is throw, there will be no memory leaks, but the object state
// may become invalid
template<class T, class Container = QList<T>>
class DependencyTree
{
public:
    DependencyTree() = default;
    DependencyTree(const DependencyTree &) = delete;
    DependencyTree &operator =(const DependencyTree &) = delete;

    bool addNode(const T &value, const Container &dependencies = Container{})
    {
        QSet<T> uniqueDependencies;

        // fill uniqueDependencies
        for (const auto &d: dependencies) {
            if (d == value)
                return false;

            if (d != T())
                uniqueDependencies.insert(d);
        }

        if (uniqueDependencies.empty()) {
            auto nodeExists = [this](const T &value) -> bool {
                if (unsatisfiedDependencies.contains(value))
                    return false;

                if (rootNodes.contains(value))
                    return true;

                for (const auto &e: edges) {
                    if (e.first == value || e.second == value)
                        return true;
                }

                return false;
            };

            if (nodeExists(value))
                return false;

            unsatisfiedDependencies.remove(value);
            rootNodes.insert(value);
            return true;
        }

        auto contains = [this](const T &value) -> bool {
            if (rootNodes.contains(value))
                return true;

            for (const auto &edge: edges) {
                if (edge.first == value || edge.second == value)
                    return true;
            }

            return false;
        };

        auto depends = [this](const T &first, const T &second) -> bool {
            QList<T> remaining{first};
            QSet<T> visited;

            while (remaining.size()) {
                T current = remaining.front();

                if (current == second)
                    return true;

                remaining.pop_front();
                visited.insert(current);

                for (auto it = std::lower_bound(edges.begin(), edges.end(),
                                                qMakePair(current, T())),
                         end = edges.end();it != end;++it) {
                    if (it->first != current)
                        break;

                    if (!visited.contains(it->second))
                        remaining.push_back(it->second);
                }
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
        if (unsatisfiedDependencies.size())
            return Container{};

        Container sorted;

        auto current = edges;

        auto nodeExists = [&current,this](const T &value) -> bool {
            if (unsatisfiedDependencies.contains(value))
                return false;

            if (rootNodes.contains(value))
                return true;

            for (const auto &e: current) {
                if (e.first == value || e.second == value)
                    return true;
            }

            return false;
        };

        for (const auto &n: rootNodes) {
            sorted.push_back(n);

            QSet<T> removed;

            for (typename QList<QPair<T, T>>::size_type i = 0
                 ;i != current.size();++i) {
                if (current[i].second == n) {
                    removed.insert(current[i].first);
                    current.removeAt(i--);
                }
            }

            for (const auto &n: removed) {
                if (!nodeExists(n))
                    sorted.push_back(n);
            }
        }

        auto hasDependencies = [&current](const T &value) -> bool {
            auto it = std::lower_bound(current.begin(), current.end(),
                                       qMakePair(value, T()));

            if (it != current.end() && it->first == value)
                return true;

            return false;
        };

        auto findSink = [&current,&hasDependencies,&nodeExists]() -> T {
            QSet<T> visited;

            for (const auto &e: current) {
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

            QSet<T> removed;

            for (typename QList<QPair<T, T>>::size_type i = 0
                 ;i != current.size();++i) {
                if (current[i].second == sink) {
                    removed.insert(current[i].first);
                    current.removeAt(i--);
                }
            }

            for (const auto &n: removed) {
                if (!nodeExists(n))
                    sorted.push_back(n);
            }
        }

        return sorted;
    }

private:
    // See also boost::container::flat_map
    QList<QPair<T, T>> edges;
    // Nodes referenced, but not added explicitly by addNode
    QSet<T> unsatisfiedDependencies;
    // Nodes without dependencies
    QSet<T> rootNodes;
};

} // namespace Tufao

#endif // TUFAO_DEPENDENCYTREE_H
