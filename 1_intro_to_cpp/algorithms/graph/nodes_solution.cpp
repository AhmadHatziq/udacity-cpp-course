#include <iostream>
#include <vector>

struct Node {
    int id;
    bool visited = false;
    Node* parent = nullptr;

    void mark() { visited = true; }
};

int main() {
    Node a{0}, b{1}, c{2};

    Node* current = &a; // Current is at node A, with neighbours 
    std::vector<Node*> neighbors = { &b, &c };

    current->mark(); // Current is a node pointer. Call the `mark` to define the current node A as visited 

    for (Node* nb : neighbors) {  // For the neigbbour nodes, mark the parent as node A and mark as visited 
        nb->parent = current;     // This is the traversal. 
        nb->mark();
    }
	
	// Print the graph state 
    for (Node* nb : neighbors) {
        std::cout << "Neighbor id=" << nb->id;

        std::cout << " visited=";
        if (nb->visited) {
            std::cout << "true";
        } else {
            std::cout << "false";
        }

        std::cout << " parent=";
        if (nb->parent != nullptr) {
            std::cout << nb->parent->id;
        } else {
            std::cout << -1;
        }

        std::cout << '\n';
    }
    return 0;
}