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
    std::vector<GraphNode *> _nodes; // TODO

    // data handles (not owned)
    GraphNode *_currentNode;
    ChatBot *_chatBot;
    ChatBotPanelDialog *_panelDialog;

    // proprietary type definitions
    typedef std::vector<std::pair<std::string, std::string>> tokenlist;

    // proprietary functions
    template <typename T>
    void AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element);

public:
    // constructor / destructor
    ChatLogic();
    ~ChatLogic();

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