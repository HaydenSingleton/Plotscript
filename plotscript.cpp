#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cassert>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "TSmessage.hpp"

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {
  switch (fdwCtrlType) {
  case CTRL_C_EVENT:
    if (global_status_flag > 0) {
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
    return TRUE;

  default:
    return FALSE;
  }
}

inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }

#elif defined(__APPLE__) || defined(__linux) || defined(__unix) || defined(__posix)
#include <unistd.h>

void interrupt_handler(int signal_num) {
  if(signal_num == SIGINT){
    if (global_status_flag > 0) {
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
  }
}

inline void install_handler() {

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = interrupt_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif

typedef TSmessage<std::string> InputQueue;
typedef std::tuple<Expression, std::string, bool> output_type;
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
    Consumer(InputQueue * inq, OutputQueue * outq, Interpreter & inter) {
      iqueue = inq;
      oqueue = outq;
      cInterp = inter;
    }
    ~Consumer(){
      if(cThread.joinable()) {
        std::string temp;
        iqueue->push(temp);
        cThread.join();
      }
    }
    void ThreadFunction() {
      bool succ;
      while(isRunning() && global_status_flag == 0){
        std::string line;
        succ = false;

        if(!iqueue->try_pop(line)){
          continue;
        }
        std::istringstream expression(line);

        if(line == "")
          continue;

        Expression result;
        std::string error;

        if(!cInterp.parseStream(expression)){
          error = "Invalid Expression. Could not parse.";
        }
        else{
          try{
            result = cInterp.evaluate();
            succ = true;
          }
          catch(const SemanticError & ex){
            error = ex.what();
          }
        }

        output_type output = std::make_tuple(result, error, succ);
        oqueue->push(output);
      }
    }
    bool isRunning(){
      return running;
    }
    void startThread(){
      if(!running){
        running = true;
        cThread = std::thread(&Consumer::ThreadFunction, this);
      }
    }
    void stopThread(){
      if(running){
        running = false;
        std::string empty;
        iqueue->push(empty);
        cThread.join();
        iqueue->try_pop(empty);
      }
    }
    void resetThread(Interpreter & newinter){
      if(running){
        stopThread();
      }
      cInterp = newinter;
      startThread();
    }
};

void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);
  if (std::cin.fail() || std::cin.eof()) {
    std::cin.clear(); // reset cin state
    line.clear(); //clear input string
  }
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

  Interpreter copy, default_state = interp;

  InputQueue * input = new InputQueue;
  OutputQueue * output = new OutputQueue;

  Producer p1(input);
  Consumer c1(input, output, interp);
  c1.startThread();

  while(!std::cin.eof()){

    if(global_status_flag > 0){
      input->clear();
      output->clear();
    }

    global_status_flag = 0;

    copy = interp;

    prompt();
    std::string line = readline();
    output_type result;

    if(line.empty()) continue;
    if(line == "%stop"){
      c1.stopThread();
    }
    else if (line == "%start"){
      c1.startThread();
    }
    else if (line == "%reset"){
      c1.resetThread(default_state);
    }
    else if (line == "%exit"){
      c1.stopThread();
      exit(EXIT_SUCCESS);
    }
    else if(!c1.isRunning()){
      std::cerr << "Error: interpreter kernel not running" << std::endl;
    }
    else {
      p1(line);

      while(output->empty()){
      // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (global_status_flag > 0) {
          std::cerr << "\nError: interpreter kernel interrupted [1]\n";
          // c1.resetThread(copy);
          break;
        }
      }
      //If the operation was NOT interupted, get the result. OTHERWISE prompt repl again
      if(global_status_flag == 0){
        output->try_pop(result);
        if(std::get<2>(result)) {
            std::cout << std::get<0>(result) << std::endl;
        }
        else {
            std::cerr << std::get<1>(result) << std::endl;
        }
      }
    }
  }
  delete input;
  delete output;
}

int main(int argc, char *argv[])
{
  install_handler();

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
