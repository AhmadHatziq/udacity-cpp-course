#include "route_model.h"
#include <iostream>

// Class constructor 
RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    // Create RouteModel nodes.
    int counter = 0;
    for (Model::Node node : this->Nodes()) {
        m_Nodes.emplace_back(Node(counter, this, node));
        counter++;
    }
    CreateNodeToRoadHashmap();
}


void RouteModel::CreateNodeToRoadHashmap() {
    for (const Model::Road &road : Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : Ways()[road.way].nodes) {
                if (node_to_road.find(node_idx) == node_to_road.end()) {
                    node_to_road[node_idx] = std::vector<const Model::Road *> ();
                }
                node_to_road[node_idx].push_back(&road);
            }
        }
    }
}


RouteModel::Node *RouteModel::Node::FindNeighbor(std::vector<int> node_indices) {
    Node *closest_node = nullptr;
    Node node;

    for (int node_index : node_indices) {
        node = parent_model->SNodes()[node_index];
        if (this->distance(node) != 0 && !node.visited) {
            if (closest_node == nullptr || this->distance(node) < this->distance(*closest_node)) {
                closest_node = &parent_model->SNodes()[node_index];
            }
        }
    }
    return closest_node;
}

// This method is for a Node object. 
// Populate the neighbors vector for the current node ie std::vector<Node *> neighbors. See route_model.h. 
void RouteModel::Node::FindNeighbors() {
    for (auto & road : parent_model->node_to_road[this->index]) {
        RouteModel::Node *new_neighbor = this->FindNeighbor(parent_model->Ways()[road->way].nodes);

        // If a valid neighbor node is found, add it to the neighbors vector
        // neighbors is defined in: std::vector<Node *> neighbors;
        if (new_neighbor) {
            this->neighbors.emplace_back(new_neighbor);
        }
    }
}


RouteModel::Node &RouteModel::FindClosestNode(float x, float y) {
    /*
        Function name: FindClosestNode. Is a member function defined in a RouteModel
        Find the closest non-footway node to the given x and y coordinates. 
        Returns a reference to a Node ie RouteModel::Node&
    */
    Node input;
    input.x = x;
    input.y = y;

    float min_dist = std::numeric_limits<float>::max(); // Initialize to max float value 
    float dist;
    int closest_idx; 

    for (const Model::Road &road : Roads()) {           // Enhanced for loop, start iterating in Roads, using 'road' as single value iterator
        if (road.type != Model::Road::Type::Footway) {  // Ignore footways 
            for (int node_idx : Ways()[road.way].nodes) { // Iterate over node indices in the road
                dist = input.distance(SNodes()[node_idx]); // Calculate distance between our input (x,y) node and the iterated node 
                if (dist < min_dist) {
                    closest_idx = node_idx;               // Store closest node index and min distance 
                    min_dist = dist;
                }
            }
        }
    }

    return SNodes()[closest_idx];
}