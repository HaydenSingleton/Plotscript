#include "myWidgets.hpp"

NotebookApp::NotebookApp(QWidget *parent){
    in = new InputWidget();
    in->setPlainText("Type code here.");
    out = new OutputWidget();
    resize(400, 500);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(in);
    layout->addWidget(out);

    setLayout(layout);
};

InputWidget::InputWidget(QObject * parent){
    setObjectName("Input");
    setBackgroundVisible(true);
};

OutputWidget::OutputWidget(QWidget * parent){
    setObjectName("Output");
};