//
// Created by ����Դ on 24-5-16.
//

#ifndef LGRAPH_LGRAPH_H
#define LGRAPH_LGRAPH_H

#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include "LocationInfo.h"
#include <unordered_map>


namespace Graph {
    typedef size_t Vertex;    /* ����ID���ͣ�����ʹ��size_t���� */
    typedef int GElemSet;  /* ��Ȩ���ͣ�����ʹ��int���� */
    typedef LocationInfo VertInfo; /* ������Ϣ���ͣ�����ʹ��LocationInfo */
    constexpr int NIL = -1;         /* ���㲻����ʱ�ķ���ֵ */

    struct EdgeNode {
        Vertex from, dest;     /* �ߵ���һ�˵��� */
        GElemSet weight; /* Ȩ�� */
        EdgeNode(Vertex from, Vertex dest, GElemSet weight)
            : from(from), dest(dest), weight(weight) {}
    };

    struct HeadNode {
        std::list<EdgeNode> adj; /* �ڽӱ�ʹ��STL������listʵ�� */
        VertInfo data;           /* �洢������Ϣ */
        HeadNode() = default;

        explicit HeadNode(const VertInfo& data_)
            : data(data_), adj(std::list<EdgeNode>()) {}
    };

    class LGraph {
    private:
        int n_verts;   // ������
        int m_edges;   // ����
        bool directed; // trueΪ����ͼ��falseΪ����ͼ
        std::vector<HeadNode> ver_list; // �ڽӱ�
        std::map<std::string, Vertex> vertex_map; // �ص����͵ص�IDӳ�䣬ID��0��ʼ
        std::set<std::string> deleted; // �Ѿ�ɾ���ĵص�
        std::unordered_map<Vertex, std::unordered_map<Vertex, EdgeNode*>> edge_index;
        std::unordered_map<std::string, std::vector<Vertex>> type_index; //  �ص���������

    public:
        bool exist_vertex(const std::string& name) const; // ��ѯ�ڵ��Ƿ���ڣ����ڷ���true
        bool exist_edge(const std::string& vertex_x_name, const std::string& vertex_y_name) const; // ��ѯ���Ƿ���ڣ����ڷ���true
        explicit LGraph(bool directed = false);
        const int& VertexCount() const { return n_verts; } // ��ȡ������
        int VertextCount() const { return n_verts; } // ��const�汾
        const int& EdgesCount() const { return m_edges; } // ��ȡ����
        const std::vector<HeadNode>& List() const { return ver_list; } // ��ȡ�ڽӱ�
        std::vector<HeadNode>& List() { return ver_list; } // ��const�汾
        const std::map<std::string, Vertex>& Map() const { return vertex_map; } //��ȡӳ���

        bool IsDirected() const { return directed; }
        std::vector<VertInfo> GetVerticesByType(const std::string& type) const; // �������ܣ����ݶ������Ͳ�ѯ���ж���
        std::vector<VertInfo> GetVerticesByTypePattern(const std::string& pattern) const;
        std::vector<EdgeNode> GetConnectedEdges(const std::string& name) const; //�������ܣ���ȡ��ָ�������ڽӵı�
        bool IsDeleted(Vertex v) const {
            const VertInfo& info = ver_list[v].data;
            return deleted.find(info.name) != deleted.end();
        }
        int ValidVertexCount() const {
            return n_verts - deleted.size();
        }

        void InsertVertex(const LocationInfo& vertex_info); // �����
        void DeleteVertex(const LocationInfo& vertex_info); // ɾ���㣬ͬʱɾ�����ڱ�
        void UpdateVertex(const LocationInfo& old_info, LocationInfo& new_info); //�޸ĵ���Ϣ
        VertInfo GetVertex(const std::string& name) const; // ͨ���ص��� ��ѯ��
        VertInfo GetVertex(Vertex vertex) const; // ͨ����ID��ѯ��

        void InsertEdge(const std::string& vertex_x_name, const std::string& vertex_y_name, GElemSet weight);// �����
        void DeleteEdge(const std::string& vertex_x_name, const std::string& vertex_y_name); // ɾ����
        void DeleteEdge(Vertex vertex_x, Vertex vertex_y); // ɾ����
        void UpdateEdge(const std::string& vertex_x_name, const std::string& vertex_y_name, GElemSet new_weight); // �޸ı�
        GElemSet GetEdge(const std::string& vertex_x_name, const std::string& vertex_y_name) const; // ��ѯ��
        std::vector<EdgeNode> SortedEdges(
            std::function<bool(const GElemSet&, const GElemSet&)> cmp = std::less<>()
        ) const; // ���ر�Ȩ��ֵ����Ľ��(��С����)
    };
}
#endif  // LGRAPH_LGRAPH_H
