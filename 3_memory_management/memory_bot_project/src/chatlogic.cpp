#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>

#include "graphedge.h"
#include "graphnode.h"
#include "chatbot.h"
#include "chatlogic.h"

ChatLogic::ChatLogic()
{
   _chatBot = new ChatBot();
   _chatBot->SetChatLogicHandle(this);
}

ChatLogic::~ChatLogic()
{
   
    delete _chatBot;

    // delete all nodes
    for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    {
        delete *it;
    }
}

template <typename T>
/*    
T is a placeholder for a type, determined by the compiler when you call the function.
Find all "ANSWER" tokens and add their text to a GraphNode.
Find all "KEYWORD" tokens and add their text to a GraphEdge.

E.g. tokenlist vector: { {"TYPE", "EDGE"}, {"ID", "2"}, {"PARENT", "1"}, {"CHILD", "3"},
       {"KEYWORD", "unique pointer"}, {"KEYWORD", "shared pointer"}, {"KEYWORD", "weak pointer"}}
- Process the keyword strings, attach to the node or edge 

Sample usage: 
- AddAllTokensToElement("ANSWER", tokens, **newNode); => **newNode to dereference to node itself 
- AddAllTokensToElement("KEYWORD", tokens, *edge); => *edge to dereference to edge object itself
*/
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element)
{
    // find all occurences for current node
    auto token = tokens.begin();
    while (true)
    {   
        // Find all tokens matching the token ID e.g. all "KEYWORD" or "ANSWER" tokens, add them to the element 
        token = std::find_if(token, tokens.end(), [&tokenID](const std::pair<std::string, std::string> &pair) { return pair.first == tokenID;; });
        if (token != tokens.end())
        {
            // AddToken: Vector string push back to GraphNode _answers.push_back() or GraphEdhe _keywords.push_back(token)
            element.AddToken(token->second); // add new keyword to edge or node
            token++;                         // increment iterator to next element
        }
        else
        {
            break; // quit infinite while-loop
        }
    }
}

/*
Parses .txt file to create nodes and edges, along with keywords & answers 

 */
void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    // load file with answer graph elements
    std::ifstream file(filename);

    // check for file availability and process it line by line
    if (file)
    {
        // loop over all lines in the file
        std::string lineStr;
        while (getline(file, lineStr))
        {
            // extract all tokens from current line
            // Sample line: <TYPE:EDGE><ID:0><PARENT:0><CHILD:1><KEYWORD:pointer><KEYWORD:smart pointer>
            // tokenlist definition fron .h: std::vector<std::pair<std::string, std::string>> tokenlist; 
            tokenlist tokens;
            while (lineStr.size() > 0)
            {
                // extract next token inside arrow brackers e.g. "<token123:abc>" => "token123:abc"
                int posTokenFront = lineStr.find("<");
                int posTokenBack = lineStr.find(">");
                if (posTokenFront < 0 || posTokenBack < 0)
                    break; // quit loop if no complete token has been found
                std::string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - 1);

                // extract token type and info. Format is "<TokenType:TokenInformation>"
                int posTokenInfo = tokenStr.find(":");
                if (posTokenInfo != std::string::npos)
                {
                    std::string tokenType = tokenStr.substr(0, posTokenInfo);
                    std::string tokenInfo = tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

                    // add token to vector
                    // Vector of string pairs e.g. {"TYPE: EDGE"}
                    tokens.push_back(std::make_pair(tokenType, tokenInfo));
                }

                // remove token from current line
                // Process remaining substring e.g. "<TYPE:EDGE><ID:0>" => "<ID:0>"
                lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
            }
            // Singl line has been processed for tokens, all sent to the vector of pairs "tokens"

            // process tokens for current line
            // Find the TYPE first e.g. TYPE:EDGE
            auto type = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "TYPE"; });
            if (type != tokens.end()) // If type exists in "tokens"
            {
                // check for id e.g. ID:0
                auto idToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "ID"; });
                if (idToken != tokens.end()) // If ID exists in "tokens"
                {
                    // extract id from token
                    int id = std::stoi(idToken->second);

                    // node-based processing
                    // E.g. <TYPE:NODE><ID:2><ANSWER:xxx> 
                    if (type->second == "NODE")
                    {
                        // check if node with this ID exists already
                        auto newNode = std::find_if(_nodes.begin(), _nodes.end(), [&id](GraphNode *node) { return node->GetID() == id; }); // TODO

                        // create new element if ID does not yet exist
                        if (newNode == _nodes.end())
                        {
                            _nodes.emplace_back(new GraphNode(id)); // TODO
                            newNode = _nodes.end() - 1; // get iterator to last element

                            // add all answers to current node
                            AddAllTokensToElement("ANSWER", tokens, **newNode);
                        }

                    }

                    // edge-based processing
                    // E.g. <TYPE:EDGE><ID:0><PARENT:0><CHILD:1><KEYWORD:pointer><KEYWORD:smart pointer> 
                    if (type->second == "EDGE")
                    {
                        //// STUDENT CODE
                        ////

                        // find tokens for incoming (parent) and outgoing (child) node
                        // Find the source (PARENT) and destination (CHILD) node-ID tokens.
                        auto parentToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "PARENT"; });
                        auto childToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "CHILD"; });

                        if (parentToken != tokens.end() && childToken != tokens.end())
                        {
                            // get iterator on incoming and outgoing node via ID search
                            // Find iterators to the source and destination nodes using their IDs.
                            // String to integer: std::stoi 
                            auto parentNode = std::find_if(_nodes.begin(), _nodes.end(), [&parentToken](GraphNode *node) { return node->GetID() == std::stoi(parentToken->second); }); // TODO
                            auto childNode = std::find_if(_nodes.begin(), _nodes.end(), [&childToken](GraphNode *node) { return node->GetID() == std::stoi(childToken->second); }); // TODO

                            // TODO: create new edge
                            GraphEdge *edge = new GraphEdge(id);
                            edge->SetChildNode(*childNode);
                            edge->SetParentNode(*parentNode);
                            //_edges.push_back(edge);
                            // END OF TODO

                            // find all keywords for current node
                            AddAllTokensToElement("KEYWORD", tokens, *edge);

                            // TODO: store reference in child node and parent node
                            (*childNode)->AddEdgeToParentNode(edge);  // TODO: add non-owning reference
                            (*parentNode)->AddEdgeToChildNode(edge); // TODO: transfer ownership to parent node
                            // END OF TODO
                        }
                    }
                }
                else
                {
                    std::cout << "Error: ID missing. Line is ignored!" << std::endl;
                }
            }
        } // eof loop over all lines in the file

        file.close();

    } // eof check for file availability
    else
    {
        std::cout << "File could not be opened!" << std::endl;
        return;
    }
    // End of processing txt file 

    //// STUDENT CODE
    ////

    // identify root node. There should only be 1 node with no parents. 
    GraphNode *rootNode = nullptr;
    for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    {
        // search for nodes which have no incoming edges
        if ((*it)->GetNumberOfParents() == 0)
        {

            if (rootNode == nullptr)
            {
                // assign current node to root
                rootNode = *it; // TODO: assign current node to root
            }
            else
            {
                std::cout << "ERROR : Multiple root nodes detected" << std::endl;
            }
        }
    }

    // TODO: add chatbot to graph root node
    _chatBot->SetRootNode(rootNode);
    rootNode->moveChatbotHere(_chatBot);
    // END OF TODO

}

#ifdef GUI_APP  // Skipped as GUI_APP is not defined here.
void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SendMessageToUser(std::string message)
{
    _panelDialog->PrintChatbotResponse(message);
}
#else
void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog) {}
void ChatLogic::SendMessageToUser(std::string message) {}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot) {
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(std::string message) {
    if (_chatBot) {
        _chatBot->ReceiveMessageFromUser(message);
    }
}
#endif

bool ChatLogic::IsGraphLoaded() const {
    return !_nodes.empty();
}
