#include "output_widget.hpp"
#include <iostream>

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
    setObjectName("output");
    view->setScene(scene);

    scene->addText("Hello World!");
    QGraphicsRectItem *panel = new QGraphicsRectItem(0,0,1440,900);//,(QGraphicsRectItem *)this);
    scene->addItem(panel);
}

void OutputWidget::clear_screen() {
    scene->clear();
}

void OutputWidget::catch_result(Expression e){
    clear_screen();
    if(e.isNone() || e.isList()){ // Not a special case, display normally
        QGraphicsTextItem *text = scene->addText(QString::fromStdString(e.toString()));
        view->update();
    }
}

void OutputWidget::catch_failure(int code, std::string message){
    clear_screen();
    QString msg = QString::fromStdString(message);
    if(code == 0) {
        QGraphicsTextItem * output = new QGraphicsTextItem;
        output->setPos(0,10);
        output->setPlainText(msg);
        scene->addItem(output);
        view->update();
    }
    else {
        emit QCoreApplication::quit();
    }
}