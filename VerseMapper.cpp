#include "VerseMapper.h"
#include <algorithm>
#include <sstream>
#include <cctype>

const std::unordered_set<std::string> stopWords = {
    "the", "is", "and", "a", "an", "of", "to", "in", "that", "it", "for", "on", "with",
    "as", "by", "at", "i", "you", "he", "she", "we", "they", "be", "this", "will", "but", "do", "not"
};

VerseMapper::VerseMapper() {
    verseMap["anxiety"] = {
        "Philippians 4:6–7 >> Have no anxiety at all, but in everything, by prayer and petition, with thanksgiving, make your requests known to God. Then the peace of God that surpasses all understanding will guard your hearts and minds in Christ Jesus.",
        "1 Peter 5:7 >> Cast all your worries upon him because he cares for you.",
        "Matthew 6:34 >> Do not worry about tomorrow; tomorrow will take care of itself. Sufficient for a day is its own evil."
    };

    verseMap["sadness"] = {
        "Psalm 34:19 >> The Lord is close to the brokenhearted, saves those whose spirit is crushed.",
        "Revelation 21:4 >> He will wipe every tear from their eyes, and there shall be no more death or mourning, wailing or pain, [for] the old order has passed away.",
        "John 16:20 >> Amen, amen, I say to you, you will weep and mourn, while the world rejoices; you will grieve, but your grief will become joy."
    };

    verseMap["fear"] = {
        "Isaiah 41:10 >> Do not fear: I am with you; do not be anxious: I am your God. I will strengthen you, I will help you, I will uphold you with my victorious right hand.",
        "Psalm 23:4 >> Even though I walk through the valley of the shadow of death, I will fear no evil, for you are with me; your rod and your staff comfort me.",
        "2 Timothy 1:7 >> For God did not give us a spirit of cowardice but rather of power and love and self-control."
    };

    verseMap["anger"] = {
        "Ephesians 4:26 >> Be angry but do not sin; do not let the sun set on your anger.",
        "Proverbs 15:1 >> A mild answer turns back wrath, but a harsh word stirs up anger.",
        "James 1:19–20 >> Know this, my dear brothers: everyone should be quick to hear, slow to speak, slow to wrath, for the wrath of a man does not accomplish the righteousness of God."
    };

    verseMap["joy"] = {
        "Nehemiah 8:10 >> Do not be saddened this day, for rejoicing in the Lord is your strength!",
        "Psalm 16:11 >> You will show me the path to life, abounding joy in your presence, the delights at your right hand forever.",
        "Romans 15:13 >> May the God of hope fill you with all joy and peace in believing, so that you may abound in hope by the power of the holy Spirit."
    };

    verseMap["guilt"] = {
        "1 John 1:9 >> If we acknowledge our sins, he is faithful and just and will forgive our sins and cleanse us from every wrongdoing.",
        "Psalm 103:12 >> As far as the east is from the west, so far has he removed our sins from us.",
        "Isaiah 1:18 >> Though your sins be like scarlet, they may become white as snow; though they be red like crimson, they may become white as wool."
    };

    verseMap["love"] = {
        "1 Corinthians 13:4–7 >> Love is patient, love is kind. It is not jealous, love is not pompous, it is not inflated, it is not rude, it does not seek its own interests, it is not quick-tempered, it does not brood over injury, it does not rejoice over wrongdoing but rejoices with the truth. It bears all things, believes all things, hopes all things, endures all things.",
        "John 15:13 >> No one has greater love than this, to lay down one’s life for one’s friends.",
        "Romans 8:38–39 >> For I am convinced that neither death, nor life, nor angels, nor principalities, nor present things, nor future things, nor powers, nor height, nor depth, nor any other creature will be able to separate us from the love of God in Christ Jesus our Lord."
    };
}


void VerseMapper::addVerse(const std::string& emotion, const std::string& verse) {
    verseMap[emotion].push_back(verse);
}

std::vector<std::string> VerseMapper::getVerses(const std::string& emotion) const {
    if (verseMap.find(emotion) == verseMap.end()) return {};
    return verseMap.at(emotion);
}

void VerseMapper::setEmotionKeywords(const std::unordered_map<std::string, std::unordered_set<std::string>>& keywords) {
    emotionKeywords = keywords;
}

std::unordered_set<std::string> VerseMapper::tokenizeToSet(const std::string& text) const {
    std::unordered_set<std::string> tokens;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](char c) { return std::ispunct(static_cast<unsigned char>(c)); }),
            word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty() && stopWords.count(word) == 0) {
            tokens.insert(word);
        }
    }
    return tokens;
}

double VerseMapper::jaccardSimilarity(
    const std::unordered_set<std::string>& setA,
    const std::unordered_set<std::string>& setB) const {

    size_t intersectionSize = 0;
    for (const auto& word : setA) {
        if (setB.count(word)) intersectionSize++;
    }
    size_t unionSize = setA.size() + setB.size() - intersectionSize;

    return unionSize == 0 ? 0.0 : static_cast<double>(intersectionSize) / unionSize;
}

std::vector<std::string> VerseMapper::getRankedVerses(
    const std::string& emotion,
    const std::vector<std::string>& inputTokens,
    const std::vector<std::string>& neighborTokens,
    double similarityThreshold
) const {
    std::vector<std::string> rankedVerses;
    if (verseMap.find(emotion) == verseMap.end()) return rankedVerses;

    std::unordered_set<std::string> inputSet(inputTokens.begin(), inputTokens.end());
    std::unordered_set<std::string> neighborSet(neighborTokens.begin(), neighborTokens.end());

    for (const auto& verse : verseMap.at(emotion)) {
        auto verseTokens = tokenizeToSet(verse);

        double simInput = jaccardSimilarity(verseTokens, inputSet);
        double simNeighbor = jaccardSimilarity(verseTokens, neighborSet);

        double totalSim = simInput + simNeighbor;

        if (totalSim >= similarityThreshold) {
            rankedVerses.push_back(verse);
        }
    }
    return rankedVerses;
}

std::vector<std::string> VerseMapper::getRecommendedVerses(
    const std::string& emotion,
    const std::vector<std::string>& inputTokens,
    const std::vector<std::string>& neighborTokens
) const {
    std::vector<double> thresholds = { 0.03, 0.01, 0.0 };  // lower thresholds, removed 0.05 and 0.02
    for (double threshold : thresholds) {
        auto filtered = getRankedVerses(emotion, inputTokens, neighborTokens, threshold);
        if (!filtered.empty()) {
            return filtered;
        }
    }
    // fallback: return all verses if none passed
    return getVerses(emotion);
}

