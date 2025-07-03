#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class VerseMapper {
public:
    VerseMapper();

    void addVerse(const std::string& emotion, const std::string& verse);
    std::vector<std::string> getVerses(const std::string& emotion) const;
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

    std::unordered_set<std::string> tokenizeToSet(const std::string& text) const;
    double jaccardSimilarity(const std::unordered_set<std::string>& setA,
        const std::unordered_set<std::string>& setB) const;
};
