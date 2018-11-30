#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "TSmessage.hpp"

typedef TSmessage<std::string> InputQueue;
typedef std::pair<Expression, std::string> output_type;
typedef TSmessage<output_type> OutputQueue;

class Producer {
  private:
    InputQueue * iqueue;
  public:
    Producer(InputQueue * a) : iqueue(a){}
    void operator()(std::string & str) const {
      iqueue->push(str);
    }
};

class Consumer {
  private:
    InputQueue * iqueue;
    OutputQueue * oqueue;
    Interpreter cInterp;
    bool running = false;
    std::thread cThread;
  public:
    Consumer(InputQueue * inq, OutputQueue * outq, Interpreter & inter): cThread() {
      iqueue = inq;
      oqueue = outq;
      cInterp = inter;
    }
    ~Consumer(){
      stopThread();
    }
    void ThreadFunction() {
      while(true){

        std::string line;
        iqueue->wait_and_pop(line);
        if(line==""){
          break;
        }
        std::istringstream expression(line);

        Expression result;
        std::string error;

        if(!cInterp.parseStream(expression)){
          error = "Invalid Expression. Could not parse.";
        }
        else{
          try{
            result = cInterp.evaluate();
          }
          catch(const SemanticError & ex){
            error = ex.what();
          }
        }

        output_type output = std::make_pair(result, error);
        oqueue->push(output);
        // std::cout << "Sent result" << std::endl;
      }
    }
    void change_interpreter(Interpreter & newinter){
      cInterp = newinter;
    }
    void startThread(){
      if(!running){
        running = true;
        cThread = std::thread(&Consumer::ThreadFunction, this);
      }
    }
    void stopThread(){
      if(running){
        if(cThread.joinable()) cThread.join();
        running = false;
      }
    }
    void resetThread(Interpreter & newinter){
      if(running){
        stopThread();
        startThread();
        cInterp = newinter;
      }
      else{
        startThread();
      }
    }
};

void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);
  return line;
}

void error(const std::string & err_str){
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, Interpreter &interp){

  if(!interp.parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
      std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int eval_from_file(std::string filename, Interpreter &interp){

  std::ifstream ifs(filename);

  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }

  return eval_from_stream(ifs, interp);
}

int eval_from_command(std::string argexp, Interpreter &interp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression, interp);
}

// A REPL is a repeated read-eval-print loop
void repl(Interpreter &interp){

  Interpreter default_state = interp;

  InputQueue * input = new InputQueue;
  OutputQueue * output = new OutputQueue;

  Producer p1(input);
  Consumer c1(input, output, interp);
  c1.startThread();

  while(!std::cin.eof()){

    prompt();
    std::string line = readline();

    if(line.empty()) continue;
    if(line == "%stop"){
      c1.stopThread();
      continue;
    }
    else if (line == "%start"){
      c1.startThread();
      continue;
    }
    else if (line == "%reset"){
      c1.resetThread(default_state);
      continue;
    }
    p1(line);
    ////THREAD STUFF HAPPENS
    output_type result;
    output->wait_and_pop(result);

    if(result.second==""){
      std::cout << result.first << std::endl;
    }
    else{
      std::cerr << result.second << std::endl;
    }
  }

  // if(cThread.joinable())
  //   cThread.join();
  delete input;
  delete output;
}

int main(int argc, char *argv[])
{
  Interpreter interp;
  std::ifstream startup_stream(STARTUP_FILE);
  if(!interp.parseStream(startup_stream)){
    error("Error: Invalid Startup Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
    }
    catch(const SemanticError & ex){
      std::cerr << "Start-up failed " << std::endl;
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  if(argc == 2){
    return eval_from_file(argv[1], interp);
  }
  else if(argc == 3){
    if(std::string(argv[1]) == "-e"){
      return eval_from_command(argv[2], interp);
    }
    else{
      error("Incorrect number of command line arguments.");
    }
  }
  else{
    repl(interp);
  }
}
