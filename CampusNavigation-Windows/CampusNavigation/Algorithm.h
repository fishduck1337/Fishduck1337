//
// Created by ����Դ on 24-10-13.
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
            // ���鼯���캯��
            explicit DSU(int n) {
                parent.resize(n);
                rank.resize(n, 1);
                for (int i = 0;i < n; i++) {
                    parent[i] = i;
                }
            }

            // ���Ҹ��ڵ㲢·��ѹ��
            int find(int x) {
                if (parent[x] != x) {
                    parent[x] = find(parent[x]);
                }
                return parent[x];
            }

            // �ϲ���������
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

            // ��������ڵ��Ƿ�����ͬһ����
            bool same(int x, int y) {
                return find(x) == find(y);

            }
        };

        struct HamiltonianResult {
            bool exists = false;
            std::vector<std::string> min_path;
            int min_cost = INT_MAX;
            int search_count = 0;
            std::string status = "";  //״̬��Ϣ
        };

        HamiltonianResult FindHamiltonianCycle(const LGraph& graph, const std::string& start);
        bool IsHamiltonianPossible(const LGraph& graph);   // ���ܶ��ٻ�·���
           
        std::list<Vertex> EulerCircle(const LGraph& graph); //����ŷ����·
        bool HaveEulerCircle(const LGraph& graph); //�ж��Ƿ����ŷ����·
        bool IsConnected(const LGraph& graph); //�ж�ͼ�Ƿ���ͨ
        int GetShortestPath(const LGraph& graph, const std::string& start, const std::string& end, std::list<std::string>& path);
        int TopologicalShortestPath(const LGraph& graph, std::vector<std::string> path); //�����������޵����·��
        std::vector<EdgeNode> MinimumSpanningTree(const LGraph& graph); //������С������
        std::list<Vertex> GetCircuit(LGraph& graph, Vertex start);
        void BFSv(const LGraph& graph, Vertex v, std::vector<bool>& visited);
    }
}
#endif //CAMPUSNAVIGATION_ALGORITHM_H
