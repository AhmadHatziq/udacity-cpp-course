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
}

// TODO the following:
    // TODO: add copy constructor
    // TODO: add copy assignment operator
    // TODO: add move constructor
    // TODO: add move assignment operator
// END OF TODO

ChatBot::~ChatBot()
{
    std::cerr << ">>> Rule of Five Component: ChatBot Destructor <<<" << std::endl;
}

void ChatBot::ReceiveMessageFromUser(std::string message)
{
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists;
    std::vector<GraphEdge *> keywordMatches;
    std::string userInputLower = message;
    std::transform(userInputLower.begin(), userInputLower.end(), userInputLower.begin(), ::tolower);
    // First, check for direct keyword containment (case-insensitive)
    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {
            std::string keywordLower = keyword;
            std::transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(), ::tolower);
            if (userInputLower.find(keywordLower) != std::string::npos) {
                keywordMatches.push_back(edge);
                break; // Only need one keyword match per edge
            }
        }
    }
    GraphNode *newNode = nullptr;
    if (!keywordMatches.empty()) {
        // If multiple matches, pick the first (or could add more logic)
        newNode = keywordMatches[0]->GetChildNode();
    } else {
        // Fall back to Levenshtein distance
        for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
        {
            GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
            for (auto keyword : edge->GetKeywords())
            {
                EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
                levDists.push_back(ed);
            }
        }
        if (levDists.size() > 0)
        {
            std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
            newNode = levDists.at(0).first->GetChildNode();
        }
        else
        {
            newNode = _rootNode;
        }
    }
    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    _currentNode = node;
    std::vector<std::string> answers = _currentNode->GetAnswers();
    std::mt19937 generator(int(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator));
    _chatLogic->SendMessageToUser(answer);
    // If the answer contains the reset message, reset to root and print the root answer
    if (answer.find("There are no more topics in this section, starting over!") != std::string::npos) {
        if (_currentNode != _rootNode) {
            _currentNode->MoveChatbotToNewNode(_rootNode);
            // The move will call SetCurrentNode on the root node, which will print the welcome message.
        }
    }
}

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