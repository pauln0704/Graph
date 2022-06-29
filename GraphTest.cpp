/**
 * Name: Paul Nguyen
 *
 * Contains main function for test cases
 */
#include <iostream>
#include <string>
#include <chrono>
#include <limits>
#include <unordered_set>
#include "Graph.h"

/* Macro to explicity print tests that are run along with colorized result. */
#define TEST(EX) (void)((fprintf(stdout, "(%s:%d) %s:", __FILE__, __LINE__,\
                 #EX)) && ((EX && fprintf(stdout, "\t\x1b[32mPASSED\x1b[0m\n"))\
                 || (fprintf(stdout, "\t\x1b[31mFAILED\x1b[0m\n")) ))

bool compare(const double& d1, const double& d2) {
    return std::abs(d1 - d2) <= std::numeric_limits<double>::epsilon();
}

int main() {
    Graph graph("example/small.csv");
    auto n = graph.nodes();
    TEST(graph.nodes() == unordered_set<string>({"A", "B", "C", "D", "E", "F", "G"}));
    TEST(graph.num_nodes() == 7);
    TEST(graph.edge_weight("A", "B") == 1);
    TEST(graph.edge_weight("A", "F") == -1); // check if edge DNE
    TEST(graph.edge_weight("A", "Z") == -1); // check if a node DNE
    TEST(graph.num_edges() == 6);

    vector<tuple<string, string, int>> result {{"A", "B", 1}, {"B", "C", 1}};
    vector<tuple<string, string, int>> result2 {};
    TEST(graph.shortest_path_weighted("A", "C") == result);
    TEST(graph.shortest_path_weighted("A", "F") == result2); // non-connecting
    TEST(graph.shortest_path_weighted("A", "Z") == result2); // node DNE

    TEST(graph.smallest_connecting_threshold("A", "C") == 1);
    TEST(graph.smallest_connecting_threshold("A", "F") == -1); // non-connecting
    TEST(graph.smallest_connecting_threshold("A", "Z") == -1); // node DNE

    // tests for empty graph
    Graph graph2("example/empty.csv");
    auto n2 = graph2.nodes();
    TEST(graph2.nodes() == unordered_set<string>({}));
    TEST(graph2.num_nodes() == 0);
    TEST(graph2.edge_weight("A", "B") == -1);
    TEST(graph2.num_edges() == 0);
    TEST(graph2.shortest_path_weighted("A", "C") == result2);
    TEST(graph2.smallest_connecting_threshold("A", "C") == -1);

    // tests that nothing crashes for a much larger file
    Graph graph3("example/hiv.csv");
    auto n3 = graph3.nodes();
    graph3.nodes();
    graph3.num_nodes();
    graph3.edge_weight("A", "B");
    graph3.num_edges();
    graph3.shortest_path_weighted("A", "C");
    graph3.smallest_connecting_threshold("A", "C");

}

