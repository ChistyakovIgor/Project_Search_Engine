#include "inverted_index.h"
#include <sstream>
#include <map>

InvertedIndex::InvertedIndex(const InvertedIndex& other) {
    //std::lock_guard<std::mutex> lock(other._dictionary_mtx);
    docs = other.docs;
    freq_dictionary = other.freq_dictionary;
}
void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    std::lock_guard<std::mutex> lock(_dictionary_mtx);
    docs = input_docs;
    freq_dictionary.clear();

    for (size_t docId = 0; docId < docs.size(); ++docId) {
        std::map<std::string, size_t> word_count;
        std::istringstream stream(docs[docId]);
        std::string word;

        while (stream >> word) {
            ++word_count[word];
        }

        for (auto& [word, count] : word_count) {
            freq_dictionary[word].push_back({docId, count});
            //freq_dictionary[word][docId] = static_cast<size_t>(count);
        }
    }
};

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::lock_guard<std::mutex> lock(_dictionary_mtx);
    std::vector<Entry> entries;
    auto it = freq_dictionary.find(word);
    if (it != freq_dictionary.end()) {
        for (const auto& [doc_id, count] : it->second) {
            entries.push_back({doc_id, count});
        }
    }
    return entries;
};