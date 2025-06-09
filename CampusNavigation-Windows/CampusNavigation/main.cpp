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
#define NOMINMAX // 禁用 Windows 的 min/max 宏
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
    std::cout << "\033[2J\033[1;1H"; // ANSI 转义码
#endif
}

/*========================================前言==============================================*/
/*本项目我是用visual studio 2022完成的，想提一嘴就是小规模测试数据提供的表格是xlsx格式，
我得手动转换为csv后缀的文件才行，否则不是纯文本。。。此外，例如star_100测试项目中提供的
表格数据信息均为中文，由于是UTF-8编码的文件，在命令行窗口中输出的信息全是乱码，所以我先
在vscode中打开这些csv文件，右下角改变编码格式为GB2312(简体中文)后保存再搬运回来就可以正
常显示中文了(在visual studio 2022的命令行窗口中）。*/

constexpr int PAGE_SIZE = 5;
using Graph::LGraph;
using Graph::VertInfo;
using namespace Graph::Algorithm;

// Hint: 如果不慎写坏项目附带的两个文件，可以在backup文件夹中获取原始文件
// TODO: 请根据可执行文件和csv的位置填写文件路径
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
    LGraph graph(false); //初始化一个无向图。
    init(graph);
    while (true) {
        int choice{ 0 };
        std::cout << "欢迎使用校园导航系统！" << std::endl
            << "请选择您要进行的操作：" << std::endl
            << "1.顶点相关操作" << std::endl
            << "2.边相关操作" << std::endl
            << "3.从文件中重新加载点与边" << std::endl
            << "4.是否存在欧拉通路" << std::endl
            << "5.求任意两点间的最短距离" << std::endl
            << "6.求最小生成树" << std::endl
            << "7.求解拓扑受限时的最短路径" << std::endl
            << "8.推荐环形游览路径（游览时间最短）（建议在测试数据规模较小时使用）" << std::endl
            << "9.退出程序" << std::endl
            << "请输入操作前的数字：";
        std::cin >> choice;
        if (choice == 1) { // 顶点相关操作
            std::cout << "顶点相关操作：" << std::endl
                << "1.输出特定顶点信息" << std::endl
                << "2.输出所有顶点信息" << std::endl
                << "3.添加一个顶点" << std::endl
                << "4.删除一个顶点" << std::endl
                << "5.存储顶点到文件中" << std::endl
                << "NEW* 6.按类型查询顶点" << std::endl // 根据小规模数据中的测试流程新增的功能！
                << "NEW* 7.查询顶点邻接边" <<std::endl // 根据小规模数据中的测试流程新增的功能！
                << "8.回到上一级菜单" << std::endl
                << "请输入操作前的数字：";
            std::cin >> choice;
            if (choice == 1) {
                std::string name;
                std::cout << "请输入顶点名称：";
                std::cin >> name;
                try {
                    VertInfo v{ graph.GetVertex(name) };
                    std::cout << "顶点名称：" << v.name << std::endl
                        << "顶点类型：" << v.type << std::endl
                        << "建议游览时间：" << v.visitTime << std::endl;

                }
                catch (Graph::GraphException& e) {
                    std::cout << "名称为 " << name << " 的顶点不存在" << std::endl;
                }
            }
            else if (choice == 2) {
                std::cout << "顶点名称, 顶点类型, 建议游览时间：" << std::endl;
                show_all_nodes(graph);
            }
            else if (choice == 3) {
                std::cout << "请输入顶点名称，顶点类型，建议游览时间(空格分隔)：";
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
                std::cout << "请输入顶点名称：";
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
                std::cout << "正在将顶点存储到testing_nodes.csv中..." << std::endl;
                store_nodes(nodes_path, graph);
                std::cout << "存储成功！" << std::endl;
            }
            else if (choice == 6) {
                std::string type;
                std::cout << "请输入要查询的定点类型：";
                std::cin >> type;

                auto vertices = graph.GetVerticesByType(type);
                if (vertices.empty()) {
                    std::cout << "未找到类型为 '" << type << "' 的顶点" << std::endl;
                }
                else {
                    std::cout << "类型为 '" << type << "' 的顶点列表:" << std::endl;
                    for (const auto& v : vertices) {
                        std::cout << v.name << ", " << v.type << ", " << v.visitTime << std::endl;
                    }
                }
            }
            else if (choice == 7) { // 查询邻接边（分页版）
                std::string name;
                std::cout << "请输入顶点名称: ";
                std::cin >> name;

                try {
                    auto edges = graph.GetConnectedEdges(name);
                    if (edges.empty()) {
                        std::cout << "顶点 '" << name << "' 无邻接边" << std::endl;
                        continue;
                    }

                    // 分页参数配置
                    const int page_size = 5; // 每页显示5条
                    int total_pages = (edges.size() + page_size - 1) / page_size;
                    int current_page = 0;
                    std::string input;

                    do {
                        // 清屏（Windows系统）
                        system("cls");

                        // 显示当前页
                        std::cout << "===== 第 " << current_page + 1 << "/" << total_pages << " 页 =====" << std::endl;
                        for (int i = 0; i < page_size; ++i) {
                            int idx = current_page * page_size + i;
                            if (idx >= edges.size()) break;

                            const auto& edge = edges[idx];
                            const std::string from = graph.GetVertex(edge.from).name;
                            const std::string to = graph.GetVertex(edge.dest).name;
                            std::cout << "[" << idx + 1 << "] " << from << " -> " << to
                                << " 距离: " << edge.weight << std::endl;
                        }

                        // 操作提示
                        std::cout << "\n输入 'n' 下一页, 'p' 上一页, 'q' 退出: ";
                        std::cin >> input;

                        // 处理翻页
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
                            std::cout << "无效输入，请重试" << std::endl;
                            std::cin.ignore((std::numeric_limits<std::streamsize>::max()), '\n');
                        }
                    } while (true);

                }
                catch (const std::out_of_range&) {
                    std::cout << "顶点 '" << name << "' 不存在" << std::endl;
                }
            }
            else {
                break;
            }
        }
        else if (choice == 2) { // 边相关操作
            std::cout << "边相关操作：" << std::endl
                << "1.输出特定边信息" << std::endl
                << "2.输出所有边信息" << std::endl
                << "3.添加一条边" << std::endl
                << "4.删除一条边" << std::endl
                << "5.存储边到文件中" << std::endl
                << "NEW* 6.修改边的权重" << std::endl
                << "7.回到上一级菜单" << std::endl
                << "请输入操作前的数字：";
            std::cin >> choice;
            if (choice == 1) {
                std::string from, to;
                std::cout << "请输入边的起点与终点(空格分隔)：";
                std::cin >> from >> to;
                try {
                    Graph::GElemSet w{ graph.GetEdge(from, to) };
                    std::cout << from << " <---> " << to << " 距离为：" << w << std::endl;
                }
                catch (Graph::GraphException& e) {
                    std::cout << from << " <---> " << to << " 不存在！" << std::endl;
                }
            }
            else if (choice == 2) {
                std::cout << "起点, 终点, 距离：" << std::endl;
                show_all_edges(graph);
            }
            else if (choice == 3) {
                /*std::cout << "请输入起点名称，终点名称，距离(空格分隔)：";
                std::string from, to;
                int length;
                std::cin >> from >> to >> length;
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请输入整数权重！" << std::endl;
                    continue;
                }
                graph.InsertEdge(from, to, length);
                std::cout << "边已成功添加！" << std::endl; */
                std::cout << "请输入起点名称，终点名称，距离(以空格分隔，如：思群堂 体育健康楼 5): ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除缓冲区
                std::string line;
                std::getline(std::cin, line);
                std::istringstream sin(line);
                std::string from, to;
                int length;
                if (sin >> from >> to >> length) {
                    graph.InsertEdge(from, to, length);
                    store_edges("star_100_edges.csv", graph); // 立即保存
                    std::cout << "边已成功添加！" << std::endl;
                }
                else {
                    std::cout << "输入格式错误！" << std::endl;
                }
            }
            else if (choice == 4) {
                std::cout << "请输入起点名称，终点名称(空格分隔)：";
                std::string from, to;
                std::cin >> from >> to;
                graph.DeleteEdge(from, to); // 删除一条边
                store_edges(edges_path, graph);
            }
            else if (choice == 5) {
                std::cout << "正在将边存储到testing_edges.csv文件中..." << std::endl;
                store_edges(edges_path, graph);
                std::cout << "存储成功！" << std::endl;
            }
            else if (choice == 6) {
                std::string x, y;
                int new_weight;
                std::cout << "输入起点、终点和新权重：";
                std::cin >> x >> y >> new_weight;

                try {
                    graph.UpdateEdge(x, y, new_weight);
                    std::cout << "权重已更新！\n";
                    store_edges(edges_path, graph);
                }
                catch (const Graph::GraphException& e) {
                    std::cout << "错误: " << e.what() << std::endl;
                }
            }
        }
        else if (choice == 3) {// 从文件中重新加载点与边
            init(graph);
        }
        else if (choice == 4) { // 是否存在欧拉通路
            auto res = HaveEulerCircle(graph);
            std::cout << (res ? "存在欧拉回路" : "不存在欧拉回路") << std::endl;
        }
        else if (choice == 5) { // 求任意两点间的最短距离
            std::cout << "请输入两个地点，使用空格分开：" << std::endl;
            std::string x, y;
            std::cin >> x >> y;
            try {
                std::list<std::string> path;
                int cost = GetShortestPath(graph, x, y, path);

                if (cost != 1) {
                    std::cout << "路径为：";
                    for (auto it = path.begin(); it != path.end(); ++it) {
                        if (it != path.begin()) std::cout << " -> ";
                        std::cout << *it;
                    }
                    std::cout << "\n最短距离为：" << cost << std::endl;
                }
                else {
                    std::cout << "无法到达/路径不存在" << std::endl;
                }
            }
            catch (Graph::GraphException& e) {
                std::cout << "你找到了虚空的距离" << "，错误： " << e.what() << std::endl;
            }
        }
        else if (choice == 6) { // 求最小生成树
            if (IsConnected(graph)) {
                std::cout << "最小生成树的点如下：";
                auto res = MinimumSpanningTree(graph);
                int sum = 0;
                for (auto item : res) {
                    const auto& e = edge{ graph.List()[item.from].data.name, graph.List()[item.dest].data.name, item.weight };
                    std::cout << e.from << "," << e.to << "," << e.length << std::endl;
                    sum += e.length;
                }
                std::cout << "总权重为" << sum << std::endl;
            }
            else {
                std::cout << "图不连通" << std::endl;
            }
        }
        else if (choice == 7) { // 求解拓扑受限时的最短路径
            std::cout << "请输入您希望的拓扑序，第一行一个n为序列长度，第二行n个地点为拓扑序列" << std::endl;
            int n;
            std::vector<std::string> list;
            std::cin >> n;
            while (n--) {
                std::string x;
                std::cin >> x;
                list.push_back(x);
            }
            std::cout << "最短路径为" << TopologicalShortestPath(graph, list) << std::endl;
        }
        else if (choice == 8) {
            std::cout << "请输入起点：";
            std::string start;
            std::cin >> start;

            try {
                auto result = FindHamiltonianCycle(graph, start);
                if (result.exists) {
                    std::cout << "找到最小代价环形路径（搜索路径数："
                        << result.search_count << "）\n";
                    std::cout << "路径：";
                    for (size_t i = 0; i < result.min_path.size(); ++i) {
                        if (i > 0) std::cout << " -> ";
                        std::cout << result.min_path[i];
                    }
                    std::cout << "\n总建议游览时间：" << result.min_cost << "分钟\n";
                }
                else {
                    std::cout << "不存在满足条件的环形路径\n";
                }
            }
            catch (const Graph::GraphException& e) {
                std::cout << "错误：" << e.what() << std::endl;
            }
            }
        else {
            std::cout << "感谢您的使用，再见！\n 开发者：\\lky1433223/ \\Voltline/ \\10245102437 王韩/" << std::endl;
            break;
        }
    }
    return 0;
}

/* 从文件中读取顶点信息
 * @param nodes_path: 顶点文件路径
 * @return: 包含读取到的所有顶点的std::vector
 */
std::vector<VertInfo> read_nodes(const std::string& nodes_path) {
    std::ifstream fin(nodes_path);              // 创建文件输入流对象
    assert(fin.good());                         // 断言，通过文件输入流的good()方法检查文件是否存在
    std::vector<VertInfo> nodes;                // 临时存储顶点信息
    std::string line;                           // 通过std::getline读取到的每一行，临时存储在字符串line中

    std::getline(fin, line);                   //  跳过第一行，即name,type,weight，下面read_edges同理

    while (std::getline(fin, line)) {       // 通过std::getline(std::istream&, std::string&)来获取输入流的一行
        for (auto& c : line) {
            if (c == ',') c = ' ';              // csv文件以逗号分隔，为了方便后续使用字符串输入流，将逗号替换为空格
        }
        std::istringstream sin(line);           // 利用std::istringstream，以获取的行为内容构建一个字符串输入流
        std::string name, type;                 // 文件每行中的顶点名，顶点类型
        int visit_time;                         // 文件中每行的顶点推荐游览时间
        sin >> name >> type >> visit_time;      // 利用已构造的字符串输入流，获取对应的几个参数
        nodes.push_back({ name, visit_time, type });  // 放入需要返回的std::vector中
    }
    fin.close();                                // 文件使用完毕后，请务必使用close访问关闭输入流，避免文件损坏
    return nodes;
}

/* 从文件中读取边信息
 * @param edges_path: 边文件路径
 * @return: 包含读取到的所有边的std::vector
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

    // 跳过标题行
    std::getline(fin, line);

    while (std::getline(fin, line)) {
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream sin(line);
        std::string from, to;
        int weight;
        if (sin >> from >> to >> weight) { // 确保读取三列数据
            edges.push_back({ from, to, weight });
        }
        else {
            std::cerr << "警告: 忽略无效行: " << line << std::endl;
        }
    }
    return edges;
}

void init(LGraph& graph) {
    std::vector<VertInfo> nodes{ read_nodes(nodes_path) };
    std::vector<edge> edges{ read_edges(edges_path) };

    // 插入顶点
    for (auto& v : nodes) {
        try {
            graph.InsertVertex(v);
        }
        catch (Graph::GraphException& e) {
            std::cerr << "插入顶点失败: " << e.what() << std::endl;
        }
    }

    // 插入边
    for (auto& e : edges) {
        try {
            graph.InsertEdge(e.from, e.to, e.length);
        }
        catch (Graph::GraphException& e) {
            std::cerr << "插入边失败: " << e.what() << std::endl;
        }
    }
}

/* 打印目前的所有顶点
 * @param graph: 图
 */
void show_all_nodes(const LGraph& graph) {
    const auto& list = graph.List();
    for (const auto& head : list) {
        const VertInfo& v = head.data;
        // 检查顶点是否有效（未被删除）
        if (!v.name.empty()) {
            std::cout << v.name << ", " << v.type << ", " << v.visitTime << std::endl;
        }
    }
}

/* 打印目前的所有边
 * @param graph: 图
 */
void show_all_edges(const LGraph& graph) {
    // LGraph类只提供了获取邻接表和获取经过排序的边的方法
    // 为了能够获取到每条边的元信息，我们需要配合两组数据来完成信息的获取
    const auto& graph_edges{ graph.SortedEdges() };       // 获取经过排序的边
    const auto& graph_list{ graph.List() };               // 获取邻接表
    for (const auto& en : graph_edges) {                 // 遍历边，通过下标来获取对应的顶点信息
        const auto& e = edge{ graph_list[en.from].data.name, graph_list[en.dest].data.name, en.weight };
        std::cout << e.from << "," << e.to << "," << e.length << std::endl;
    }
}

/* 将顶点信息存储回文件中
 * @param nodes_path: 顶点文件路径
 * @param graph: 图
 */
void store_nodes(const std::string& nodes_path, const LGraph& graph) {
    std::ofstream fout(nodes_path);                                         // 通过目录创建文件输出流
    const auto& graph_list{ graph.List() };                                   // 通过LGraph对象的List()方法获得邻接表
    for (const auto& n : graph_list) {                                       // 遍历邻接表中的每个顶点
        const auto& v = n.data;                                             // 获得顶点中的元数据
        fout << v.name << "," << v.type << "," << v.visitTime << std::endl; // 按照顶点存储的格式，重新输入到文件中
    }
    fout.close();                                                           // 在使用完任何文件流后，都要使用close方法关闭
}

/* 将边信息存储回文件中
 * @param edges_path: 边文件路径
 * @param graph: 图
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