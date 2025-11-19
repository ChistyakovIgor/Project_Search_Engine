#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include "server.h"

/**
*  Класс для работы с JSON-файлами
*/
class ConverterJSON {
    std::string _config_path;
    std::string _request_path;
    std::string _answers_path;

    std::string _name;
    std::string _version;
    int _max_responses;
    std::vector<std::string> _file_names;
private:
    std::string configPath;
    std::string requestsPath;
    std::string answersPath;
public:
    ConverterJSON(const std::string&  config_path = "config.json",
                  const std::string& request_path = "request.json",
                  const std::string& answers_path = "answers.json");
        /**
         * Метод внесения данных в файл config.json
         */
        std::vector<std::string> InputTextDocuments() const;

        /**
         * Метод получения содержимого файлов
         * @return Возвращает список с содержимым файлов перечисленных
         *       в config.json
         */
        std::vector<std::string> getTextDocuments();

        /**
         * Метод считывает поле max_responses для определения предельного
         *  количества ответов на один запрос
         * @return
         */
        int getResponsesLimit();

        /**
        * Метод внесения запросов в файл requests.json
        */
        std::vector<std::string> InputRequests(int count);

        /**
         * Метод получения запросов из файла requests.json
         * @return возвращает список запросов из файла requests.json
         */
        std::vector<std::string> getRequests();

        /**
         * Положить в файл answers.json результаты поисковых запросов
         */
        void putAnswers(std::vector<std::vector<RelativeIndex>> answers);

        std::string getName() const;

        std::string getVersion() const;

};
