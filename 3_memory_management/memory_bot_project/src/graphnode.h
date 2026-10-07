#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include <vector>
#include <string>
#include <memory>
#include "chatbot.h"

// forward declarations
class GraphEdge;

class GraphNode
{
private:

    //// TODO
    
    // Owns outgoing edges from this node to its child nodes.
    // data handles (owned)
    // std::vector<GraphEdge *> _childEdges;  // edges to subsequent nodes
    std::vector<std::unique_ptr<GraphEdge>> _childEdges; // Use unique_ptr 

    // Observes incoming edges from parent nodes to this node. These edges are owned by their respective parent nodes.
    // data handles (not owned)
    std::vector<GraphEdge *> _parentEdges; // edges to preceding nodes 

    // Chatbot pointer 
    // ChatBot *_chatBot;
    
    // Change to ChatBot object 
    ChatBot _chatBot;
   
    //// End of TODO


    // proprietary members
    int _id;
    std::vector<std::string> _answers;

public:
    // constructor / destructor
    GraphNode(int id);
    ~GraphNode();

    // getter / setter
    int GetID() { return _id; }
    int GetNumberOfChildEdges() { return _childEdges.size(); }
    GraphEdge *GetChildEdgeAtIndex(int index);
    std::vector<std::string> GetAnswers() { return _answers; }
    int GetNumberOfParents() { return _parentEdges.size(); }

    // proprietary functions
    void AddToken(std::string token); // add answers to list
    void AddEdgeToParentNode(GraphEdge *edge);
    
    void AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge);
    void AddEdgeToChildNode(GraphEdge *edge); // TODO

    // Change from pointer as will use std::move(existingBot)
    void moveChatbotHere(ChatBot newNode); // TODO

    void MoveChatbotToNewNode(GraphNode *newNode);
};

#endif /* GRAPHNODE_H_ */
