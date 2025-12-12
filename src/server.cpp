#include "server.h"
#include "inverted_index.h"
#include <sstream>
#include <algorithm>

//SearchServer::SearchServer(InvertedIndex& idx) : _index(&idx) {};
/**
 * Метод обработки поисковых запросов
 * @param queries_input поисковые запросы взятые из файла requests.json
 * @return возвращает отсортированный список релевантных ответов для заданных запросов
 */
std::vector<std::vector<RelativeIndex>> SearchServer::search(
        const std::vector<std::string>& queries, size_t responses_limit) {

    std::vector<std::vector<RelativeIndex>> results;

    for (const auto& query : queries) {

        std::map<size_t, size_t> doc_freqs;
        std::istringstream iss(query);
        std::string word;

        while (iss >> word) {
            for (const auto &[doc_id, count]: _index->GetWordCount(word)) {
                doc_freqs[doc_id] += count;
            }
        }

        std::vector<RelativeIndex> rels;

        if (!doc_freqs.empty()) {
            size_t max_freq = std::max_element(
                    doc_freqs.begin(), doc_freqs.end(),
                    [](auto &a, auto &b) { return a.second < b.second; }
            )->second;
            for (auto &[doc_id, freq]: doc_freqs) {
                rels.push_back({doc_id, static_cast<float>(freq) / max_freq});
            }

            std::sort(rels.begin(), rels.end(),
                      [](auto &a, auto &b) { return a._rank > b._rank; });

            if (rels.size() > responses_limit) {
                rels.resize(responses_limit);
            }
        }
        results.push_back(rels);
    }
    return results;
};

std::map<std::string, size_t> get_indexes_for_request_words(std::vector<std::string>& vec);
void handleRequest(const std::string& request, std::vector<RelativeIndex>& , size_t max_responses);
