#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// VerseMapper class declaration:
// Maps emotions (strings) to Bible verses and provides methods to add, retrieve, and recommend verses based on input similarity.
class VerseMapper {
public:
    VerseMapper();  
        void addVerse(const std::string& emotion, const std::string& verse);
        std::vector<std::string> getVerses(const std::string& emotion) const;
    // Sets emotion keywords mapping 
    void setEmotionKeywords(const std::unordered_map<std::string, std::unordered_set<std::string>>& keywords);

    std::vector<std::string> getRankedVerses(
        const std::string& emotion,
        const std::vector<std::string>& inputTokens,
        const std::vector<std::string>& neighborTokens,
        double similarityThreshold
    ) const;
    std::vector<std::string> getRecommendedVerses(
        const std::string& emotion,
        const std::vector<std::string>& inputTokens,
        const std::vector<std::string>& neighborTokens
    ) const;

private:
    std::unordered_map<std::string, std::vector<std::string>> verseMap;  
    std::unordered_map<std::string, std::unordered_set<std::string>> emotionKeywords;  

    // Helper: tokenize a string into a set of words excluding stop words/punctuation
    std::unordered_set<std::string> tokenizeToSet(const std::string& text) const;
    // Helper: calculate Jaccard similarity between two word sets
    double jaccardSimilarity(const std::unordered_set<std::string>& setA,
        const std::unordered_set<std::string>& setB) const;
};
