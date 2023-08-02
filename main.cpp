#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <functional>

std::map<std::string, std::function<std::string(std::vector<std::string>)>> funcs;

std::map<std::string, std::string> variables;

void function_definitions() 
{
    funcs["print"] = [](std::vector<std::string> args) {
        for(auto &s : args) {
            std::cout << s << std::endl;
        }
        return std::string("success");
    };
    funcs["let"] = [](std::vector<std::string> args) {
        if(args.size() == 1)
        {
            variables[args[0]] = std::string("");
        }
        if(args.size() > 1)
        {
            variables[args[0]] = std::string(args[1]);
        }
        return std::string("");
    };
    funcs["+"] = [](std::vector<std::string> args) {
        bool floats = false;
        for(auto& s : args) {
            if(s.find('.') != std::string::npos) floats = true;
        }
        if(floats) {
            float out = 0.0f;
            for(auto& s : args) {
                out += std::stof(s);
            }
            return std::to_string(out);
        } else {
            int out = 0;
            for(auto& s : args) {
                out += std::stoi(s);
            }
            return std::to_string(out);
        }
        return std::string("");
    };
    funcs["set"] = funcs["let"];
    funcs["l"] = funcs["let"];
    funcs["le"] = funcs["let"];
    funcs["se"] = funcs["let"];
    funcs["s"] = funcs["let"];
}

std::string recurse_and_call_line(std::string line);

int main(int argc, char *argv[])
{
    function_definitions();
    if(argc < 2) {
        std::cout << "No filename in arguments";
    }
    std::string filename = argv[1];

    std::ifstream file(filename);
    std::stringstream buf;
    buf << file.rdbuf();
    
    std::string code = buf.str();

    std::istringstream stream(code);
    std::string line;
    while(std::getline(stream, line)) {
        recurse_and_call_line(line);
    }
}

std::string recurse_and_call_line(std::string line)
{
    std::istringstream line_stream(line);
    std::string word;
    std::vector<std::string> current_expression;

            while(line_stream >> word)
            {

                //Nested expressions

                if(word.front() == '(') 
                {
                    int depth = std::count(word.begin(), word.end(), '(');
                    std::string inner_line = "";
                    inner_line += word;

                    while(depth != 0) {
                        line_stream >> word;
                        inner_line = (inner_line + " ") + word;
                        if(word.front() == '(') {
                            depth += std::count(word.begin(), word.end(), '(');
                        }
                        if(word.back() == ')') {
                            depth -= std::count(word.begin(), word.end(), ')');
                        }
                    }

                    inner_line = inner_line.substr(1, inner_line.size()-2); //remove parentheses
                    std::cout << "eval nest expr: ";
                    std::cout << inner_line << std::endl;

                    word = recurse_and_call_line(inner_line);
                }

                //Variable references

                if(word.front() == '.') 
                {
                    word = variables[word.substr(1)];
                }

                //Add to expression

                current_expression.push_back(word);
            }

    //We have the expression now
    //Call the function
    std::string fname(current_expression[0]);
    current_expression.erase(current_expression.begin());
    return funcs[fname](current_expression);
    
}

