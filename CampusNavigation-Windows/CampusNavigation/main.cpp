#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include "LGraph.h"
#include "Algorithm.h"
#include "LocationInfo.h"
#include "GraphException.h"
#include <limits>
#ifdef _WIN32
#define NOMINMAX // ���� Windows �� min/max ��
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <codecvt>
#include <locale>

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H"; // ANSI ת����
#endif
}

/*========================================ǰ��==============================================*/
/*����Ŀ������visual studio 2022��ɵģ�����һ�����С��ģ���������ṩ�ı����xlsx��ʽ��
�ҵ��ֶ�ת��Ϊcsv��׺���ļ����У������Ǵ��ı����������⣬����star_100������Ŀ���ṩ��
���������Ϣ��Ϊ���ģ�������UTF-8������ļ����������д������������Ϣȫ�����룬��������
��vscode�д���Щcsv�ļ������½Ǹı�����ʽΪGB2312(��������)�󱣴��ٰ��˻����Ϳ�����
����ʾ������(��visual studio 2022�������д����У���*/

constexpr int PAGE_SIZE = 5;
using Graph::LGraph;
using Graph::VertInfo;
using namespace Graph::Algorithm;

// Hint: �������д����Ŀ�����������ļ���������backup�ļ����л�ȡԭʼ�ļ�
// TODO: ����ݿ�ִ���ļ���csv��λ����д�ļ�·��
const std::string nodes_path{ "random_sparse_10000_nodes.csv" };
const std::string edges_path{ "random_sparse_10000_edges.csv" };

struct edge {
    std::string from;
    std::string to;
    int length;
};

void init(LGraph& graph);

std::vector<VertInfo> read_nodes(const std::string& nodes_path);

std::vector<edge> read_edges(const std::string& edges_path);

void store_nodes(const std::string& nodes_path, const LGraph& graph);

void store_edges(const std::string& edges_path, const LGraph& graph);

void show_all_nodes(const LGraph& graph);

void show_all_edges(const LGraph& graph);

int main() {
    LGraph graph(false); //��ʼ��һ������ͼ��
    init(graph);
    while (true) {
        int choice{ 0 };
        std::cout << "��ӭʹ��У԰����ϵͳ��" << std::endl
            << "��ѡ����Ҫ���еĲ�����" << std::endl
            << "1.������ز���" << std::endl
            << "2.����ز���" << std::endl
            << "3.���ļ������¼��ص����" << std::endl
            << "4.�Ƿ����ŷ��ͨ·" << std::endl
            << "5.��������������̾���" << std::endl
            << "6.����С������" << std::endl
            << "7.�����������ʱ�����·��" << std::endl
            << "8.�Ƽ���������·��������ʱ����̣��������ڲ������ݹ�ģ��Сʱʹ�ã�" << std::endl
            << "9.�˳�����" << std::endl
            << "���������ǰ�����֣�";
        std::cin >> choice;
        if (choice == 1) { // ������ز���
            std::cout << "������ز�����" << std::endl
                << "1.����ض�������Ϣ" << std::endl
                << "2.������ж�����Ϣ" << std::endl
                << "3.���һ������" << std::endl
                << "4.ɾ��һ������" << std::endl
                << "5.�洢���㵽�ļ���" << std::endl
                << "NEW* 6.�����Ͳ�ѯ����" << std::endl // ����С��ģ�����еĲ������������Ĺ��ܣ�
                << "NEW* 7.��ѯ�����ڽӱ�" <<std::endl // ����С��ģ�����еĲ������������Ĺ��ܣ�
                << "8.�ص���һ���˵�" << std::endl
                << "���������ǰ�����֣�";
            std::cin >> choice;
            if (choice == 1) {
                std::string name;
                std::cout << "�����붥�����ƣ�";
                std::cin >> name;
                try {
                    VertInfo v{ graph.GetVertex(name) };
                    std::cout << "�������ƣ�" << v.name << std::endl
                        << "�������ͣ�" << v.type << std::endl
                        << "��������ʱ�䣺" << v.visitTime << std::endl;

                }
                catch (Graph::GraphException& e) {
                    std::cout << "����Ϊ " << name << " �Ķ��㲻����" << std::endl;
                }
            }
            else if (choice == 2) {
                std::cout << "��������, ��������, ��������ʱ�䣺" << std::endl;
                show_all_nodes(graph);
            }
            else if (choice == 3) {
                std::cout << "�����붥�����ƣ��������ͣ���������ʱ��(�ո�ָ�)��";
                std::string name, type;
                int visit_time;
                std::cin >> name >> type >> visit_time;
                try {
                    graph.InsertVertex({ name, visit_time, type });
                    store_nodes(nodes_path, graph);
                }
                catch (Graph::GraphException& e) {
                    std::cout << e.what() << std::endl;
                }
            }
            else if (choice == 4) {
                std::cout << "�����붥�����ƣ�";
                std::string name;
                std::cin >> name;
                try {
                    graph.DeleteVertex({ name, 0, "" });
                }
                catch (Graph::GraphException& e) {
                    std::cout << e.what() << std::endl;
                }
            }
            else if (choice == 5) {
                std::cout << "���ڽ�����洢��testing_nodes.csv��..." << std::endl;
                store_nodes(nodes_path, graph);
                std::cout << "�洢�ɹ���" << std::endl;
            }
            else if (choice == 6) {
                std::string type;
                std::cout << "������Ҫ��ѯ�Ķ������ͣ�";
                std::cin >> type;

                auto vertices = graph.GetVerticesByType(type);
                if (vertices.empty()) {
                    std::cout << "δ�ҵ�����Ϊ '" << type << "' �Ķ���" << std::endl;
                }
                else {
                    std::cout << "����Ϊ '" << type << "' �Ķ����б�:" << std::endl;
                    for (const auto& v : vertices) {
                        std::cout << v.name << ", " << v.type << ", " << v.visitTime << std::endl;
                    }
                }
            }
            else if (choice == 7) { // ��ѯ�ڽӱߣ���ҳ�棩
                std::string name;
                std::cout << "�����붥������: ";
                std::cin >> name;

                try {
                    auto edges = graph.GetConnectedEdges(name);
                    if (edges.empty()) {
                        std::cout << "���� '" << name << "' ���ڽӱ�" << std::endl;
                        continue;
                    }

                    // ��ҳ��������
                    const int page_size = 5; // ÿҳ��ʾ5��
                    int total_pages = (edges.size() + page_size - 1) / page_size;
                    int current_page = 0;
                    std::string input;

                    do {
                        // ������Windowsϵͳ��
                        system("cls");

                        // ��ʾ��ǰҳ
                        std::cout << "===== �� " << current_page + 1 << "/" << total_pages << " ҳ =====" << std::endl;
                        for (int i = 0; i < page_size; ++i) {
                            int idx = current_page * page_size + i;
                            if (idx >= edges.size()) break;

                            const auto& edge = edges[idx];
                            const std::string from = graph.GetVertex(edge.from).name;
                            const std::string to = graph.GetVertex(edge.dest).name;
                            std::cout << "[" << idx + 1 << "] " << from << " -> " << to
                                << " ����: " << edge.weight << std::endl;
                        }

                        // ������ʾ
                        std::cout << "\n���� 'n' ��һҳ, 'p' ��һҳ, 'q' �˳�: ";
                        std::cin >> input;

                        // ����ҳ
                        if (input == "n" && current_page < total_pages - 1) {
                            current_page++;
                        }
                        else if (input == "p" && current_page > 0) {
                            current_page--;
                        }
                        else if (input == "q") {
                            break;
                        }
                        else {
                            std::cout << "��Ч���룬������" << std::endl;
                            std::cin.ignore((std::numeric_limits<std::streamsize>::max()), '\n');
                        }
                    } while (true);

                }
                catch (const std::out_of_range&) {
                    std::cout << "���� '" << name << "' ������" << std::endl;
                }
            }
            else {
                break;
            }
        }
        else if (choice == 2) { // ����ز���
            std::cout << "����ز�����" << std::endl
                << "1.����ض�����Ϣ" << std::endl
                << "2.������б���Ϣ" << std::endl
                << "3.���һ����" << std::endl
                << "4.ɾ��һ����" << std::endl
                << "5.�洢�ߵ��ļ���" << std::endl
                << "NEW* 6.�޸ıߵ�Ȩ��" << std::endl
                << "7.�ص���һ���˵�" << std::endl
                << "���������ǰ�����֣�";
            std::cin >> choice;
            if (choice == 1) {
                std::string from, to;
                std::cout << "������ߵ�������յ�(�ո�ָ�)��";
                std::cin >> from >> to;
                try {
                    Graph::GElemSet w{ graph.GetEdge(from, to) };
                    std::cout << from << " <---> " << to << " ����Ϊ��" << w << std::endl;
                }
                catch (Graph::GraphException& e) {
                    std::cout << from << " <---> " << to << " �����ڣ�" << std::endl;
                }
            }
            else if (choice == 2) {
                std::cout << "���, �յ�, ���룺" << std::endl;
                show_all_edges(graph);
            }
            else if (choice == 3) {
                /*std::cout << "������������ƣ��յ����ƣ�����(�ո�ָ�)��";
                std::string from, to;
                int length;
                std::cin >> from >> to >> length;
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "��Ч���룬����������Ȩ�أ�" << std::endl;
                    continue;
                }
                graph.InsertEdge(from, to, length);
                std::cout << "���ѳɹ���ӣ�" << std::endl; */
                std::cout << "������������ƣ��յ����ƣ�����(�Կո�ָ����磺˼Ⱥ�� ��������¥ 5): ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ���������
                std::string line;
                std::getline(std::cin, line);
                std::istringstream sin(line);
                std::string from, to;
                int length;
                if (sin >> from >> to >> length) {
                    graph.InsertEdge(from, to, length);
                    store_edges("star_100_edges.csv", graph); // ��������
                    std::cout << "���ѳɹ���ӣ�" << std::endl;
                }
                else {
                    std::cout << "�����ʽ����" << std::endl;
                }
            }
            else if (choice == 4) {
                std::cout << "������������ƣ��յ�����(�ո�ָ�)��";
                std::string from, to;
                std::cin >> from >> to;
                graph.DeleteEdge(from, to); // ɾ��һ����
                store_edges(edges_path, graph);
            }
            else if (choice == 5) {
                std::cout << "���ڽ��ߴ洢��testing_edges.csv�ļ���..." << std::endl;
                store_edges(edges_path, graph);
                std::cout << "�洢�ɹ���" << std::endl;
            }
            else if (choice == 6) {
                std::string x, y;
                int new_weight;
                std::cout << "������㡢�յ����Ȩ�أ�";
                std::cin >> x >> y >> new_weight;

                try {
                    graph.UpdateEdge(x, y, new_weight);
                    std::cout << "Ȩ���Ѹ��£�\n";
                    store_edges(edges_path, graph);
                }
                catch (const Graph::GraphException& e) {
                    std::cout << "����: " << e.what() << std::endl;
                }
            }
        }
        else if (choice == 3) {// ���ļ������¼��ص����
            init(graph);
        }
        else if (choice == 4) { // �Ƿ����ŷ��ͨ·
            auto res = HaveEulerCircle(graph);
            std::cout << (res ? "����ŷ����·" : "������ŷ����·") << std::endl;
        }
        else if (choice == 5) { // ��������������̾���
            std::cout << "�����������ص㣬ʹ�ÿո�ֿ���" << std::endl;
            std::string x, y;
            std::cin >> x >> y;
            try {
                std::list<std::string> path;
                int cost = GetShortestPath(graph, x, y, path);

                if (cost != 1) {
                    std::cout << "·��Ϊ��";
                    for (auto it = path.begin(); it != path.end(); ++it) {
                        if (it != path.begin()) std::cout << " -> ";
                        std::cout << *it;
                    }
                    std::cout << "\n��̾���Ϊ��" << cost << std::endl;
                }
                else {
                    std::cout << "�޷�����/·��������" << std::endl;
                }
            }
            catch (Graph::GraphException& e) {
                std::cout << "���ҵ�����յľ���" << "������ " << e.what() << std::endl;
            }
        }
        else if (choice == 6) { // ����С������
            if (IsConnected(graph)) {
                std::cout << "��С�������ĵ����£�";
                auto res = MinimumSpanningTree(graph);
                int sum = 0;
                for (auto item : res) {
                    const auto& e = edge{ graph.List()[item.from].data.name, graph.List()[item.dest].data.name, item.weight };
                    std::cout << e.from << "," << e.to << "," << e.length << std::endl;
                    sum += e.length;
                }
                std::cout << "��Ȩ��Ϊ" << sum << std::endl;
            }
            else {
                std::cout << "ͼ����ͨ" << std::endl;
            }
        }
        else if (choice == 7) { // �����������ʱ�����·��
            std::cout << "��������ϣ���������򣬵�һ��һ��nΪ���г��ȣ��ڶ���n���ص�Ϊ��������" << std::endl;
            int n;
            std::vector<std::string> list;
            std::cin >> n;
            while (n--) {
                std::string x;
                std::cin >> x;
                list.push_back(x);
            }
            std::cout << "���·��Ϊ" << TopologicalShortestPath(graph, list) << std::endl;
        }
        else if (choice == 8) {
            std::cout << "��������㣺";
            std::string start;
            std::cin >> start;

            try {
                auto result = FindHamiltonianCycle(graph, start);
                if (result.exists) {
                    std::cout << "�ҵ���С���ۻ���·��������·������"
                        << result.search_count << "��\n";
                    std::cout << "·����";
                    for (size_t i = 0; i < result.min_path.size(); ++i) {
                        if (i > 0) std::cout << " -> ";
                        std::cout << result.min_path[i];
                    }
                    std::cout << "\n�ܽ�������ʱ�䣺" << result.min_cost << "����\n";
                }
                else {
                    std::cout << "���������������Ļ���·��\n";
                }
            }
            catch (const Graph::GraphException& e) {
                std::cout << "����" << e.what() << std::endl;
            }
            }
        else {
            std::cout << "��л����ʹ�ã��ټ���\n �����ߣ�\\lky1433223/ \\Voltline/ \\10245102437 ����/" << std::endl;
            break;
        }
    }
    return 0;
}

/* ���ļ��ж�ȡ������Ϣ
 * @param nodes_path: �����ļ�·��
 * @return: ������ȡ�������ж����std::vector
 */
std::vector<VertInfo> read_nodes(const std::string& nodes_path) {
    std::ifstream fin(nodes_path);              // �����ļ�����������
    assert(fin.good());                         // ���ԣ�ͨ���ļ���������good()��������ļ��Ƿ����
    std::vector<VertInfo> nodes;                // ��ʱ�洢������Ϣ
    std::string line;                           // ͨ��std::getline��ȡ����ÿһ�У���ʱ�洢���ַ���line��

    std::getline(fin, line);                   //  ������һ�У���name,type,weight������read_edgesͬ��

    while (std::getline(fin, line)) {       // ͨ��std::getline(std::istream&, std::string&)����ȡ��������һ��
        for (auto& c : line) {
            if (c == ',') c = ' ';              // csv�ļ��Զ��ŷָ���Ϊ�˷������ʹ���ַ������������������滻Ϊ�ո�
        }
        std::istringstream sin(line);           // ����std::istringstream���Ի�ȡ����Ϊ���ݹ���һ���ַ���������
        std::string name, type;                 // �ļ�ÿ���еĶ���������������
        int visit_time;                         // �ļ���ÿ�еĶ����Ƽ�����ʱ��
        sin >> name >> type >> visit_time;      // �����ѹ�����ַ�������������ȡ��Ӧ�ļ�������
        nodes.push_back({ name, visit_time, type });  // ������Ҫ���ص�std::vector��
    }
    fin.close();                                // �ļ�ʹ����Ϻ������ʹ��close���ʹر��������������ļ���
    return nodes;
}

/* ���ļ��ж�ȡ����Ϣ
 * @param edges_path: ���ļ�·��
 * @return: ������ȡ�������бߵ�std::vector
 */
std::vector<edge> read_edges(const std::string& edges_path) {
    /*std::ifstream fin(edges_path);
    assert(fin.good());
    std::vector<edge> edges;
    std::string line;
    while (std::getline(fin, line)) {
        for (auto& c : line) {
            if (c == ',') c = ' ';
        }
        std::istringstream sin(line);
        std::string from, to;
        int length;
        sin >> from >> to >> length;
        edges.push_back({ from, to, length });
    }
    fin.close();
    return edges;*/
    std::vector<edge> edges;
    std::ifstream fin(edges_path);
    std::string line;

    // ����������
    std::getline(fin, line);

    while (std::getline(fin, line)) {
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream sin(line);
        std::string from, to;
        int weight;
        if (sin >> from >> to >> weight) { // ȷ����ȡ��������
            edges.push_back({ from, to, weight });
        }
        else {
            std::cerr << "����: ������Ч��: " << line << std::endl;
        }
    }
    return edges;
}

void init(LGraph& graph) {
    std::vector<VertInfo> nodes{ read_nodes(nodes_path) };
    std::vector<edge> edges{ read_edges(edges_path) };

    // ���붥��
    for (auto& v : nodes) {
        try {
            graph.InsertVertex(v);
        }
        catch (Graph::GraphException& e) {
            std::cerr << "���붥��ʧ��: " << e.what() << std::endl;
        }
    }

    // �����
    for (auto& e : edges) {
        try {
            graph.InsertEdge(e.from, e.to, e.length);
        }
        catch (Graph::GraphException& e) {
            std::cerr << "�����ʧ��: " << e.what() << std::endl;
        }
    }
}

/* ��ӡĿǰ�����ж���
 * @param graph: ͼ
 */
void show_all_nodes(const LGraph& graph) {
    const auto& list = graph.List();
    for (const auto& head : list) {
        const VertInfo& v = head.data;
        // ��鶥���Ƿ���Ч��δ��ɾ����
        if (!v.name.empty()) {
            std::cout << v.name << ", " << v.type << ", " << v.visitTime << std::endl;
        }
    }
}

/* ��ӡĿǰ�����б�
 * @param graph: ͼ
 */
void show_all_edges(const LGraph& graph) {
    // LGraph��ֻ�ṩ�˻�ȡ�ڽӱ�ͻ�ȡ��������ıߵķ���
    // Ϊ���ܹ���ȡ��ÿ���ߵ�Ԫ��Ϣ��������Ҫ������������������Ϣ�Ļ�ȡ
    const auto& graph_edges{ graph.SortedEdges() };       // ��ȡ��������ı�
    const auto& graph_list{ graph.List() };               // ��ȡ�ڽӱ�
    for (const auto& en : graph_edges) {                 // �����ߣ�ͨ���±�����ȡ��Ӧ�Ķ�����Ϣ
        const auto& e = edge{ graph_list[en.from].data.name, graph_list[en.dest].data.name, en.weight };
        std::cout << e.from << "," << e.to << "," << e.length << std::endl;
    }
}

/* ��������Ϣ�洢���ļ���
 * @param nodes_path: �����ļ�·��
 * @param graph: ͼ
 */
void store_nodes(const std::string& nodes_path, const LGraph& graph) {
    std::ofstream fout(nodes_path);                                         // ͨ��Ŀ¼�����ļ������
    const auto& graph_list{ graph.List() };                                   // ͨ��LGraph�����List()��������ڽӱ�
    for (const auto& n : graph_list) {                                       // �����ڽӱ��е�ÿ������
        const auto& v = n.data;                                             // ��ö����е�Ԫ����
        fout << v.name << "," << v.type << "," << v.visitTime << std::endl; // ���ն���洢�ĸ�ʽ���������뵽�ļ���
    }
    fout.close();                                                           // ��ʹ�����κ��ļ����󣬶�Ҫʹ��close�����ر�
}

/* ������Ϣ�洢���ļ���
 * @param edges_path: ���ļ�·��
 * @param graph: ͼ
 */
void store_edges(const std::string& edges_path, const LGraph& graph) {
    std::ofstream fout(edges_path);
    const auto& edges = graph.SortedEdges();
    const auto& list = graph.List();
    for (const auto& en : edges) {
        const std::string from = list[en.from].data.name;
        const std::string to = list[en.dest].data.name;
        fout << from << "," << to << "," << en.weight << std::endl;
    }
    fout.close();
}