#include <iostream>
#include "converter.h"
#include "server.h"
#include "inverted_index.h"
#include <fstream>

int main() {
    ConverterJSON converter;
    auto files = converter.getTextDocuments();

    InvertedIndex index;
    index.UpdateDocumentBase(files);

    auto requests = converter.getRequests();

    int limit = converter.getResponsesLimit();

    SearchServer server(index);

    converter.putAnswers(server.search(requests,limit));

    std::cout << converter.getName() << "[" << converter.getVersion() << "] finish job\n";
}