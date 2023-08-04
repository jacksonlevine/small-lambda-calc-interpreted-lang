#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <functional>
#include <chrono>

std::map<std::string, std::function<std::string(std::vector<std::string>)>> funcs;
std::map<std::string, std::string> variables;

std::string generate_unique_id() {
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    std::stringstream ss;
    ss << nanos;
    return ss.str();
}

void function_definitions()
{
    funcs["concat"] = [](std::vector<std::string> args)
    {
        std::string result;
        for (auto &s : args)
        {
            result += s;
        }
        return result;
    };
    funcs["print"] = [](std::vector<std::string> args)
    {
        for (auto &s : args)
        {
            std::cout << s << std::endl;
        }
        return std::string("success");
    };
    funcs["let"] = [](std::vector<std::string> args)
    {
        if (args.size() == 1)
        {
            variables[args[0]] = std::string("");
        }
        if (args.size() > 1)
        {
            variables[args[0]] = std::string(args[1]);
        }
        return std::string("");
    };
    funcs["+"] = [](std::vector<std::string> args)
    {
        bool floats = false;
        for (auto &s : args)
        {
            if (s.find('.') != std::string::npos)
                floats = true;
        }
        if (floats)
        {
            float out = 0.0f;
            for (auto &s : args)
            {
                out += std::stof(s);
            }
            return std::to_string(out);
        }
        else
        {
            int out = 0;
            for (auto &s : args)
            {
                out += std::stoi(s);
            }
            return std::to_string(out);
        }
        return std::string("");
    };
    funcs["p"] = funcs["print"];
    funcs["set"] = funcs["let"];
    funcs["l"] = funcs["let"];
    funcs["le"] = funcs["let"];
    funcs["se"] = funcs["let"];
    funcs["s"] = funcs["let"];
}
std::function<std::string(void)> recurse_and_call_line(std::string line, std::istringstream &full_stream)
{
    if (line.size() < 1)
    {
        return []()
        { return std::string(""); };
    }
    std::istringstream line_stream(line);
    std::string word;
    std::vector<std::string> current_expression;
    while (line_stream >> word)
    {
        if (word.find('{') != std::string::npos)
        {
            // std::cout << word;
            // Then the whole expression has been a function declaration, we need to get all the lines

            std::string func_id = generate_unique_id();
            std::string nfname(current_expression[0]);
            current_expression.erase(current_expression.begin());
            std::string func_body = "";

            std::string next_line;
            int depth = 1;
            while (std::getline(full_stream, next_line) && depth != 0)
            {
                std::istringstream func_stream(next_line);
                std::string next_word;
                std::string nl = "";

                func_stream >> next_word;

                if (next_word.find('}') != std::string::npos)
                {
                    depth -= 1;
                }
                if (next_word.find('{') != std::string::npos)
                {
                    depth += 1;
                }
                if(next_word.front() == '.') //We need to handle local variables differently
                {
                    next_word.insert(1, func_id);
                }

                if (depth != 0)
                {
                    nl += next_word;
                }
                while (func_stream >> next_word && depth != 0)
                {
                    if (next_word.find('}') != std::string::npos)
                    {
                        depth -= 1;
                    }
                    if (next_word.find('{') != std::string::npos)
                    {
                        depth += 1;
                    }
                    if(next_word.front() == '.') //We need to handle local variables differently
                    {
                        next_word.insert(1, func_id);
                    }
                    if (depth != 0)
                    {
                        nl = (nl + " ") + next_word;
                    }
                }

                func_body += nl + '\n';
            }
            funcs[nfname] = [func_id, func_body, current_expression](std::vector<std::string> args) {

                int ind = 0;
                for(auto& s : current_expression) //set up local variables for func body
                {
                    variables[func_id + s] = args[ind];
                    ind += 1;
                }

                std::istringstream strm(func_body);
                std::string line_;
                while(std::getline(strm, line_))
                {
                    recurse_and_call_line(line_, strm)();
                }

                for(auto& s : current_expression) //tear down local variables for func body
                {
                    variables.erase(func_id + s);
                }
                
                return std::string("success"); //Support return values later

            };
            //std::cout << func_body << std::endl;
            return []()
            { return std::string(""); };
        }
        if (word.front() == '(')
        {
            int depth = std::count(word.begin(), word.end(), '(');
            std::string inner_line = "";
            inner_line += word;
            while (depth != 0)
            {
                line_stream >> word;
                inner_line = (inner_line + " ") + word;
                if (word.front() == '(')
                {
                    depth += std::count(word.begin(), word.end(), '(');
                }
                if (word.back() == ')')
                {
                    depth -= std::count(word.begin(), word.end(), ')');
                }
            }
            inner_line = inner_line.substr(1, inner_line.size() - 2);
            word = recurse_and_call_line(inner_line, full_stream)();
        }
        if (word.front() == '.')
        {
            word = variables[word.substr(1)];
        }
        if (word.front() == '"')
        {
            std::string literal = "";
            bool end = false;

            int count = std::count(word.begin(), word.end(), '"');
            if (count >= 2)
            {
                end = true;
                literal += word.substr(1, word.size() - 2);
            }
            else
            {
                literal += word.substr(1);
            }

            char ch;
            while (line_stream.get(ch) && ch != '"' && !end)
            {
                literal += ch;
            }
            word = literal;
        }
        current_expression.push_back(word);
    }
    std::string fname(current_expression[0]);
    current_expression.erase(current_expression.begin());
    return [fname, current_expression]()
    { return funcs[fname](current_expression); };
}
int main(int argc, char *argv[])
{
    function_definitions();
    std::ifstream file(argv[1]);
    std::stringstream buf;
    buf << file.rdbuf();
    std::istringstream stream(buf.str());
    std::string line;
    while (std::getline(stream, line))
    {
        recurse_and_call_line(line, stream)();
    }
}