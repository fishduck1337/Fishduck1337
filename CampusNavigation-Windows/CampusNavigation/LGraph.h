//
// Created by 刘凯源 on 24-5-16.
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
    typedef size_t Vertex;    /* 顶点ID类型，这里使用size_t类型 */
    typedef int GElemSet;  /* 边权类型，这里使用int类型 */
    typedef LocationInfo VertInfo; /* 顶点信息类型，这里使用LocationInfo */
    constexpr int NIL = -1;         /* 顶点不存在时的返回值 */

    struct EdgeNode {
        Vertex from, dest;     /* 边的另一端点编号 */
        GElemSet weight; /* 权重 */
        EdgeNode(Vertex from, Vertex dest, GElemSet weight)
            : from(from), dest(dest), weight(weight) {}
    };

    struct HeadNode {
        std::list<EdgeNode> adj; /* 邻接表，使用STL的链表list实现 */
        VertInfo data;           /* 存储顶点信息 */
        HeadNode() = default;

        explicit HeadNode(const VertInfo& data_)
            : data(data_), adj(std::list<EdgeNode>()) {}
    };

    class LGraph {
    private:
        int n_verts;   // 顶点数
        int m_edges;   // 边数
        bool directed; // true为有向图，false为无向图
        std::vector<HeadNode> ver_list; // 邻接表
        std::map<std::string, Vertex> vertex_map; // 地点名和地点ID映射，ID从0开始
        std::set<std::string> deleted; // 已经删除的地点
        std::unordered_map<Vertex, std::unordered_map<Vertex, EdgeNode*>> edge_index;
        std::unordered_map<std::string, std::vector<Vertex>> type_index; //  地点类型索引

    public:
        bool exist_vertex(const std::string& name) const; // 查询节点是否存在，存在返回true
        bool exist_edge(const std::string& vertex_x_name, const std::string& vertex_y_name) const; // 查询边是否存在，存在返回true
        explicit LGraph(bool directed = false);
        const int& VertexCount() const { return n_verts; } // 获取顶点数
        int VertextCount() const { return n_verts; } // 非const版本
        const int& EdgesCount() const { return m_edges; } // 获取边数
        const std::vector<HeadNode>& List() const { return ver_list; } // 获取邻接表
        std::vector<HeadNode>& List() { return ver_list; } // 非const版本
        const std::map<std::string, Vertex>& Map() const { return vertex_map; } //获取映射表

        bool IsDirected() const { return directed; }
        std::vector<VertInfo> GetVerticesByType(const std::string& type) const; // 新增功能：根据定点类型查询所有顶点
        std::vector<VertInfo> GetVerticesByTypePattern(const std::string& pattern) const;
        std::vector<EdgeNode> GetConnectedEdges(const std::string& name) const; //新增功能：获取与指定顶点邻接的边
        bool IsDeleted(Vertex v) const {
            const VertInfo& info = ver_list[v].data;
            return deleted.find(info.name) != deleted.end();
        }
        int ValidVertexCount() const {
            return n_verts - deleted.size();
        }

        void InsertVertex(const LocationInfo& vertex_info); // 插入点
        void DeleteVertex(const LocationInfo& vertex_info); // 删除点，同时删除相邻边
        void UpdateVertex(const LocationInfo& old_info, LocationInfo& new_info); //修改点信息
        VertInfo GetVertex(const std::string& name) const; // 通过地点名 查询点
        VertInfo GetVertex(Vertex vertex) const; // 通过点ID查询点

        void InsertEdge(const std::string& vertex_x_name, const std::string& vertex_y_name, GElemSet weight);// 插入边
        void DeleteEdge(const std::string& vertex_x_name, const std::string& vertex_y_name); // 删除边
        void DeleteEdge(Vertex vertex_x, Vertex vertex_y); // 删除边
        void UpdateEdge(const std::string& vertex_x_name, const std::string& vertex_y_name, GElemSet new_weight); // 修改边
        GElemSet GetEdge(const std::string& vertex_x_name, const std::string& vertex_y_name) const; // 查询边
        std::vector<EdgeNode> SortedEdges(
            std::function<bool(const GElemSet&, const GElemSet&)> cmp = std::less<>()
        ) const; // 返回边权按值排序的结果(从小到大)
    };
}
#endif  // LGRAPH_LGRAPH_H
