//
// Created by ����Դ on 24-5-16.
//

#include "LGraph.h"
#include "GraphException.h"
#include <algorithm>
#include <regex>

namespace Graph {
    LGraph::LGraph(bool directed)
        : n_verts(0), m_edges(0), directed(directed), ver_list(std::vector<HeadNode>()) {}

    // �ж϶��������
    bool LGraph::exist_vertex(const std::string& name) const {
        return vertex_map.find(name) != vertex_map.end()
            && deleted.find(name) == deleted.end();
    }

    // ���붥�㣨��Ψһ��У�飩
    void LGraph::InsertVertex(const LocationInfo& vertex_info) {
        if (exist_vertex(vertex_info.name)) {
            std::cerr << "�����Ѵ���: " << vertex_info.name << std::endl;
            return;
        }

        const Vertex new_id = ver_list.size();
        ver_list.emplace_back(HeadNode(vertex_info));
        vertex_map[vertex_info.name] = new_id;
        n_verts++;

        type_index[vertex_info.type].push_back(new_id); //ά����������

        // �ָ���ɾ���Ķ���
        if (deleted.count(vertex_info.name)) {
            deleted.erase(vertex_info.name);
        }
        std::cout << "�������ɹ�: " << vertex_info.name << std::endl;
    }

    // ɾ�����㼰������
    void LGraph::DeleteVertex(const LocationInfo& vertex_info) {
        auto it = vertex_map.find(vertex_info.name);
        if (it == vertex_map.end() || deleted.count(vertex_info.name)) {
            std::cerr << "���㲻����: " << vertex_info.name << std::endl;
            return;
        }

        const Vertex target = it->second;
        const VertInfo& v = ver_list[target].data;

        auto& ids = type_index[v.type];
        ids.erase(std::remove(ids.begin(), ids.end(), target), ids.end());
          
        // ɾ�����г���
        m_edges -= ver_list[target].adj.size();
        ver_list[target].adj.clear();

        // ɾ��������ߣ�����ͼ�Զ�����
        if (directed) {
            for (auto& head : ver_list) {
                head.adj.remove_if([target](const EdgeNode& e) {
                    return e.dest == target;
                    });
            }
        }

        // ���Ϊ��ɾ��
        deleted.insert(vertex_info.name);
        n_verts--;
        std::cout << "����ɾ���ɹ�: " << vertex_info.name << std::endl;
    }

    void LGraph::UpdateVertex(const LocationInfo& old_info, LocationInfo& new_info) {
        if (!exist_vertex(old_info.name)) return;

        const Vertex id = vertex_map[old_info.name];
        VertInfo& old_data = ver_list[id].data;
        if (old_data.type != new_info.type) {
            auto& old_ids = type_index[old_data.type];
            old_ids.erase(std::remove(old_ids.begin(), old_ids.end(), id), old_ids.end());
            type_index[new_info.type].push_back(id);
        }
        old_data = new_info;
        vertex_map.erase(old_info.name);
        vertex_map[new_info.name] = id;
        std::cout << "������³ɹ�: " << old_info.name << " -> " << new_info.name << std::endl;
    }

    // ͨ�����Ʋ�ѯ������Ϣ
    VertInfo LGraph::GetVertex(const std::string& name) const {
        const auto it = vertex_map.find(name);
        if (it != vertex_map.end() && !deleted.count(name)) {
            return ver_list[it->second].data;
        }
        return VertInfo{}; // ������Чֵ
    }

    // ͨ��ID��ѯ������Ϣ
    VertInfo LGraph::GetVertex(Vertex vertex) const {
        if (vertex < ver_list.size()) {
            const std::string name = ver_list[vertex].data.name;
            if (!deleted.count(name)) {
                return ver_list[vertex].data;
            }
        }
        return VertInfo{};
    }

    //���������ݶ������Ͳ�ѯ������Ϣ�Ĺ���
    std::vector<VertInfo> LGraph::GetVerticesByType(const std::string& type) const {
        auto it = type_index.find(type);
        if (it == type_index.end()) return{};
        std::vector<VertInfo> result;
        for (Vertex id : it->second) {
            if (deleted.count(ver_list[id].data.name) == 0) { 
                result.push_back(ver_list[id].data);
            }
        }
        return result;
    }

    std::vector<VertInfo> LGraph::GetVerticesByTypePattern(const std::string& pattern) const {
        std::regex re(pattern);
        std::vector<VertInfo> result;
        for (const auto& head : ver_list) {
            if (std::regex_match(head.data.type, re)) {
                result.push_back(head.data);
            }
        }
        return result;
    }

    //���������ݶ����ѯ�������ڱߵĹ���
    std::vector<EdgeNode> LGraph::GetConnectedEdges(const std::string& name) const {
        std::vector<EdgeNode> edges;
        if (!exist_vertex(name)) return edges;

        const Vertex u = vertex_map.at(name);
        for (const EdgeNode& edge : ver_list[u].adj) {
            edges.push_back(edge);
        }
        return edges;
    }

    // �жϱߴ�����
    bool LGraph::exist_edge(const std::string& x, const std::string& y) const {
        const auto x_it = vertex_map.find(x);
        const auto y_it = vertex_map.find(y);
        if (x_it == vertex_map.end() || y_it == vertex_map.end()) return false;

        const Vertex x_id = x_it->second;
        const Vertex y_id = y_it->second;

        // �����ڽӱ����Ŀ���
        for (const auto& edge : ver_list[x_id].adj) {
            if (edge.dest == y_id) return true;
        }

        // ����ͼ���鷴��
        if (!directed) {
            for (const auto& edge : ver_list[y_id].adj) {
                if (edge.dest == x_id) return true;
            }
        }

        return false;
    }

    // ����ߣ�����ͻ��⣩
    void LGraph::InsertEdge(const std::string& x, const std::string& y, GElemSet weight) {
        /*if (!exist_vertex(x) || !exist_vertex(y)) {
            std::cerr << "���㲻����" << std::endl;
            return;
        }

        const Vertex x_id = vertex_map[x];
        const Vertex y_id = vertex_map[y];

        // �����Ƿ��Ѵ���
        for (const auto& edge : ver_list[x_id].adj) {
            if (edge.dest == y_id) {
                std::cerr << "���Ѵ���: " << x << "-" << y << std::endl;
                return;
            }
        }

        // �������
        ver_list[x_id].adj.emplace_back(x_id, y_id, weight);
        m_edges++;

        // ����ͼ��ӷ����
        if (!directed) {
            ver_list[y_id].adj.emplace_back(y_id, x_id, weight);
        } */
        if (!exist_vertex(x) || !exist_vertex(y)) {
            throw GraphException("���㲻����");
        }

        Vertex x_id = vertex_map[x];
        Vertex y_id = vertex_map[y];

        // ʹ�� edge_index ���ټ����Ƿ����
        if (edge_index[x_id].find(y_id) != edge_index[x_id].end()) {
            throw GraphException("���Ѵ���: " + x + "-" + y);
        }

        // ������߲���������
        ver_list[x_id].adj.emplace_back(x_id, y_id, weight);
        edge_index[x_id][y_id] = &ver_list[x_id].adj.back();
        m_edges++;

        // ����ͼ��ӷ����
        if (!directed) {
            ver_list[y_id].adj.emplace_back(y_id, x_id, weight);
            edge_index[y_id][x_id] = &ver_list[y_id].adj.back();
        }
        std::cout << "�ɹ���ӱ�: " << x << " - " << y << std::endl;
    }

    // ͨ��������ɾ����
    void LGraph::DeleteEdge(const std::string& x, const std::string& y) {
        if (!exist_vertex(x) || !exist_vertex(y)) return;

        Vertex x_id = vertex_map[x];
        Vertex y_id = vertex_map[y];

        // ɾ�������
        auto& adj_x = ver_list[x_id].adj;
        adj_x.remove_if([y_id](const EdgeNode& e) { return e.dest == y_id; });

        // ����ͼɾ�������
        if (!directed) {
            auto& adj_y = ver_list[y_id].adj;
            adj_y.remove_if([x_id](const EdgeNode& e) { return e.dest == x_id; });
        }
        m_edges -= (directed ? 1 : 2);
        std::cout << "�ɹ�ɾ����: " << x << " - " << y << std::endl;
    }

    // ��ȡ��Ȩֵ
    GElemSet LGraph::GetEdge(const std::string& x, const std::string& y) const {
        const auto x_it = vertex_map.find(x);
        const auto y_it = vertex_map.find(y);
        if (x_it == vertex_map.end() || y_it == vertex_map.end()) return GElemSet{};

        const Vertex x_id = x_it->second;
        const Vertex y_id = y_it->second;

        for (const auto& edge : ver_list[x_id].adj) {
            if (edge.dest == y_id) return edge.weight;
        }

        return GElemSet{};
    }
    void LGraph::UpdateEdge(
        const std::string& x,
        const std::string& y,
        GElemSet new_weight
    ) {
        // ��������Լ��
        if (!exist_vertex(x) || !exist_vertex(y)) {
            std::cerr << "���㲻����" << std::endl;
            return;
        }

        const Vertex x_id = vertex_map[x];
        const Vertex y_id = vertex_map[y];
        bool found = false;

        // ����x->y�ı�
        for (auto& edge : ver_list[x_id].adj) {
            if (edge.dest == y_id) {
                edge.weight = new_weight;
                found = true;
                break;
            }
        }

        // ����ͼ�����y->x�ı�
        if (!directed) {
            for (auto& edge : ver_list[y_id].adj) {
                if (edge.dest == x_id) {
                    edge.weight = new_weight;
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            std::cerr << "�߲�����: " << x << "-" << y << std::endl;
        }
    }

    std::vector<EdgeNode> LGraph::SortedEdges(
        std::function<bool(const GElemSet&, const GElemSet&)> cmp
    ) const {
        std::vector<EdgeNode> all_edges;
        std::set<std::pair<Vertex, Vertex>> added; // ��������ͼȥ��

        for (const auto& head : ver_list) {
            for (const auto& edge : head.adj) {
                // ��������ͼ�ظ���
                if (!directed) {
                    const auto ordered_pair = (edge.from < edge.dest)
                        ? std::make_pair(edge.from, edge.dest)
                        : std::make_pair(edge.dest, edge.from);

                    if (added.count(ordered_pair)) continue;
                    added.insert(ordered_pair);
                }

                all_edges.push_back(edge);
            }
        }

        std::sort(all_edges.begin(), all_edges.end(),
            [&cmp](const EdgeNode& a, const EdgeNode& b) {
                return cmp(a.weight, b.weight);
            });

        return all_edges;
    }

}