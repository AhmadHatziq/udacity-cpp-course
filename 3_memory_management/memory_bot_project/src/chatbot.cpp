#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

// constructor WITHOUT memory allocation
ChatBot::ChatBot()
{
    std::cerr << ">>> Rule of Five Component: ChatBot Default Constructor <<<" << std::endl;
    _chatLogic = nullptr;
    _rootNode = nullptr;
    _currentNode = nullptr; 
}

// T̶O̶D̶O̶ the following: 
// DONE: add copy constructor
// Copy constructor - Creaes a new object from an existing object 
// Used via: ChatBot b(a); 
ChatBot::ChatBot(const ChatBot &other)
    : _currentNode(other._currentNode),
      _rootNode(other._rootNode),
      _chatLogic(other._chatLogic)
{
    std::cerr << ">>> Rule of Five Component: ChatBot Copy Constructor <<<" << std::endl;
}

// DONE: add copy assignment operator
// Copy assignment - Replaces contents of an existing obj with another existing obj 
// Used via b = a; 
ChatBot &ChatBot::operator=(const ChatBot &other)
{
    std::cerr << ">>> Rule of Five Component: ChatBot Copy Assignment <<<" << std::endl;

    if (this != &other)
    {
        _currentNode = other._currentNode;
        _rootNode = other._rootNode;
        _chatLogic = other._chatLogic;
    }

    return *this;
}

// DONE: add move constructor
// Move constructor - Creates new obj by stealing resources from another object 
// Used via: ChatBot b(std::move(a)); 
ChatBot::ChatBot(ChatBot &&other) noexcept
    : _currentNode(other._currentNode),
      _rootNode(other._rootNode),
      _chatLogic(other._chatLogic)
{
    std::cerr << ">>> Rule of Five Component: ChatBot Move Constructor <<<" << std::endl;

    other._chatLogic = nullptr;
    other._currentNode = nullptr;
    other._rootNode = nullptr;
}


// DONE: add move assignment operator
// Move assignment - Transfers resources into an already existing object 
// Used via b = std::move(a); 
ChatBot &ChatBot::operator=(ChatBot &&other) noexcept
{
    std::cerr << ">>> Rule of Five Component: ChatBot Move Assignment <<<" << std::endl;

    if (this != &other)
    {
        _chatLogic = other._chatLogic;
        _currentNode = other._currentNode;
        _rootNode = other._rootNode;

        other._chatLogic = nullptr;
        other._currentNode = nullptr;
        other._rootNode = nullptr;
    }

    return *this;
}
// END OF T̶O̶D̶O̶

ChatBot::~ChatBot()
{
    std::cerr << ">>> Rule of Five Component: ChatBot Destructor <<<" << std::endl;
}

// Main function to handle user message
// Finds the next matching node (based on Levehnstein distance of keywords) and 
// moves the chatbot to that node via "_currentNode->MoveChatbotToNewNode(newNode)" 
void ChatBot::ReceiveMessageFromUser(std::string message)
{   

    // Init vars 
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists; // Levenshtein Distance 
    std::vector<GraphEdge *> keywordMatches;
    std::string userInputLower = message;
    
    // Added clearer representation below. Converts every character in userInputLower to lowercase, updating the same string
    // std::transform(userInputLower.begin(), userInputLower.end(), userInputLower.begin(), ::tolower);
    std::transform(
        userInputLower.begin(),  // Start of input
        userInputLower.end(),    // End of input (one past the last character)
        userInputLower.begin(),  // Start of output: overwrite the same string
        ::tolower               // Function applied to each character
    );

    // First, check for direct keyword containment (case-insensitive)
    // Note that keywords are stored in edges. Answers are stored in nodes. 
    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {   
            // Change keyword to lowercase for easier comparison 
            std::string keywordLower = keyword;
            std::transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(), ::tolower);

            // If there is a match, add the edge 
            // Returns std::string::npos if no match exists
            if (userInputLower.find(keywordLower) != std::string::npos) {
                keywordMatches.push_back(edge);
                break; // Only need one keyword match per edge
            }
        }
    }

    /*
    Chooses the chatbot’s next conversation node using this priority:
        1. Use the first direct keyword match.
        2. Otherwise, choose the keyword with the smallest Levenshtein distance.
        3. If there are no keywords to compare, return to the root node.
    */
    GraphNode *newNode = nullptr;
    if (!keywordMatches.empty()) {
        // If multiple matches, pick the first (or could add more logic)
        newNode = keywordMatches[0]->GetChildNode();
    } else {
        // No matches, fall back to Levenshtein distance
        for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
        {
            GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
            for (auto keyword : edge->GetKeywords())
            {
                EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
                levDists.push_back(ed);
            }
        }

        // If there are elements in the Levehnstein vector: 
        if (levDists.size() > 0)
        {   
            // Sort the vector, choose the first element ie closest in terms of Levehnstein distance 
            std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
            newNode = levDists.at(0).first->GetChildNode();
        }
        else
        {   
            // If no match at all, use the root node 
            newNode = _rootNode;
        }
    }
    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node)
{   
    // Sets current node to be the argument node 
    _currentNode = node;
    _chatLogic->SetChatbotHandle(this);

    std::vector<std::string> answers = _currentNode->GetAnswers(); // Extract current node answers 
    std::mt19937 generator(int(std::time(0))); // Create a random number given current time as the seed 
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator)); // Gets a random answer string, based off the random generated index 

    // Sends the answer string to the user 
    _chatLogic->SendMessageToUser(answer);

    // If the answer contains the reset message, reset to root and print the root answer
    if (answer.find("There are no more topics in this section, starting over!") != std::string::npos) {
        if (_currentNode != _rootNode) {
            
            // Set chatbot to root node. The move will call SetCurrentNode on the root node, which will print the welcome message.
            _currentNode->MoveChatbotToNewNode(_rootNode);
            // No need to set the node as node setting is already done. 
            
        }
    }
}

// Computes Levehnstein Distance given 2 strings: Used to see how similar the 2 strings are to each other. 
int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);
    const size_t m(s1.size());
    const size_t n(s2.size());
    if (m == 0)
        return n;
    if (n == 0)
        return m;
    size_t *costs = new size_t[n + 1];
    for (size_t k = 0; k <= n; k++)
        costs[k] = k;
    size_t i = 0;
    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;
        size_t j = 0;
        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
        {
            size_t upper = costs[j + 1];
            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }
            corner = upper;
        }
    }
    int result = costs[n];
    delete[] costs;
    return result;
}