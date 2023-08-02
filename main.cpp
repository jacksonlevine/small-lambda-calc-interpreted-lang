#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <functional>

enum OutType {
  ERR_OUT,
  RES_OUT
};

class Out {
public:
  OutType type;
  std::string value;
};

class FunctionCall
{
public:
  std::string func;
  std::vector<std::string> args;
};

std::map<std::string, std::function<Out(std::vector<std::string>, int)>> funcs;

std::map<std::string, std::string> variables;

FunctionCall from_line(std::string line)
{
  std::string word;
  std::istringstream line_stream(line);
  int index = 0;

  FunctionCall func;
  while(line_stream >> word)
  {
    if(index == 0)
    {
      func.func = word;
    } else {
      func.args.push_back(word);
    }
    index++;
  }
  return func;
}


std::string get_result_from_func(std::string line) //does side effect too
{
   std::istringstream lineStream(line);
          std::string word;
          int index = 0;
          FunctionCall currFuncCall;
          while (lineStream >> word) {
              
              if(word.front() == '(')
              {
                std::string inner_line = "";
                inner_line += word;
                bool closed = false;
                while(!closed)
                {
                  lineStream >> word;
                  if(word.back() == ')')
                  {
                    closed = true;
                  }
                  inner_line += " ";

                  inner_line += word;
                }

                inner_line = inner_line.substr(1, inner_line.size()-2); //remove parenthesese
                std::cout << "Evaluating nested expression first: ";
                std::cout << inner_line << std::endl;
                word = get_result_from_func(inner_line);
                  
              }


              if(index == 0) //then this is the func name
              {
                
                currFuncCall.func = word;
              } else {      //ARGS
                currFuncCall.args.push_back(word);
              }

              index += 1;
          }
          if(currFuncCall.func.front() == '.')
          {
            currFuncCall.func = variables[currFuncCall.func.substr(1)];
          }
          for(int i = 0; i < currFuncCall.args.size(); i++)
          {
            if(currFuncCall.args[i].front() == '.') {
              currFuncCall.args[i] = variables[currFuncCall.args[i].substr(1)];
            }
          }

          Out result = funcs[currFuncCall.func](currFuncCall.args, currFuncCall.args.size());
          if(result.type == RES_OUT)
          {
            return result.value;
          } else {
            std::cerr << "Error in this line: ";
            std::cerr << line << std::endl;
            return std::string("error");
          }
          //std::cout << std::endl;
}



void interpret_file(const char* filename) {
  std::ifstream file(filename);
  std::stringstream buf;
  buf << file.rdbuf();

  std::string code = buf.str();

  std::istringstream stream(code);
      std::string line;
      while (std::getline(stream, line)) {
          get_result_from_func(line);
      }
  
  
}


int main() {

    
    funcs["l"] = [](std::vector<std::string> args, int size){
      Out result;
      result.type = RES_OUT;
      result.value = "success";

      if(size != 2)
      {
        result.type = ERR_OUT;
        result.value = std::string("\'let\' takes 2 arguments. You provided ") + std::to_string(size);
        return result;
      }

      std::string var = args[0];
      std::string val = args[1];

      if(variables.find(var) != variables.end())
      {
        result.type = ERR_OUT;
        result.value = (std::string("Attempted to define variable ") + var) + " more than once.";
        return result;
      }

      variables[var] = val;
      return result;
    };
    funcs["s"] = [](std::vector<std::string> args, int size){
      Out result;
      result.type = RES_OUT;
      result.value = "success";

      if(size != 2)
      {
        result.type = ERR_OUT;
        result.value = std::string("\'set\' takes 2 arguments. You provided ") + std::to_string(size);
        return result;
      }

      std::string var = args[0];
      std::string val = args[1];

      if(variables.find(var) != variables.end())
      {
        variables[var] = val;
      } else {
        result.type = ERR_OUT;
        result.value = (std::string("Attempted to set non-existent variable ") + var) + ".";
        return result;
      }

      return result;
    };
    funcs["p"] = [](std::vector<std::string> args, int size){

      Out result;
      result.type = RES_OUT;
      result.value = "success";

      for(auto& print_this : args) {
        std::cout << print_this << std::endl;
      }
      return result;
    };
    funcs["let"] = funcs["l"];
    funcs["set"] = funcs["s"];
    funcs["print"] = funcs["p"];
    funcs["+"] = [](std::vector<std::string> args, int size){
      bool any_floats = false;
      for(auto& s : args)
      {
        if(s.find('.') != std::string::npos)
        {
          any_floats = true;
        }
      }

      Out result;
      result.type = RES_OUT;
      float out = 0.0f;
      int out2 = 0;
      result.value = "error";
      switch(any_floats)
      {
        case true:


          for(auto &s : args)
          {
            out += std::stof(s);
          }
          result.value = std::to_string(out);
          return result;

        case false:


          for(auto &s : args){
            out2 += std::stoi(s);
          }

          result.value = std::to_string(out2);
          return result;
      }
   
    };
    funcs["concat"] = [](std::vector<std::string> args, int size){
      Out result;
      result.type = RES_OUT;
      std::string val;
      for(auto& s : args) {
        val += s;
      }
      result.value = val;
      return result;
    };
    funcs["ct"] = funcs["concat"];


  interpret_file("./othertest.y");


  



  std::string _;
  std::cin >> _;
  return 1;
}


