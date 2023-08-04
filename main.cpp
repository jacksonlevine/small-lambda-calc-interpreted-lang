#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <functional>
#include <chrono>

std::map<std::string, std::function<std::string(std::vector<std::string>)>> funcs;
std::map<std::string, std::string> variables;
std::map<std::string, std::vector<std::string>> vectors;

bool debug = false;

std::string generate_unique_id()
{
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
    funcs["forin"] = [](std::vector<std::string> args)
    {
        std::string func_id = generate_unique_id();
        std::string vname = args[0];
        std::string fname = args[1];
        for(auto &s : vectors[vname])
        {
            std::vector<std::string> ags;
            ags.push_back(s);
            funcs[fname](ags);
        }
        return std::string("");
    };
    funcs["append"] = [](std::vector<std::string> args)
    {
        std::string item = args[0];
        std::string vname = args[1];
        vectors[vname].push_back(item);
        return std::string("");
    };
    funcs["prepend"] = [](std::vector<std::string> args)
    {
        std::string item = args[0];
        std::string vname = args[1];
        vectors[vname].insert(vectors[vname].begin(), item);
        return std::string("");
    };
    funcs["size"] = [](std::vector<std::string> args)
    {
        std::string vname = args[0];
        return std::to_string(vectors[vname].size());
    };
    funcs["p"] = funcs["print"];
    funcs["set"] = funcs["let"];
    funcs["l"] = funcs["let"];
    funcs["le"] = funcs["let"];
    funcs["se"] = funcs["let"];
    funcs["s"] = funcs["let"];
}
std::function<std::string(void)> recurse_and_call_line(std::string line, std::istringstream &full_stream);
std::string eval_parentheses(std::string word, std::istringstream& line_stream, std::istringstream& full_stream)
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
    return word;
}
std::string eval_quotes(std::string word, std::istringstream& line_stream)
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
    return word;
}

std::string eval_to_literal(std::string word, std::istringstream& line_stream, std::istringstream& full_stream)
{
    if (word.front() == '(')
    {
        word = eval_parentheses(word, line_stream, full_stream);
    }
    if (word.front() == '.')
    {
        word = variables[word.substr(1)];
    }
    if (word.front() == '"')
    {
        word = eval_quotes(word, line_stream);
    }
    return word;
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
                    if (next_word.front() == '.' && depth == 1) // We need to handle local variables differently
                    {
                        /*for (auto &s : current_expression)
                        {
                            std::cout << "Curr exp: ";
                            std::cout << s << std::endl;
                        }*/
                        std::string vname = next_word.substr(1);
                        if (vname.back() == ')')
                        {
                            vname = vname.substr(0, vname.size() - 1);
                        }
                        //std::cout << vname << std::endl;
                        if (std::find(current_expression.begin(), current_expression.end(), vname) != current_expression.end())
                        {
                            next_word.insert(1, func_id);
                        }
                    }
                    if (depth != 0)
                    {
                        nl = (nl + " ") + next_word;
                    }
                }

                func_body += nl + '\n';
            }
            funcs[nfname] = [func_id, func_body, current_expression](std::vector<std::string> args)
            {
                int ind = 0;
                for (auto &s : current_expression) // set up local variables for func body
                {
                    variables[func_id + s] = args[ind];
                    ind += 1;
                }

                std::istringstream strm(func_body);
                std::string line_;
                std::string last = "";
                while (std::getline(strm, line_))
                {
                    std::string s = recurse_and_call_line(line_, strm)();
                    if(s.size() > 0) { last = s; }
                }

                for (auto &s : current_expression) // tear down local variables for func body
                {
                    variables.erase(func_id + s);
                }

                return last; // Return last evaluated value
            };
            if(debug)
            {
                std::cout << func_body << std::endl;
            }
            return [nfname]()
            { return nfname; };
        }
        word = eval_to_literal(word, line_stream, full_stream);
        if (word.front() == '[')
    
        {
            std::vector<std::string> this_array;
            std::string next_word;
            int depth = 1;

            if(word.size() > 1)
            {
                word = word.substr(1);

                word = eval_to_literal(word, line_stream, full_stream);
                this_array.push_back(word);
            }

            while(depth > 0) {
                if(line_stream >> next_word)
                {
                    int oc = std::count(next_word.begin(), next_word.end(), '[');
                    int cc = std::count(next_word.begin(), next_word.end(), ']');
                    depth += oc - cc;

                    if(next_word.find(']') != std::string::npos)
                    {
                        if(depth == 0)
                        {

                            size_t pos = next_word.find(']');
                            if (pos != std::string::npos) {
                                next_word.erase(pos, 1);
                            }
                        }
                    }

                    if(next_word.find('[') != std::string::npos)
                    {
                        if(depth == 0)
                        {
                            size_t pos = next_word.find('[');
                            if (pos != std::string::npos) {
                                next_word.erase(pos, 1);
                            }
                        }
                    }
                    next_word = eval_to_literal(next_word, line_stream, full_stream);
                    if(next_word.size() > 0)
                    {
                        this_array.push_back(next_word);
                    }
                    

                } else if(full_stream >> next_word)
                {
                    int oc = std::count(next_word.begin(), next_word.end(), '[');
                    int cc = std::count(next_word.begin(), next_word.end(), ']');
                    depth += oc - cc;

                    if(next_word.find(']') != std::string::npos)
                    {
                        if(depth == 0)
                        {

                            size_t pos = next_word.find(']');
                            if (pos != std::string::npos) {
                                next_word.erase(pos, 1);
                            }
                        }
                    }

                    if(next_word.find('[') != std::string::npos)
                    {
                        if(depth == 0)
                        {
                            size_t pos = next_word.find('[');
                            if (pos != std::string::npos) {
                                next_word.erase(pos, 1);
                            }
                        }
                    }
                    next_word = eval_to_literal(next_word, line_stream, full_stream);
                    if(next_word.size() > 0)
                    {
                        this_array.push_back(next_word);
                    }
                } else {
                    break;
                }
            }
            std::string vname = current_expression[0];
            
            if(debug) {
                std::cout << "Vname: ";
                std::cout << vname << std::endl;
                for(auto &s : this_array)
                {
                    std::cout << s << std::endl;
                }   
            }
            //Add the array now
            vectors[vname] = this_array;
            //Add the function for it
            funcs[vname] = [vname](std::vector<std::string> args){
                if(args.size() > 0)
                {
                    return vectors[vname][std::stoi(args[0])];
                }else {
                    return vname;
                }
                return std::string("");
            };

            return [vname]()
            { return vname; };
            //word = "success";
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