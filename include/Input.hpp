#include <iostream>

namespace Local{

    class UserInput{
        public:
            std::string getUserInput(const char* msg, std::istream& input_stream);
            std::string getUserInput(std::istream& input_stream);
    };

}