#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <string>

class GraphNode; // forward declaration
class ChatLogic; // forward declaration

class ChatBot
{
private:
    // data handles (not owned)
    GraphNode *_currentNode;
    GraphNode *_rootNode;
    ChatLogic *_chatLogic;

    // proprietary functions
    int ComputeLevenshteinDistance(std::string s1, std::string s2);

public:
    // constructors / destructors
    ChatBot();                     // constructor
    ChatBot(std::string filename); // constructor (filename is ignored in CLI)
    ~ChatBot();

    // TODO the following:
        // TODO: add copy constructor
        // TODO: add copy assignment operator
        // TODO: add move constructor
        // TODO: add move assignment operator
    // END OF TODO

    // getters / setters
    void SetCurrentNode(GraphNode *node);
    void SetRootNode(GraphNode *rootNode) { _rootNode = rootNode; }
    void SetChatLogicHandle(ChatLogic *chatLogic) { _chatLogic = chatLogic; }

    // communication
    void ReceiveMessageFromUser(std::string message);
};

#endif /* CHATBOT_H_ */