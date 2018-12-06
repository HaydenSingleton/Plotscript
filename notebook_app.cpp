#include "notebook_app.hpp"

Consumer::Consumer(InputQueue * inq, OutputQueue * outq, Interpreter & inter): cThread() {
    iqueue = inq;
    oqueue = outq;
    cInterp = inter;
}
Consumer::Consumer(){
}
Consumer & Consumer::operator=(Consumer & c) noexcept{
    if(this!=&c){
        iqueue = c.iqueue;
        oqueue = c.oqueue;
        cInterp = c.cInterp;
        cThread.swap(c.cThread);
    }
    return *this;
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
    bool succ;

    std::string line;
    while(isRunning()){

        iqueue->wait_and_pop(line);

        std::istringstream expression(line);
        if(line == "quit" || std::all_of(line.begin(), line.end(),isspace)){
            continue;
        }

        succ = false;
        error = "";
        output_type output;

        if(!cInterp.parseStream(expression)){
            error = "Error: Invalid Expression. Could not parse.";
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
        output = std::make_tuple(result, error, succ);
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
        std::string quit = "quit";
        iqueue->push(quit);
        if(cThread.joinable())
            cThread.join();
        iqueue->try_pop(quit);
    }
}
void Consumer::resetThread(Interpreter *original){
    if(running){
        stopThread();
    }
    cInterp = *original;
    startThread();
}

NotebookApp::NotebookApp(QWidget *parent) : QWidget(parent) {
    setObjectName("notebook");

    std::ifstream startip_str(STARTUP_FILE);
    if(!mrInterpret->parseStream(startip_str)){
        emit send_failure("Error: Invalid Startup Program. Could not parse.");
    }
    else{
        try{
            Expression exp = mrInterpret->evaluate();
        }
        catch(const SemanticError & ex){
            emit send_failure(ex.what());
        }
    }

    default_state = mrInterpret;

    c1 = new Consumer(inputQ, outputQ, *mrInterpret);
    c1->startThread();

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

    timer = new QTimer(this);
    timer->start(100);

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
    QObject::connect(interuptButton, SIGNAL(clicked()), this, SLOT(interupt_kernAl()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(time_ran_out()));
}
void NotebookApp::catch_input(QString s){

    global_status_flag = 0;
    if(c1->isRunning()) {
        inputQ->push(s.toStdString());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    else {
        emit send_failure("Error: Interpreter kernal is not running");
    }
}

void NotebookApp::start_kernal() {
    c1->startThread();
}

void NotebookApp::stop_kernal() {
    c1->stopThread();
}

void NotebookApp::reset_kernal() {
    mrInterpret = default_state;
    c1->resetThread(default_state);
}

void NotebookApp::interupt_kernAl(){
    global_status_flag = 1;
}

void NotebookApp::time_ran_out(){
    output_type results; /// Tuple with { Expression e, std::string ex.what(), bool success }
    if(c1->isRunning() && outputQ->try_pop(results)){
        if(std::get<2>(results)) {
            emit send_result(std::get<0>(results));
        }
        else {
            emit send_failure(std::get<1>(results));
        }
        c1->stopThread();
        c1->startThread();
    }
}