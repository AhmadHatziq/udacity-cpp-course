#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Inputs are obtained from user input ie 0 - 100 
    // Convert inputs to percentage:
    start_x *= 0.01; // Means start_x = start_x * 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    RouteModel::Node& node_closest_to_start = model.FindClosestNode(start_x, start_y); // Returns a reference to a node ie 'Node&'
    RouteModel::Node& node_closest_to_end = model.FindClosestNode(end_x, end_y);

    // Store nodes in Routelanner's attributes. 
    start_node = &node_closest_to_start; 
    end_node = &node_closest_to_end;
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.
float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    /*
        Calculates H Value as the distance of argument node to the end node 
    */
    float distance_to_end_node = node->distance(*end_node); 
    return distance_to_end_node; 
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.
void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    /*
        Expands current_node by adding all unvisited neighbors to the open list 
    */

    // Populate current_node.neighbors vector with all the neighbors
    current_node ->FindNeighbors(); 

    // Iterate over each neighbor in current_node.neighbors and set parent, h_value, g_value
    for (auto neighbor_node_ptr : current_node->neighbors) {
        // If neighbour has been visited, skip it
        if (neighbor_node_ptr -> visited) {
            continue;
        }

        // Set the parent attribute to current_node
        neighbor_node_ptr->parent = current_node;

        // Set the h_value attribute using CalculateHValue method
        neighbor_node_ptr->h_value = CalculateHValue(neighbor_node_ptr);

        // Set the g_value attribute = current_node's g_value + distance from current_node to neighbor
        neighbor_node_ptr->g_value = current_node->g_value + current_node->distance(*neighbor_node_ptr);

        // Set node's visited attribute to true
        neighbor_node_ptr->visited = true;

        // Add neighbor to open_list
        open_list.emplace_back(neighbor_node_ptr);

    }
}

// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.
RouteModel::Node *RoutePlanner::NextNode() {

    // Check if open_list is empty
    if (open_list.empty()) return nullptr;

    // Sort open_list according to the sum of h and g values
    // Use lambda function for custom sort criteria
    std::sort(open_list.begin(), open_list.end(), [](const RouteModel::Node* node1, const RouteModel::Node* node2) {
        float f1 = node1->h_value + node1->g_value; // f = g + h
        float f2 = node2->h_value + node2->g_value;
        return f1 < f2; // Sort in ascending order
    });

    // Get pointer to node with lowest sum (first element after sorting)
    RouteModel::Node* lowest_sum_node = open_list.front();

    // Remove that node from open_list
    open_list.erase(open_list.begin());
    
    // Return the pointer
    return lowest_sum_node;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.
// Argument node is the final node. 
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
    RouteModel::Node* node_ptr = current_node;
    while (node_ptr != nullptr) { // Loop until we reach the start node which has no parent. Safer than checking for equality with start_node. 
        path_found.emplace_back(*node_ptr); // Add the node to path_found vector

        // If the node has a parent, add the distance to the total distance
        if (node_ptr->parent != nullptr) {
            distance += node_ptr->distance(*(node_ptr->parent));
        }

        // Move to the parent node. Only start node has parent == nullptr
        node_ptr = node_ptr->parent;
    }

    // Reverse the path_found vector to have the start node at the beginning
    std::reverse(path_found.begin(), path_found.end());

    // distance is private attribute of RoutePlanner class. 
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;
}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.
void RoutePlanner::AStarSearch() {
    // TODO: Implement your solution here.
    // Implement the while loop and the methods above 

    // Initialize
    RouteModel::Node *current_node = nullptr;
    start_node->visited = true;
    start_node->g_value = 0.0f;
    start_node->h_value = CalculateHValue(start_node);
    open_list.clear();

    // Mark the start node as visited and add it to the open list
    current_node = start_node;
    open_list.emplace_back(start_node);

    // Continue the search until we find the end node or the open list is empty
    while (!open_list.empty()) {
        // Choose next node with lowest f = g + h value
        current_node = NextNode(); // Sorts open_list and returns pointer to node with lowest

        // Check if we reached the goal / end node 
        if (current_node == end_node) {
            m_Model.path = ConstructFinalPath(current_node); // Store the final path
            return;
        }

        // Expand and add neighbors of the current node to the open list
        AddNeighbors(current_node);
    } 
}
