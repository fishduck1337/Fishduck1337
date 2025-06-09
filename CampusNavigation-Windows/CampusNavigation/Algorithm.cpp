//
// Created by ����Դ on 24-10-13.
//
#include "Algorithm.h"
#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <vector>
#include <algorithm>
#include "GraphException.h"

namespace Graph {
    namespace Algorithm {
        bool Algorithm::IsHamiltonianPossible(const LGraph& graph) {
            if (!IsConnected(graph)) return false;

            for (Vertex v = 0; v < graph.VertexCount(); ++v) {
                if (!graph.IsDeleted(v) &&
                    graph.List()[v].adj.size() < graph.ValidVertexCount() / 2) {
                    return false;
                }
            }
            return true;
        }

        Algorithm::HamiltonianResult Algorithm::FindHamiltonianCycle(
            const LGraph& graph,
            const std::string& start
        ) {
            HamiltonianResult result;
            constexpr int MAX_DEPTH = 50;
            const auto start_time = std::chrono::system_clock::now();
            if (!graph.exist_vertex(start)) return result;

            const Vertex start_v = graph.Map().at(start);
            const int total_verts = graph.ValidVertexCount();
            if (total_verts < 3) return result; // ������Ҫ3�������γɻ�

            std::vector<bool> visited(graph.VertexCount(), false);
            std::vector<Vertex> current_path;
            int current_cost = 0;

            // �ݹ���ݺ���������֦�Ż���
            std::function<void(Vertex)> backtrack = [&](Vertex current) {
                visited[current] = true;
                current_path.push_back(current);
                current_cost += graph.GetVertex(current).visitTime;

                // ��֦����ǰ�����ѳ�����֪��Сֵ
                if (current_cost >= result.min_cost) {
                    visited[current] = false;
                    current_path.pop_back();
                    current_cost -= graph.GetVertex(current).visitTime;
                    return;
                }

                if (current_path.size() == total_verts) {
                    for (const auto& edge : graph.List()[current].adj) {
                        if (edge.dest == start_v) {
                            result.search_count++;
                            const int total = current_cost + graph.GetVertex(start_v).visitTime;
                            if (total < result.min_cost) {
                                result.min_cost = total;
                                result.min_path.clear();
                                for (Vertex v : current_path) {
                                    result.min_path.push_back(graph.GetVertex(v).name);
                                }
                                result.min_path.push_back(start);
                                result.exists = true;
                            }
                            break;
                        }
                    }
                    visited[current] = false;
                    current_path.pop_back();
                    current_cost -= graph.GetVertex(current).visitTime;
                    return;
                }

                // ��ȡ�������ڽӶ��㣨������ʱ������
                std::vector<std::pair<Vertex, int>> neighbors;
                for (const auto& edge : graph.List()[current].adj) {
                    const Vertex next = edge.dest;
                    if (!visited[next] && !graph.IsDeleted(next)) {
                        const int cost = graph.GetVertex(next).visitTime;
                        neighbors.emplace_back(next, cost);
                    }
                }
                std::sort(neighbors.begin(), neighbors.end(),
                    [](const auto& a, const auto& b) { return a.second < b.second; });

                // �ݹ�̽���ڽӶ���
                for (const auto& pair : neighbors) {
                    Vertex next = pair.first;
                    backtrack(next);
                }

                backtrack: // ���ݻָ�״̬
                visited[current] = false;
                current_path.pop_back();
                current_cost -= graph.GetVertex(current).visitTime;
                };

            backtrack(start_v);
            return result;
        }

        std::list<Vertex> Algorithm::GetCircuit(LGraph& graph, Vertex start) {
            std::list<Vertex> circuit;
            if (graph.List()[start].adj.empty()) return circuit;

            std::stack<Vertex> stk;
            stk.push(start);

            while (!stk.empty()) {
                Vertex u = stk.top();
                if (!graph.List()[u].adj.empty()) {
                    Vertex v = graph.List()[u].adj.front().dest;
                    stk.push(v);
                    // ɾ����
                    graph.List()[u].adj.pop_front();
                    if (!graph.IsDirected()) {
                        // ����ͼɾ�������
                        auto& adj_v = graph.List()[v].adj;
                        adj_v.remove_if([u](const EdgeNode& e) { return e.dest == u; });
                    }
                }
                else {
                    circuit.push_front(u);
                    stk.pop();
                }
            }
            return circuit;
        }

        std::list<Vertex> Algorithm::EulerCircle(const LGraph& graph) {
            LGraph temp_graph = graph; // �������޸ı�
            std::list<Vertex> path;

            // Ѱ����ʼ��
            Vertex start = 0;
            for (; start < temp_graph.VertexCount(); ++start) {
                if (!temp_graph.List()[start].adj.empty()) break;
            }

            path = GetCircuit(temp_graph, start);

            // ����Ƿ���δ�����ı�
            for (const auto& head : temp_graph.List()) {
                if (!head.adj.empty()) return {}; // ����δ������
            }

            return path;
        }

        bool HaveEulerCircle(const LGraph& graph) {
            if (graph.IsDirected()) {
                // ����ͼ��ÿ��������ȵ��ڳ���
                std::unordered_map<Vertex, int> in_degree, out_degree;
                for (const auto& head : graph.List()) {
                    for (const auto& edge : head.adj) {
                        out_degree[edge.from]++;
                        in_degree[edge.dest]++;
                    }
                }
                for (Vertex v = 0; v < graph.VertexCount(); ++v) {
                    if (in_degree[v] != out_degree[v]) return false;
                }
            }
            else {
                // ����ͼ�����ж������Ϊż��
                for (const auto& head : graph.List()) {
                    if (head.adj.size() % 2 != 0) return false;
                }
            }
            return IsConnected(graph);
        }

        void Algorithm::BFSv(const LGraph& graph, Vertex v, std::vector<bool>& visited) {
            std::queue<Vertex> q;
            q.push(v);
            visited[v] = true;

            while (!q.empty()) {
                Vertex current = q.front();
                q.pop();

                // �����ڽӱ�ʱ������ɾ������
                for (const EdgeNode& edge : graph.List()[current].adj) {
                    Vertex neighbor = edge.dest;
                    if (!graph.IsDeleted(neighbor) && !visited[neighbor]) {
                        visited[neighbor] = true;
                        q.push(neighbor);
                    }
                }
            }
        }

        bool Algorithm::IsConnected(const LGraph& graph) {
            if (graph.VertexCount() == 0) return true;

            // ��ȡ��Ч�����б�
            std::vector<Vertex> valid_vertices;
            for (Vertex v = 0; v < graph.VertexCount(); ++v) {
                if (!graph.IsDeleted(v)) {
                    valid_vertices.push_back(v);
                }
            }
            if (valid_vertices.empty()) return true;

            // BFS ����
            std::vector<bool> visited(graph.VertexCount(), false);
            BFSv(graph, valid_vertices[0], visited);

            // �������Ч����
            for (Vertex v : valid_vertices) {
                if (!visited[v]) {
                    std::cerr << "δ��ͨ���㣺" << graph.GetVertex(v).name << std::endl;
                    return false;
                }
            }
            return true;
        }

        int GetShortestPath(    //Dijkstra�㷨
            const LGraph& graph,
            const std::string& start,
            const std::string& end,
            std::list<std::string>& path) {
            const auto& vertex_map = graph.Map();
            if (!graph.exist_vertex(start) || !graph.exist_vertex(end)) return -1;

            Vertex src = vertex_map.at(start);
            Vertex dest = vertex_map.at(end);

            // ���ȶ��д洢 (����, ����)
            using Pair = std::pair<int, Vertex>;
            std::priority_queue<Pair, std::vector<Pair>, std::greater<>> pq;

            std::vector<int> dist(graph.VertexCount(), INT_MAX);
            std::vector<Vertex> prev(graph.VertexCount(), -1);
            dist[src] = 0;
            pq.emplace(0, src);

            while (!pq.empty()) {
                //auto [d, u] = pq.top(); pq.pop(); ��֪��ΪʲôĬ��visual studioû��C++ 17������
                const auto& top = pq.top();
                int d = top.first;
                Vertex u = top.second;
                pq.pop();
                if (u == dest) {
                    std::stack<Vertex> reverse_path;
                    Vertex current = dest;
                    while (current != src) {
                        reverse_path.push(current);
                        current = prev[current];
                        if (current == -1) return -1; //·��������
                    }
                    reverse_path.push(src);

                    //ת��Ϊ�ڵ������б�
                    path.clear();
                    while (!reverse_path.empty()) {
                        path.push_back(graph.GetVertex(reverse_path.top()).name);
                        reverse_path.pop();
                    }
                    return d;
                }

                for (const auto& edge : graph.List()[u].adj) {
                    Vertex v = edge.dest;
                    int new_dist = d + edge.weight;
                    if (new_dist < dist[v]) {
                        dist[v] = new_dist;
                        prev[v] = u;
                        pq.emplace(new_dist, v);
                    }
                }
            }
            return -1; // ���ɴ�
        }

        int Algorithm::TopologicalShortestPath(
            const LGraph& graph,
            std::vector<std::string> path
        ) {
            // ת��Ϊ����ID����
            std::vector<Vertex> topo_order;
            for (const auto& name : path) {
                if (!graph.exist_vertex(name)) return -1;
                topo_order.push_back(graph.Map().at(name));
            }

            // DP�����ʼ��
            std::unordered_map<Vertex, int> dist;
            for (Vertex v : topo_order) {
                dist[v] = INT_MAX;
            }
            dist[topo_order[0]] = 0;

            // ���������ɳڱ�
            for (Vertex u : topo_order) {
                if (dist[u] == INT_MAX) continue;

                for (const auto& edge : graph.List()[u].adj) {
                    Vertex v = edge.dest;
                    if (dist[v] > dist[u] + edge.weight) {
                        dist[v] = dist[u] + edge.weight;
                    }
                }
            }

            return (dist[topo_order.back()] != INT_MAX) ?
                dist[topo_order.back()] : -1;
        }

        std::vector<EdgeNode> Algorithm::MinimumSpanningTree(const LGraph& graph) {
            if (!IsConnected(graph)) {
                throw GraphException("ͼ����ͨ");
            }

            // ��ȡ���б߲�������ɾ������ı�
            auto edges = graph.SortedEdges();
            edges.erase(std::remove_if(edges.begin(), edges.end(),
                [&graph](const EdgeNode& e) {
                    return graph.IsDeleted(e.from) || graph.IsDeleted(e.dest);
                }), edges.end());

            //����ͼ����
            if (!graph.IsDirected()) {
                std::map<std::pair<Vertex, Vertex>, int> edge_map;
                for (auto& edge : edges) {
                    Vertex u = edge.from, v = edge.dest;
                    if (u > v) std::swap(u, v);
                    auto key = std::make_pair(u, v);
                    if (!edge_map.count(key) || edge.weight < edge_map[key]) {
                        edge_map[key] = edge.weight;
                    }
                }
                edges.clear();
                for (const auto& entry : edge_map) {
                    const Vertex u = entry.first.first;
                    const Vertex v = entry.first.second;
                    const int weight = entry.second;
                    edges.emplace_back(u, v, weight);
                }
            }
            /*if (!graph.IsDirected()) {
                for (auto& edge : edges) {
                    if (edge.from > edge.dest) {
                        std::swap(edge.from, edge.dest);
                    }
                }
                // ȥ��
                std::sort(edges.begin(), edges.end(), [](const EdgeNode& a, const EdgeNode& b) {
                    return std::tie(a.from, a.dest) < std::tie(b.from, b.dest);
                    });
                auto last = std::unique(edges.begin(), edges.end(),
                    [](const EdgeNode& a, const EdgeNode& b) {
                        return a.from == b.from && a.dest == b.dest;
                    });
                edges.erase(last, edges.end());
            }*/

            // ��Ȩ����������(Kruskal�㷨)
            std::sort(edges.begin(), edges.end(), [](const EdgeNode& a, const EdgeNode& b) {
                return a.weight < b.weight;
                });

            DSU dsu(graph.VertexCount());
            std::vector<EdgeNode> mst;

            for (const auto& edge : edges) {
                Vertex u = edge.from;
                Vertex v = edge.dest;
                if (!dsu.same(u, v)) {
                    dsu.unite(u, v);
                    mst.push_back(edge);
                }
            }

            // ��֤������������Ч��������
            int valid_vertex_count = graph.ValidVertexCount();
            if (mst.size() != valid_vertex_count - 1) {
                throw GraphException("��С�����������쳣");
            }

            return mst;
        }
    }
}