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

//#define DEBUGGING
// #define PARENTHESES_DEBUGGING
//#define FUNCTION_DEBUGGING
// #define IF_STATEMENT_DEBUGGING

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
    funcs[">"] = [](std::vector<std::string> args)
    {
        std::string result = "false";
        bool floats = false;
        for (auto &s : args)
        {
            if (s.find('.') != std::string::npos)
                floats = true;
        }
        if (floats)
        {
            result = std::stof(args[0]) > std::stof(args[1]) ? "true" : "false";
        }
        else
        {
            result = std::stoi(args[0]) > std::stoi(args[1]) ? "true" : "false";
        }
        return result;
    };
    funcs["repeat"] = [](std::vector<std::string> args)
    {
        int count = std::stoi(args[0]);
        std::string funcname = args[1];
        for(int i = 0; i < count; i++)
        {
            std::vector<std::string> _;
            funcs[funcname](_);
        }
        return std::string("success");
    };
    funcs["loop"] = funcs["repeat"];
    funcs["<"] = [](std::vector<std::string> args)
    {
        std::string result = "false";
        bool floats = false;
        for (auto &s : args)
        {
            if (s.find('.') != std::string::npos)
                floats = true;
        }
        if (floats)
        {
            result = std::stof(args[0]) < std::stof(args[1]) ? "true" : "false";
        }
        else
        {
            result = std::stoi(args[0]) < std::stoi(args[1]) ? "true" : "false";
        }
        return result;
    };
    funcs["return"] = funcs["concat"]; // because with 1 arg, it means "evaluate to this", and we return last eval
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
        return variables[args[0]];
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
        for (auto &s : vectors[vname])
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
    funcs["/"] = [](std::vector<std::string> args) {
        bool floats = false;
        for (auto &s : args)
        {
            if (s.find('.') != std::string::npos)
                floats = true;
        }
        if (floats)
        {
            return std::to_string(std::stof(args[0]) / std::stof(args[1]));
        }
        else
        {
            return std::to_string(std::stoi(args[0]) / std::stoi(args[1]));
        }
        return std::string("");
    };
    funcs["%"] = [](std::vector<std::string> args) {
        bool floats = false;
        for (auto &s : args)
        {
            if (s.find('.') != std::string::npos)
                floats = true;
        }
        if (floats)
        {
            return std::to_string((int)std::stof(args[0]) % (int)std::stof(args[1]));
        }
        else
        {
            return std::to_string(std::stoi(args[0]) % std::stoi(args[1]));
        }
        return std::string("");
    };
    funcs["not"] = [](std::vector<std::string> args) {
        std::string result = "";
        if(args[0] == "0" || args[0] == "false")
        {
            result = "true";
        } else {
            result = "false";
        }
        return result;
    };
    funcs["!"] = funcs["not"];
}
std::function<std::string(void)> recurse_and_call_line(std::string line, std::istringstream &full_stream);
std::string eval_parentheses(std::string &word, std::istringstream &line_stream, std::istringstream &full_stream)
{
    int depth = std::count(word.begin(), word.end(), '(');
    std::string inner_line = "";
    inner_line += word;
    while (depth != 0)
    {
        if (line_stream >> word)
        {
        }
        else
        {
            full_stream >> word;
        }
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
#ifdef PARENTHESES_DEBUGGING
    std::cout << "INNER LINE: " << inner_line << std::endl;
#endif
    inner_line = inner_line.substr(1, inner_line.size() - 2);

    word = recurse_and_call_line(inner_line, full_stream)();

#ifdef PARENTHESES_DEBUGGING
    std::cout << "EVAL WORD FROM LINE IS: \"" << word << "\"." << std::endl;
#endif

    if (funcs.find(word.substr(0, word.find(' '))) != funcs.end())
    {

        word = recurse_and_call_line(word, full_stream)();
    }

    return word;
}
std::string eval_quotes(std::string word, std::istringstream &line_stream)
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

std::string eval_to_literal(std::string word, std::istringstream &line_stream, std::istringstream &full_stream)
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
std::string collect_raw_brace_innards(std::istringstream &line_stream, std::istringstream &full_stream)
{

    std::string func_body = "";

    std::string next_line;
    int depth = 1;
    while (depth != 0)
    {
        if (std::getline(line_stream, next_line))
        {
        }
        else if (std::getline(full_stream, next_line))
        {
        }
        std::istringstream func_stream(next_line);
        std::string next_word;
        std::string nl = "";
        while (func_stream >> next_word && depth != 0)
        {
            int lastbit = 0;
            std::string corrected_last_word = "";
            if (next_word.find('>') != std::string::npos)
            {
                depth -= std::count(next_word.begin(), next_word.end(), '>');
                corrected_last_word = next_word.substr(0, next_word.find('>'));
                lastbit = 1;
            }
            if (next_word.find('<') != std::string::npos)
            {
                depth += std::count(next_word.begin(), next_word.end(), '<');
            }

            if (depth == 0)
            {
                next_word.erase(std::remove(next_word.begin(), next_word.end(), '>'), next_word.end());
                next_word.erase(std::remove(next_word.begin(), next_word.end(), '<'), next_word.end());
                next_word.erase(std::remove(next_word.begin(), next_word.end(), '\n'), next_word.end());
            }

            if (next_word.size() > 0)
            {
                nl = (nl + " ") + next_word;
            }
        }
        if (nl.size() > 0)
        {
            func_body += nl + '\n';
        }
    }

    return func_body;
}

std::function<std::string()> func_from_block_body(std::string func_body)
{
    return [func_body]()
    {
        std::istringstream strm(func_body);
        std::string line_;
        std::string last = "";
        while (std::getline(strm, line_))
        {
            std::string s = recurse_and_call_line(line_, strm)();
            if (s.size() > 0)
            {
                last = s;
            }
        }

        return last; // Return last evaluated value, or empty string
    };
}

int count_substr_occurrences(const std::string& text, const std::string& substring) {
    int count = 0;
    size_t pos = 0;

    while ((pos = text.find(substring, pos)) != std::string::npos) {
        ++count;
        pos += substring.length();
    }

    return count;
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
        if (word.find("if") != std::string::npos)
        {
#ifdef IF_STATEMENT_DEBUGGING
            std::cout << "IF STATEMENT DETECTED: CURRENT WORD: " << word << std::endl;
#endif
            std::string exp = "";
            std::string w;

            if (word.find('(') != std::string::npos)
            {
                exp += "( ";
            }
            bool block_start_found = false;
            while (!block_start_found)
            {
                if (line_stream >> w) // Take from either line or full till found
                {
                }
                else if (full_stream >> w)
                {
                }

                if (w.find('<') == std::string::npos)
                {
                    (exp += " ") += w;
                }
                else
                {
                    std::string w2(w);
                    w2.erase(std::remove(w2.begin(), w2.end(), '<'), w2.end()); // If the paren and brace touch, remove brace here
                    (exp += " ") += w2;
                    block_start_found = true; // exit the while
                }
            }
#ifdef IF_STATEMENT_DEBUGGING
            std::cout << "EXP TO EVALUATE: " << exp << std::endl;
#endif
            // Got the expression in exp. Collect brace innards away from rest of parser...
            // collect_raw_brace_innards goes in with an assumed starting depth of 1 so we dont need the first brace here
            std::string block_body = collect_raw_brace_innards(line_stream, full_stream);
#ifdef IF_STATEMENT_DEBUGGING
            std::cout << "BLOCK BODY: " << std::endl
                      << block_body << std::endl;
#endif
            // Evaluate exp:
            std::string res = recurse_and_call_line(exp, full_stream)();
#ifdef IF_STATEMENT_DEBUGGING
            std::cout << "RESULT OF EVALUATING EXPRESSION: " << res << std::endl;
#endif

            if (res != "0" && res != "false")
            {
                // if the statement evaluates to true then the block is executed as normal code wherever it is
                word = func_from_block_body(block_body)();
#ifdef IF_STATEMENT_DEBUGGING
                std::cout << "RESULT OF EVALUATING BLOCK: " << word << std::endl;
#endif
            }
            else
            {
                word = "";
            }
        }

        if (word.find('{') != std::string::npos)
        {
            // std::cout << word;
            // Then the whole expression so far has been a function declaration, we need to get all the lines

            std::string func_id = generate_unique_id();
            std::string nfname(current_expression[0]);
            current_expression.erase(current_expression.begin());
            std::string func_body = "";

            std::string additional_args = "";
            std::string next_line;
            int depth = 1;
            while (depth != 0)
            {
                if (std::getline(line_stream, next_line))
                {
#ifdef DEBUGGING
                    std::cout << "Using rest of line to start this function off:" << next_line << std::endl;
#endif
                }
                else if (std::getline(full_stream, next_line))
                {
                }
                std::istringstream func_stream(next_line);
                std::string next_word;
                std::string nl = "";
                while (func_stream >> next_word && depth != 0)
                {
                    int lastbit = 0;
                    std::string corrected_last_word = "";



                    if (next_word.find('}') != std::string::npos)
                    {
                        depth -= std::count(next_word.begin(), next_word.end(), '}');
                        corrected_last_word = next_word.substr(0, next_word.find('}'));
                        lastbit = 1;
                    }
                    if (next_word.find('{') != std::string::npos)
                    {
                        depth += std::count(next_word.begin(), next_word.end(), '{');
                    }
                    if (depth == 0)
                    {
                        next_word.erase(std::remove(next_word.begin(), next_word.end(), '}'), next_word.end());
                        next_word.erase(std::remove(next_word.begin(), next_word.end(), '{'), next_word.end());
                        next_word.erase(std::remove(next_word.begin(), next_word.end(), '\n'), next_word.end());
                    }
                    if (next_word.front() == '.' && (depth > 0 || lastbit)) // We need to handle local variables differently
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
                        // std::cout << vname << std::endl;
                        if (std::find(current_expression.begin(), current_expression.end(), vname) != current_expression.end() && depth == 1)
                        {
                            next_word.insert(1, func_id);
                        }
                    }

                    if (next_word.size() > 0)
                    {
                        nl = (nl + " ") + next_word;
                    }
                    if (depth == 0)
                    {
                        int args_count = 0;
                        while (func_stream >> next_word && args_count < current_expression.size())
                        {
                            (additional_args += " ") += next_word;
                            args_count += 1;
                        }
                        if (args_count = current_expression.size())
                        {
                            std::string remainder = "";
                            while (func_stream >> next_word)
                            {
                                (remainder += " ") += next_word;
                            }
                            recurse_and_call_line(remainder, full_stream);
                        }
                    }
                }
                if (nl.size() > 0)
                {
                    func_body += nl + '\n';
                }
            }

#ifdef PARENTHESES_DEBUGGING
            std::cout << "ADDITIONAL ARGS: " << additional_args << std::endl;
#endif
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
                    if (s.size() > 0)
                    {
                        last = s;
                    }
                }

                for (auto &s : current_expression) // tear down local variables for func body
                {
                    variables.erase(func_id + s);
                }

                return last; // Return last evaluated value
            };
#ifdef DEBUGGING
            std::cout << "Function body definition:" << std::endl;
            std::cout << func_body << "(END)" << std::endl;
#endif
            return [nfname, additional_args]()
            { return nfname + additional_args; };
        }
        word = eval_to_literal(word, line_stream, full_stream);
        if (word.front() == '[')

        {
            std::vector<std::string> this_array;
            std::string next_word;
            int depth = 1;
            if (word.back() == ']')
            {
                depth = 0;
            }
            else
            {
                if (word.size() > 1)
                {
                    word = word.substr(1);

                    word = eval_to_literal(word, line_stream, full_stream);
                    this_array.push_back(word);
                }
            }

            while (depth > 0)
            {
                if (line_stream >> next_word)
                {
                    int oc = std::count(next_word.begin(), next_word.end(), '[');
                    int cc = std::count(next_word.begin(), next_word.end(), ']');
                    depth += oc - cc;

                    if (next_word.find(']') != std::string::npos)
                    {
                        if (depth == 0)
                        {

                            size_t pos = next_word.find(']');
                            if (pos != std::string::npos)
                            {
                                next_word.erase(pos, 1);
                            }
                        }
                    }

                    if (next_word.find('[') != std::string::npos)
                    {
                        if (depth == 0)
                        {
                            size_t pos = next_word.find('[');
                            if (pos != std::string::npos)
                            {
                                next_word.erase(pos, 1);
                            }
                        }
                    }
                    next_word = eval_to_literal(next_word, line_stream, full_stream);
                    if (next_word.size() > 0)
                    {
                        this_array.push_back(next_word);
                    }
                }
                else if (full_stream >> next_word)
                {
                    int oc = std::count(next_word.begin(), next_word.end(), '[');
                    int cc = std::count(next_word.begin(), next_word.end(), ']');
                    depth += oc - cc;

                    if (next_word.find(']') != std::string::npos)
                    {
                        if (depth == 0)
                        {

                            size_t pos = next_word.find(']');
                            if (pos != std::string::npos)
                            {
                                next_word.erase(pos, 1);
                            }
                        }
                    }

                    if (next_word.find('[') != std::string::npos)
                    {
                        if (depth == 0)
                        {
                            size_t pos = next_word.find('[');
                            if (pos != std::string::npos)
                            {
                                next_word.erase(pos, 1);
                            }
                        }
                    }
                    next_word = eval_to_literal(next_word, line_stream, full_stream);
                    if (next_word.size() > 0)
                    {
                        this_array.push_back(next_word);
                    }
                }
                else
                {
                    break;
                }
            }
            std::string vname = current_expression[0];

#ifdef DEBUGGING

            std::cout << "Vname: ";
            std::cout << vname << std::endl;
            for (auto &s : this_array)
            {
                std::cout << s << std::endl;
            }
#endif
            // Add the array now
            vectors[vname] = this_array;
            // Add the function for it
            funcs[vname] = [vname](std::vector<std::string> args)
            {
                if (args.size() > 0)
                {
                    return vectors[vname][std::stoi(args[0])];
                }
                else
                {
                    return vname;
                }
                return std::string("");
            };

            return [vname]()
            { return vname; };
            // word = "success";
        }
        current_expression.push_back(word);
    }
    std::string fname(current_expression[0]);
    current_expression.erase(current_expression.begin());

    if (funcs.find(fname) != funcs.end())
    {
#ifdef FUNCTION_DEBUGGING
        std::cout << "CALLING FUNCTION: " << fname << " ";
        for (auto &s : current_expression)
        {
            std::cout << s << " ";
        }
        std::cout << std::endl;
#endif
        return [fname, current_expression]()
        { return funcs[fname](current_expression); };
    }
    else
    {
        return [fname]()
        { return fname; };
    }
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