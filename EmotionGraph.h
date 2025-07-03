#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <queue>
#include <tuple>

struct Edge {
    std::string target;
    double baseWeight;
};

class EmotionGraph {
public:
    void addNode(const std::string& node);
    void addEdge(const std::string& from, const std::string& to, double weight);
    void buildExpandedGraph();

    void setEmotionPriorities(const std::unordered_map<std::string, double>& priorities);
    void setEmotionKeywords(const std::unordered_map<std::string, std::unordered_set<std::string>>& keywords);

    std::vector<std::pair<std::string, double>> getTopEmotions(
        const std::unordered_map<std::string, double>& intensityScores,
        const std::unordered_map<std::string, double>& toneSimilarity,
        int topK = 3);

    std::unordered_map<std::string, std::vector<Edge>> graph;
    std::unordered_map<std::string, double> emotionPriority;
    std::unordered_map<std::string, std::unordered_set<std::string>> emotionKeywords;

private:
    void addNodeInternal(const std::string& node);

};
