#include "notebook_app.hpp"

#include <csignal>
#include <cstdlib>

volatile sig_atomic_t global_status_flag = 0;
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

Consumer::Consumer(InputQueue * inq, OutputQueue * outq, Interpreter & inter): cThread() {
    iqueue = inq;
    oqueue = outq;
    cInterp = inter;
}
Consumer::Consumer(){
}
Consumer & Consumer::operator=(Consumer & c){
    iqueue = c.iqueue;
    oqueue = c.oqueue;
    cInterp = c.cInterp;
    cThread.swap(c.cThread);
}
Consumer::~Consumer(){
    stopThread();
}
Consumer::Consumer(Consumer & c){
    iqueue = c.iqueue;
    oqueue = c.oqueue;
    cInterp = c.cInterp;
    cThread.swap(c.cThread);
}
void Consumer::ThreadFunction() {
    Expression result;
    std::string error;
    std::string line;
    while(isRunning()){

        if(global_status_flag > 0){
            error = "Error: interpreter kernel interrupted";
            output_type output = std::make_pair(result, error);
            oqueue->push(output);
        }
        if(!iqueue->try_pop(line)){
            continue;
        }
        std::istringstream expression(line);
        if(line == "")
            continue;

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
    }
}
bool Consumer::isRunning(){
    return running;
}
void Consumer::startThread(){
    if(!running){
    running = true;
    cThread = std::thread(&Consumer::ThreadFunction, this);
    }
}
void Consumer::stopThread(){
    if(running){
        running = false;
        std::string empty;
        iqueue->push(empty);
        cThread.join();
        iqueue->try_pop(empty);
    }
}
void Consumer::resetThread(Interpreter & newinter){
    if(running){
        stopThread();
    }
    cInterp = newinter;
    startThread();
}

NotebookApp::NotebookApp(QWidget *parent) : QWidget(parent) {
    setObjectName("notebook");

    std::ifstream startip_str(STARTUP_FILE);
    if(!mrInterpret.parseStream(startip_str)){
        emit send_failure("Error: Invalid Startup Program. Could not parse.");
    }
    else{
        try{
            Expression exp = mrInterpret.evaluate();
        }
        catch(const SemanticError & ex){
            emit send_failure(ex.what());
        }
    }
    default_state = mrInterpret;
    c1 = *(new Consumer(inputQ, outputQ, mrInterpret));
    c1.startThread();

    in = new InputWidget(this); //child widgets of notebook
    out = new OutputWidget(this);

    auto row = new QHBoxLayout();
    auto startButton = new QPushButton(QString("Start Kernal"), this);
    startButton->setObjectName("start");
    row->addWidget(startButton);
    auto stopButton = new QPushButton(QString("Stop Kernal"), this);
    stopButton->setObjectName("stop");
    row->addWidget(stopButton);
    auto resetButton = new QPushButton(QString("Reset Kernal"), this);
    resetButton->setObjectName("reset");
    row->addWidget(resetButton);
    auto interuptButton = new QPushButton(QString("Interrupt"), this);
    interuptButton->setObjectName("interrupt");
    row->addWidget(interuptButton);

    auto layout = new QVBoxLayout(this); // add the widgets to a vertical layout
    layout->addLayout(row);
    layout->addWidget(in, 1);
    layout->addWidget(out, 1);
    setLayout(layout);

    // connect input to this notebook for evaluating
    QObject::connect(in, SIGNAL(send_input(QString)), this, SLOT(catch_input(QString)));

    // sent evaluated expressions to output widget
    QObject::connect(this, SIGNAL(send_result(Expression)), out, SLOT(catch_result(Expression)));

    // send any errors to output widget with a different mechanism
    QObject::connect(this, SIGNAL(send_failure(std::string)), out, SLOT(catch_failure(std::string)));

    // send the clear signal to the output
    QObject::connect(in, SIGNAL(clear_output()), out, SLOT(clear_screen()));

    // connect buttons to functions in this class
    QObject::connect(startButton, SIGNAL(clicked()), this, SLOT(start_kernal()));
    QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(stop_kernal()));
    QObject::connect(resetButton, SIGNAL(clicked()), this, SLOT(reset_kernal()));
    // QObject::connect(resetButton, SIGNAL(clicked()), out, SLOT(clear_screen()));
    QObject::connect(interuptButton, SIGNAL(clicked()), this, SLOT(interrupt_kernal()));
}

void NotebookApp::catch_input(QString s){
    std::string errorMessage;
    Expression result;
    output_type results;

    if(c1.isRunning()){
        inputQ->push(s.toStdString());
        in->setReadOnly(true);
        outputQ->wait_and_pop(results);
        in->setReadOnly(false);

        if(results.second=="")
        {
            emit send_result(results.first);
        }
        else
        {
            emit send_failure(results.second);
        }
    }
    else
        emit send_failure("Error: Interpreter kernal is not running");
}

void NotebookApp::start_kernal() {
    c1.startThread();
}

void NotebookApp::stop_kernal() {
    c1.stopThread();
}

void NotebookApp::reset_kernal() {
    c1.resetThread(default_state);
}

void NotebookApp::interrupt_kernal() {
    emit send_failure("Error: Interpreter kernal is not running");
}