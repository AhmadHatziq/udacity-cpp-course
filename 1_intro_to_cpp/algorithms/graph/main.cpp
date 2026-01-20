#include <iostream>
#include "graph.h"

int main() {
    Graph g;

    g.addNode("New York");
    g.addNode("Boston");
    g.addNode("Chicago");
    g.addNode("Miami");

    g.addEdge(0, 1); // New York - Boston
    g.addEdge(0, 2); // New York - Chicago
    g.addEdge(1, 3); // Boston - Miami

    std::cout << "--- City Connection Graph ---\n";
    g.printGraph();

    return 0;
}