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

    // Set keywords per emotion
    void setEmotionKeywords(const std::unordered_map<std::string, std::unordered_set<std::string>>& keywords); 

    //Return top K emotions ranked by scores
    std::vector<std::pair<std::string, double>> getTopEmotions(
        const std::unordered_map<std::string, double>& intensityScores,
        const std::unordered_map<std::string, double>& toneSimilarity,
        int topK = 3);

    // Adjacency list graph structure
    std::unordered_map<std::string, std::vector<Edge>> graph;    

    // Priorities per emotion
    std::unordered_map<std::string, double> emotionPriority;   

    // Keywords per emotion
    std::unordered_map<std::string, std::unordered_set<std::string>> emotionKeywords;  

private:
    // Helper to add node if not present
    void addNodeInternal(const std::string& node);  
};
