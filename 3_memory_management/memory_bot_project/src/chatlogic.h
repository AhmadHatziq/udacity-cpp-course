#ifndef CHATLOGIC_H_
#define CHATLOGIC_H_

#include <vector>
#include <string>
#include <memory>
// #include "chatgui.h"  // Removed for CLI decoupling

// forward declarations
class ChatBot;
class GraphEdge;
class GraphNode;
class ChatBotPanelDialog; // Forward declaration for GUI compatibility (deprecated)

class ChatLogic
{
private:
    // data handles (owned)
    // std::vector<GraphNode *> _nodes; // TODO
    std::vector<std::unique_ptr<GraphNode>> _nodes; // Use smart pointers instead of raw pointers 

    // data handles (not owned)
    GraphNode *_currentNode = nullptr;
    ChatBot *_chatBot = nullptr;
    ChatBotPanelDialog *_panelDialog = nullptr;

    // Original graph owning logic
    ChatLogic *_hostLogic = nullptr; 

    // proprietary type definitions
    typedef std::vector<std::pair<std::string, std::string>> tokenlist;

    // proprietary functions
    template <typename T>
    void AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element);

public:
    // constructor / destructor
    ChatLogic();
    // ~ChatLogic();
    explicit ChatLogic(ChatLogic *hostLogic);
    virtual ~ChatLogic();

    std::unique_ptr<ChatLogic> CloneForBot() const;

    // getter / setter
    void SetPanelDialogHandle(ChatBotPanelDialog *panelDialog);
    void SetChatbotHandle(ChatBot *chatbot);

    // proprietary functions
    void LoadAnswerGraphFromFile(std::string filename);
    void SendMessageToChatbot(std::string message);
    virtual void SendMessageToUser(std::string message);
    bool IsGraphLoaded() const;
};

#endif /* CHATLOGIC_H_ */