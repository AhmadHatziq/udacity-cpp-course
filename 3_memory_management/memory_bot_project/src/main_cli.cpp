// CLI-only main for membot. Does not require wxWidgets or GUI code.
#define GUI_APP 0
#include <iostream>
#include <string>
#include "chatlogic.h"

// Simple CLI version of SendMessageToUser
class ChatLogicCLI : public ChatLogic {
public:
    void SendMessageToUser(std::string message) override {
        std::cout << "Bot: " << message << std::endl;
    }
};

int main() {
    ChatLogicCLI chatLogic;
    chatLogic.LoadAnswerGraphFromFile("../src/answergraph.txt");
    // Check if the root node was loaded (basic check for file load success)
    // If the file could not be opened, exit gracefully
    // (Assume that if no nodes are loaded, the file was not found or empty)
    if (!chatLogic.IsGraphLoaded()) {
        std::cerr << "Error: Could not load answer graph. Exiting." << std::endl;
        return 1;
    }
    std::string userInput;
    std::cout << "Type 'quit' to exit at anytime." << std::endl;
    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, userInput);
        if (userInput == "quit") break;
        chatLogic.SendMessageToChatbot(userInput);
    }
    std::cout << "Goodbye!" << std::endl;
    return 0;
} 