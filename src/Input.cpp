#include "Input.hpp"

namespace Local{
    
    std::string UserInput::getUserInput(const char* msg, std::istream& input_stream){
        std::cout << msg;
        return getUserInput(input_stream);
    }
    std::string UserInput::getUserInput(std::istream& input_stream){
        std::string input;
        std::getline(input_stream, input);
        input_stream.clear();

        return input;
    }
}