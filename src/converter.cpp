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
    std::string max_responses;
    std::vector<std::string> files;
    std::cout << "Input the name, version, max_responses: " << std::endl;
    std::cin >> name >> version >> max_responses;
    std::ofstream file(configPath);
    nlohmann::json dict;
    dict["name"] = name;
    dict["version"] = version;
    dict["max_responses"] = max_responses;
    int n = 0;
    std::cout << "Input numbers of the files: " << std::endl;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
    std::string filesPath;
    std::cout << "Input filesPath: " << std::endl;
    std::cin >> filesPath;
    files.push_back(filesPath);
    }
    for (int i = 0; i < n; ++i) {
    dict["files"][i] = files[i];
    }
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
    nlohmann::json configData;
    if (!fileIn) {
        std::cerr << "Error! Couldn`t open file!" << std::endl;
    }
    if (fileIn.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Json-file is empty!" << std::endl;
        return documents;
    }
    try {
        fileIn >> configData;
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return documents;
    }
    if (configData.contains("files") && configData["files"].is_array()) {
        std::cout << "File list: " << std::endl;
        //std::cout << configData << std::endl;
        for (const auto &item: configData["files"]) {
            if (configData["files"].is_string()) {
                //std::cout << configData["files"][i] << std::endl;
                documents.push_back(configData["files"].get<std::string>());
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
    nlohmann::json responsesData;
    if (!responsesOut) {
        std::cerr << "Error! Couldn`t open file!" << std::endl;
        return 0;
    }
    if (responsesOut.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Json-file is empty!" << std::endl;
        return 0;
    }
    try {
        responsesOut >> responsesData;
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return 0;
    }

    //if (responsesData["max_responses"].is_number_integer()) {
    if (responsesData.contains("config") &&
        responsesData["config"].contains("max_responses") &&
        responsesData["config"]["max_responses"].is_number_integer()) {
        int limit = responsesData["config"]["max_responses"].get<int>();
        std::cout << "max_responses: " << limit << std::endl;
        return limit;
    } else {
        std::cerr << "Key 'max_responses' not found or not an integer!" << std::endl;
        return 0;
    }
};

/**
  * Метод внесения запросов в файл requests.json
  */
std::vector<std::string> ConverterJSON::InputRequests(int count) {
//std::map <std::string, std::string> requestMap;
//std::vector<std::string> requests;
//std::vector <std::pair<std::string, std::string>> requestMap;
//std::string request;
//std::cout << "Input number of the requests: " << std::endl;
//std::cin >> count;
//for (int i = 0; i < count; ++i) {
//std::cout << "Input the request: " << std::endl;
//std::cin >> request;
//std::getline(std::cin, request);
//std::string requestID = "request" + std::to_string(0) + std::to_string(0) + std::to_string(i + 1);
//requestMap[requestID] = request;
//requests.push_back(request);
//}
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
    requestsFile["request"] = requestsVector;

    std::ofstream requestsStreamIn(requestsPath);
    requestsStreamIn << requestsFile.dump();
    requestsStreamIn.close();

    std::ifstream requestsStreamOut (requestsPath);
    if (!requestsStreamOut) {
        std::cerr << "Error! Couldn`t open file!" << std::endl;
    }
    if (requestsStreamOut.peek() != std::ifstream::traits_type::eof()) {
        requestsStreamOut >> requestsFile;
    } else {
        std::cerr << "Json-file is empty!" << std::endl;
    }
    if (!requestsFile.contains("requests") || requestsFile["requests"].is_array()) {
        requestsFile["requests"] = nlohmann::json::array();
    }
    std::cout << requestsFile.dump(4) << std::endl;
    requestsStreamOut.close();
    return requestsVector;
};

/**
  * Положить в файл answers.json результаты поисковых запросов
  */
void ConverterJSON::putAnswers(std::vector<std::vector<RelativeIndex>> answers) {
    nlohmann::json j;
    for (size_t i = 0; i < answers.size(); ++i) {
        nlohmann::json requestResult;
        requestResult["request"] = "request" + std::to_string(i + 1);

        if (answers[i].empty()) {
            requestResult["result"] = "false";
        } else {
            requestResult["result"] = "true";
            requestResult["relevance"] = nlohmann::json::array();
            for (auto &rel: answers[i]) {
                nlohmann::json relItem;
                relItem["docid"] = rel._doc_id;
                relItem["rank"] = rel._rank;
                requestResult["relevance"].push_back(relItem);
            }
        }
        j["answers"].push_back(requestResult);
    }
    std::ofstream file("answers.json");
    file << std::setw(4) << j;
};

std::string ConverterJSON::getName() const {
    return "Search Engine";
};

std::string ConverterJSON::getVersion() const {
    return "1.0";
};



