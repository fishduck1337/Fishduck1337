//
// Created by 刘凯源 on 24-10-13.
//

#ifndef CAMPUSNAVIGATION_ALGORITHM_H
#define CAMPUSNAVIGATION_ALGORITHM_H

#include "LGraph.h"
#include <chrono>

namespace Graph {
    namespace Algorithm {
        class DSU {
        private:
            std::vector<int> parent, rank;
        public:
            // 并查集构造函数
            explicit DSU(int n) {
                parent.resize(n);
                rank.resize(n, 1);
                for (int i = 0;i < n; i++) {
                    parent[i] = i;
                }
            }

            // 查找根节点并路径压缩
            int find(int x) {
                if (parent[x] != x) {
                    parent[x] = find(parent[x]);
                }
                return parent[x];
            }

            // 合并两个集合
            void unite(int x, int y) {
                int x_root = find(x);
                int y_root = find(y);

                if (x_root == y_root) return;

                if (rank[x_root] < rank[y_root]) {
                    parent[x_root] = y_root;
                }
                else {
                    parent[y_root] = x_root;
                    if (rank[x_root] == rank[y_root]) {
                        rank[x_root]++;
                    }
                }
            }

            // 检查两个节点是否属于同一集合
            bool same(int x, int y) {
                return find(x) == find(y);

            }
        };

        struct HamiltonianResult {
            bool exists = false;
            std::vector<std::string> min_path;
            int min_cost = INT_MAX;
            int search_count = 0;
            std::string status = "";  //状态信息
        };

        HamiltonianResult FindHamiltonianCycle(const LGraph& graph, const std::string& start);
        bool IsHamiltonianPossible(const LGraph& graph);   // 哈密尔顿回路检测
           
        std::list<Vertex> EulerCircle(const LGraph& graph); //计算欧拉回路
        bool HaveEulerCircle(const LGraph& graph); //判断是否存在欧拉回路
        bool IsConnected(const LGraph& graph); //判断图是否联通
        int GetShortestPath(const LGraph& graph, const std::string& start, const std::string& end, std::list<std::string>& path);
        int TopologicalShortestPath(const LGraph& graph, std::vector<std::string> path); //计算拓扑受限的最短路径
        std::vector<EdgeNode> MinimumSpanningTree(const LGraph& graph); //计算最小生成树
        std::list<Vertex> GetCircuit(LGraph& graph, Vertex start);
        void BFSv(const LGraph& graph, Vertex v, std::vector<bool>& visited);
    }
}
#endif //CAMPUSNAVIGATION_ALGORITHM_H
