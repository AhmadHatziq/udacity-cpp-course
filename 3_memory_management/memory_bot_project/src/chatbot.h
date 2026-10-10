#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <string>
#include <memory>

class GraphNode; // forward declaration
class ChatLogic; // forward declaration

class ChatBot
{
private:
    // data handles (not owned)
    GraphNode *_currentNode = nullptr;
    GraphNode *_rootNode = nullptr;
    
    // ChatLogic *_chatLogic;
    // Owned forwarding logic object 
    std::unique_ptr<ChatLogic> _chatLogic; 

    // proprietary functions
    int ComputeLevenshteinDistance(std::string s1, std::string s2);

public:
    // constructors / destructors
    ChatBot();                     // constructor
    ChatBot(std::string filename); // constructor (filename is ignored in CLI)
    ~ChatBot();

    // DONE the following:
    // DONE: add copy constructor
    // Copy constructor - Creaes a new object from an existing object 
    ChatBot(const ChatBot &other);

    // DONE: add copy assignment operator
    // Copy assignment - Replaces contents of an existing obj with another existing obj 
    ChatBot &operator=(const ChatBot &other);

    // DONE: add move constructor
    // Move constructor - Creates new obj by stealing resources from another object 
    ChatBot(ChatBot &&other) noexcept;

    // DONE: add move assignment operator
    // Move assignment - Transfers resources into an already existing object 
    ChatBot &operator=(ChatBot &&other) noexcept;
    // END OF T̶O̶D̶O̶

    // getters / setters
    void SetCurrentNode(GraphNode *node);
    void SetRootNode(GraphNode *rootNode) { _rootNode = rootNode; }
    void SetChatLogicHandle(std::unique_ptr<ChatLogic> chatLogic);

    // communication
    void ReceiveMessageFromUser(std::string message);
};

#endif /* CHATBOT_H_ */