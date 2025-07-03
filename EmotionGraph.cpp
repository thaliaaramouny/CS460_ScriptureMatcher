#include "EmotionGraph.h"
#include <iostream>
#include <algorithm>
#include <cmath>

// Adds a node to the graph if does not already exist
void EmotionGraph::addNodeInternal(const std::string& node) {
    if (graph.find(node) == graph.end()) {
        graph[node] = {};  
    }
}

// Public method to add a node, uses internal method
void EmotionGraph::addNode(const std::string& node) {
    addNodeInternal(node);
}

// Adds an undirected edge between two nodes with a given weight
void EmotionGraph::addEdge(const std::string& from, const std::string& to, double weight) {
    addNodeInternal(from);  
    addNodeInternal(to);    

    graph[from].push_back({ to, weight }); 
    graph[to].push_back({ from, weight }); 
}

// Builds the full emotion graph with emotion nodes, keyword nodes, and weighted edges
void EmotionGraph::buildExpandedGraph() {
    graph.clear(); 

    // Define primary emotion nodes
    std::vector<std::string> emotions = {
        "anxiety", "sadness", "fear", "anger", "joy", "guilt", "love"
    };
    for (const auto& emo : emotions) addNode(emo);

    // Map of emotions to keywords and weights representing closeness/relevance
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> emotionKeywordsWithWeights = {
        {"anxiety", {{"worried",1.0},{"overwhelmed",1.0},{"stressed",1.0},{"uneasy",1.2},
                    {"panicking",0.9},{"anxious",1.0},{"tense",1.1},{"pressured",1.2},
                    {"nervousness",1.0},{"exhausted", 0.8}, {"jittery",1.3},{"restless",1.2},{"fearful",1.3}}},
        {"sadness", {{"sad",1.0},{"down",1.0},{"lonely",1.1},{"depressed",1.0},{"crying",1.0},
                    {"hurt",1.2},{"broken",1.0},{"heartbroken",0.9},{"hopeless",1.0},
                    {"grief",0.8},{"melancholy",1.3},{"gloomy",1.2}}},
        {"fear", {{"afraid",1.0},{"scared",1.0},{"fearful",1.0},{"terrified",0.8},
                  {"nervous",1.0},{"shaking",1.2},{"paranoid",0.9},{"panicked",0.9}}},
        {"anger", {{"angry",1.0},{"mad",1.0},{"furious",0.8},{"rage",0.9},
                   {"irritated",1.2},{"annoyed",1.1},{"frustrated",1.0},{"resentful",0.9}}},
        {"joy", {{"happy",1.0},{"joyful",0.9},{"excited",1.0},{"grateful",1.1},
                 {"thankful",1.2},{"cheerful",1.0},{"delighted",0.8},{"content",1.0}}},
        {"guilt", {{"guilty",1.0},{"ashamed",1.0},{"regretful",0.9},{"remorseful",0.8},
                   {"sorry",1.1},{"blame",1.2}}},
        {"love", {{"loved",1.0},{"cherished",0.9},{"valued",1.1},{"adored",0.8},
                  {"cared",1.0},{"special",1.0},{"affection",1.1}}}
    };

    // For each emotion, add keyword nodes and edges connecting keywords to emotion nodes
    for (const auto& [emotion, kws] : emotionKeywordsWithWeights) {
        emotionKeywords[emotion].clear();  
        for (const auto& [kw, weight] : kws) {
            addNode(kw);  
            addEdge(kw, emotion, weight); 
            emotionKeywords[emotion].insert(kw); 
        }
    }

    // Add edges connecting emotions to model relationships between emotions
    addEdge("anxiety", "fear", 2.0);
    addEdge("sadness", "guilt", 2.5);
    addEdge("joy", "love", 1.5);
    addEdge("anger", "fear", 3.0);
    addEdge("guilt", "sadness", 2.5);

    // Add additional edges between keyword nodes to bridge related concepts
    addEdge("nervous", "anxiety", 1.0);
    addEdge("nervous", "fear", 1.0);
    addEdge("worried", "nervous", 1.2);
    addEdge("depressed", "sad", 1.1);
    addEdge("hurt", "sad", 1.2);
    addEdge("angry", "frustrated", 1.1);
    addEdge("happy", "joyful", 0.9);
}

// Sets the priority values for emotions, influencing traversal costs
void EmotionGraph::setEmotionPriorities(const std::unordered_map<std::string, double>& priorities) {
    for (const auto& [emotion, priority] : priorities) {
        if (!graph.count(emotion)) {
            std::cerr << "[Warning] Emotion '" << emotion << "' not found in graph.\n";
        }
        emotionPriority[emotion] = priority;
    }
}

// Updates emotion keywords map externally if needed
void EmotionGraph::setEmotionKeywords(const std::unordered_map<std::string, std::unordered_set<std::string>>& keywords) {
    for (const auto& [emotion, kws] : keywords) {
        if (!graph.count(emotion)) {
            std::cerr << "[Warning] Emotion '" << emotion << "' not found in graph.\n";
        }
        emotionKeywords[emotion] = kws;
    }
}

// Performs a custom Dijkstra-like traversal to find top emotions related to input.
std::vector<std::pair<std::string, double>> EmotionGraph::getTopEmotions(
    const std::unordered_map<std::string, double>& intensityScores,
    const std::unordered_map<std::string, double>& toneSimilarity,
    int topK
) {
    using PQElement = std::tuple<double, std::string, std::vector<std::string>>; 
    // Best cost to reach each node
    std::unordered_map<std::string, double> distances;  
    // Minheap for Dijkstra traversal
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<>> pq; 

    // Initialize priority queue with input keywords based on inverse of intensity (higher intensity → lower cost)
    for (const auto& [keyword, intensity] : intensityScores) {
        if (!graph.count(keyword)) continue;  
        double safeIntensity = std::max(intensity, 0.1);  
        double initCost = 1.0 / (safeIntensity + 1e-6);   
        pq.push({ initCost, keyword, {keyword} });
        distances[keyword] = initCost;
    }

    std::unordered_set<std::string> visited;

    // Adaptive Early Stopping Setup 
    double avgIntensity = 0.0;
    double avgTone = 0.0;
    for (const auto& [_, score] : intensityScores) avgIntensity += score;
    for (const auto& [_, score] : toneSimilarity) avgTone += score;
    if (!intensityScores.empty()) avgIntensity /= intensityScores.size();
    if (!toneSimilarity.empty()) avgTone /= toneSimilarity.size();

    // Higher intensity/tone allows longer paths; low intensity > stricter cutoff
    double MAX_PATH_COST = 100.0 / (avgIntensity + avgTone + 1e-3);

    // Custom dijkstra traversal
    while (!pq.empty()) {
        auto [curCost, node, path] = pq.top();
        pq.pop();

        if (visited.count(node)) continue;
        visited.insert(node);

        // Early stopping: ignore long paths
        if (curCost > MAX_PATH_COST) continue;

        for (const auto& edge : graph[node]) {
            const std::string& neighbor = edge.target;
            if (visited.count(neighbor)) continue;

            double baseWeight = edge.baseWeight;

            // Use user intensity for neighbor word or fallback to 1.0
            double intensity = intensityScores.count(neighbor) ? intensityScores.at(neighbor) : 1.0;
            intensity = std::max(intensity, 0.1);

            // Tone similarity boost if available
            double tone = toneSimilarity.count(neighbor) ? toneSimilarity.at(neighbor) : 0.0;

            // Priority (if set) modifies weight; lower priority = higher cost
            double priorityFactor = emotionPriority.count(neighbor) ? 1.0 / std::max(0.01, emotionPriority.at(neighbor)) : 1.0;

            // Boost path cost if the neighbor has appeared recently in path (reduces repetition)
            double contextBoost = 1.0;
            for (int i = std::max(0, (int)path.size() - 3); i < (int)path.size(); ++i) {
                if (path[i] == neighbor) {
                    contextBoost += 0.5;
                    break;
                }
            }

            // Dynamic cost adjusts edge weight based on emotional relevance
            double denom = intensity + tone + 1e-6;
            double dynamicCost = baseWeight / denom;
            dynamicCost *= priorityFactor * contextBoost;

            double newCost = curCost + dynamicCost;

            // Update distance if this is a better (lower cost) path
            if (!distances.count(neighbor) || newCost < distances[neighbor]) {
                distances[neighbor] = newCost;
                auto newPath = path;
                newPath.push_back(neighbor);
                pq.push({ newCost, neighbor, newPath });
            }
        }
    }

    // Filter only actual emotion category nodes from graph
    std::unordered_set<std::string> emotionNodes;
    for (const auto& [emotion, _] : emotionKeywords)
        emotionNodes.insert(emotion);

    std::vector<std::pair<std::string, double>> results;

    for (const auto& [node, cost] : distances) {
        if (!emotionNodes.count(node) || cost > MAX_PATH_COST) continue;

        // Convert path cost to score: lower cost = higher emotion match
        double score = 1.0 / (cost + 1e-6);
        results.emplace_back(node, score);
    }

    // Sort by score in descending order
    std::sort(results.begin(), results.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    if ((int)results.size() > topK)
        results.resize(topK);

    return results;
}
