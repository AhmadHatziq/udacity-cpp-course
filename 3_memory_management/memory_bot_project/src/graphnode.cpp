#include "chatbot.h"
#include "graphedge.h"
#include "graphnode.h"
#include <utility>

GraphNode::GraphNode(int id)
{
    _id = id;
}

GraphNode::~GraphNode()
{
    // leave as-is
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(token);
}

// Parent edges: incoming edges from parent node to this node. 
// Is not owned. 
void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

// Child edges: outgoing edges from this node to its child nodes.
// Is owned. Need to transfer ownership. 
void GraphNode::AddEdgeToChildNode(GraphEdge* edge)
{
    //_childEdges.push_back(edge); // TODO
    _childEdges.push_back(std::unique_ptr<GraphEdge>(edge));
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge)
{
    // Transfer ownership into this node's outgoing-edge container.
    _childEdges.push_back(std::move(edge));
}

// Store the chatbot pointer and set this node as its current node.
// Used for initial setup (to root node) and chatbot navigation (subsequent operations)
void GraphNode::moveChatbotHere(ChatBot* chatbot) 
{
    _chatBot = chatbot;
    _chatBot->SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->moveChatbotHere(std::move(_chatBot)); // TODO
}

// Given int index, return a non-owning pointer to the outgoing edge 
GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    // return _childEdges[index]; // TODO
    return _childEdges.at(index).get();
}