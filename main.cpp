#include <iostream>
#include "InputProcessor.h"
#include "EmotionGraph.h"
#include "VerseMapper.h"

int main() {
    // Step 1: Get user input
    std::cout << "Enter your feelings or thoughts:\n> ";
    std::string input;
    std::getline(std::cin, input);

    // Step 2: Tokenize and analyze input
    std::vector<std::string> tokens = InputProcessor::tokenize(input);
    std::unordered_map<std::string, double> intensityScores = InputProcessor::scoreIntensities(input);
    
    // Step 3: Build emotion graph
    EmotionGraph graph;
    graph.buildExpandedGraph();

    // Step 4: Compute tone similarity
    std::unordered_map<std::string, double> toneSim = InputProcessor::computeToneSimilarity(
        tokens,
        graph.emotionKeywords
    );

    // Step 5: Score top emotions using modified Dijkstra’s
    std::vector<std::pair<std::string, double>> topEmotions = graph.getTopEmotions(intensityScores, toneSim, 3);

    // Step 6: Retrieve matching Bible verses
    VerseMapper verseMapper;
    verseMapper.setEmotionKeywords(graph.emotionKeywords);

    std::cout << "\nTop emotions detected:\n";
    for (const auto& [emotion, score] : topEmotions) {
        std::cout << "- " << emotion << " (score: " << score << ")\n";

        auto verses = verseMapper.getRecommendedVerses(emotion, tokens, {});
        if (!verses.empty()) {
            std::cout << " Suggested Bible verses:\n";
            for (const std::string& verse : verses) {
                std::cout << "    * " << verse << "\n";
            }
        } else {
            std::cout << "  No matching verses found.\n";
        }

        std::cout << std::endl;
    }

    return 0;
}
