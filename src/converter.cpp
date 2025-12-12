#include "converter.h"
#include <fstream>
#include <iostream>

ConverterJSON::ConverterJSON (const std::string& config_path,
                              const std::string& request_path,
                              const std::string& answers_path)
                              : configPath(config_path), requestsPath(request_path), answersPath(answers_path) {}

/**
* Метод внесения данных в файл config.json
*/
std::vector<std::string> ConverterJSON::InputTextDocuments() const {
    std::string name;
    std::string version;
    int max_responses;
    std::vector<std::string> files;

    std::cout << "Input the name, version, max_responses: " << std::endl;
    std::cin >> name >> version >> max_responses;

    nlohmann::json dict;
    dict[CONFIG][NAME] = name;
    dict[CONFIG][VERSION] = version;
    dict[CONFIG][MAX_RESPONSES] = max_responses;

    int n = 0;
    std::cout << "Input numbers of the files: " << std::endl;
    std::cin >> n;

    for (int i = 0; i < n; ++i) {
        std::string filesPath;
        std::cout << "Input filesPath: " << std::endl;
        std::cin >> filesPath;

        files.push_back(filesPath);
        dict[FILES].push_back(filesPath);
    }

    std::ofstream file(configPath);
    file << dict.dump(4);

    return files;
}

/**
 ** Метод получения содержимого файлов
  * @return Возвращает список с содержимым файлов перечисленных
  *       в config.json
  */
std::vector<std::string> ConverterJSON::getTextDocuments() {

    std::vector<std::string> documents;
    std::ifstream fileIn(configPath);

    if (!fileIn.is_open()) {
        std::cerr << "Error! Couldn`t open file!" << std::endl;
        return documents;
    }
    if (fileIn.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Json-file is empty!" << std::endl;
        return documents;
    }

    nlohmann::json configData;

    try {
        fileIn >> configData;
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return documents;
    }

    if (configData.contains(FILES) && configData[FILES].is_array()) {
        std::cout << "File list: " << std::endl;
        for (const auto &item: configData[FILES]) {
            if (item.is_string()) {
                documents.push_back(item.get<std::string>());
                std::cout << " - " << item.get<std::string>() << std::endl;
            }
        }
    } else {
        std::cerr << "Key 'files' not found or not array" << std::endl;
    }
    return documents;
};

/**
  * Метод считывает поле max_responses для определения предельного
  *  количества ответов на один запрос
  *  @return
  */
int ConverterJSON::getResponsesLimit() {
    std::ifstream responsesOut(configPath);

    if (!responsesOut) {
        throw std::runtime_error("Cannot open config file: " + configPath);
    }

    nlohmann::json data;

    try {
        responsesOut >> data;
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }

    if (!data.contains(CONFIG)) {
        throw std::runtime_error("Key 'config' not found in config.json");
    }

    if (!data[CONFIG].contains(MAX_RESPONSES)) {
        throw std::runtime_error("Key 'max_responses' not found in config.json");
    }

    if (!data[CONFIG][MAX_RESPONSES].is_number_integer()) {
        throw std::runtime_error("'max_responses' must be integer");
    }

    int limit = data[CONFIG][MAX_RESPONSES].get<int>();
    std::cout << "max_responses: " << limit << std::endl;

    return limit;
};

/**
  * Метод получения запросов из файла requests.json
  * @return возвращает список запросов из файла requests.json
  */
std::vector<std::string> ConverterJSON::getRequests() {
    std::vector<std::string> requestsVector;
    std::string request;
    int count = 0;

    std::cout << "Input number of the requests: " << std::endl;
    std::cin >> count;

    for (int i = 0; i < count; ++i) {
        std::cout << "Input the request: " << std::endl;
        std::cin >> request;
        requestsVector.push_back(request);
    }

    nlohmann::json requestsFile;
    requestsFile[REQUESTS] = requestsVector;

    std::ofstream requestsStreamIn(requestsPath);
    if (!requestsStreamIn.is_open()) {
        std::cerr << "Error! Couldn't open file for writing!" << std::endl;
    }
    requestsStreamIn << requestsFile.dump(4);
    requestsStreamIn.close();

    std::ifstream requestsStreamOut (requestsPath);
    if (!requestsStreamOut.is_open()) {
        std::cerr << "Error! Couldn`t open file for reading!" << std::endl;
        return requestsVector;
    }

    requestsStreamOut >> requestsFile;

    if (!requestsFile.contains(REQUESTS) || !requestsFile[REQUESTS].is_array()) {
        requestsFile[REQUESTS] = nlohmann::json::array();
    }

    std::cout << requestsFile.dump(4) << std::endl;

    return requestsVector;
};

/**
  * Положить в файл answers.json результаты поисковых запросов
  */
void ConverterJSON::putAnswers(std::vector<std::vector<RelativeIndex>> answers) {
    nlohmann::json j;
    j[ANSWERS] = nlohmann::json::array();

    for (size_t i = 0; i < answers.size(); ++i) {
        nlohmann::json requestResult;

        requestResult[REQUEST] = REQUEST + std::to_string(i + 1);

        if (answers[i].empty()) {
            requestResult[RESULT] = "false";
        } else {
            requestResult[RESULT] = "true";

            requestResult[RELEVANCE] = nlohmann::json::array();

            for (const auto &rel: answers[i]) {
                nlohmann::json relItem;
                relItem[DOCID] = rel._doc_id;
                relItem[RANK] = rel._rank;
                requestResult[RELEVANCE].push_back(relItem);
            }
        }
        j[ANSWERS].push_back(requestResult);
    }
    std::ofstream file(answersPath);
    if (!file.is_open()) {
        throw std::runtime_error("Error! Couldn`t open file!:" + answersPath);
    }
    file << std::setw(4) << j;
};

std::string ConverterJSON::getName() const {
    std::ifstream file(configPath);
    nlohmann::json j;
    file >> j;
    return j[CONFIG][NAME];
};

std::string ConverterJSON::getVersion() const {
    std::ifstream file(configPath);
    nlohmann::json j;
    file >> j;
    return j[CONFIG][VERSION];
};



