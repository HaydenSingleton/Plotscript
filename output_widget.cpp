#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
    setObjectName("output");
    auto layout = new QHBoxLayout(this);
    layout->addWidget(view);
    view->setScene(scene);

    // QGraphicsTextItem *welcome = scene->addText("Hello World!");
    // QGraphicsTextItem *centered = scene->addText("(0,0)");

    QGraphicsRectItem *panel = new QGraphicsRectItem();
    scene->addItem(panel);

}

void OutputWidget::clear_screen() {
    scene->clear();
}

void OutputWidget::catch_result(Expression e){

    if(clear_on_print) {
        clear_screen();
    }
    if(e.isPoint()) {
        auto coordinates = e.getPointCoordinates();
        double diam = e.getNumericalProperty("\"size\"");

        QRectF *circle = new QRectF(0, 0, diam, diam);
        circle->moveCenter(QPointF(coordinates.first, coordinates.second));
        scene->addEllipse(*circle, QPen(), QBrush(Qt::SolidPattern));
    }
    else if (e.isLine()) {
        std::vector<Expression> vec = e.asVector();
        Expression p1 = vec[0], p2 = vec[1];
        auto start = p1.getPointCoordinates();
        auto end = p2.getPointCoordinates();
        QPointF start_point = QPointF(start.first, start.second);
        QPointF end_point = QPointF(end.first, end.second);

        QLineF line = QLineF(start_point, end_point);
        double thicc = e.getNumericalProperty("\"thickness\"");
        scene->addLine(line, QPen(QBrush(Qt::SolidPattern), thicc));
    }
    else if (e.isText()) {
        std::string repl = e.toString();
        QString qstr = QString::fromStdString(repl.substr(2, repl.length()-4));
        QGraphicsTextItem *text = scene->addText(qstr);
        auto coordinates = e.getPosition();
        text->setPos(QPointF(coordinates.first, coordinates.second));
    }
    else if (e.isList()) {
        clear_on_print = false;
        for(auto &e_part : e.asVector()){
            catch_result(e_part);
        }
        clear_on_print = true;
    }
    else if(!e.isLambda()) {
        // Not a special case, display normally
        // QGraphicsTextItem *text =
        scene->addText(QString::fromStdString(e.toString()));
    }
}

void OutputWidget::catch_failure(int code, std::string message){
    clear_screen();
    QString msg = QString::fromStdString(message);
    if(code == 0) {
        QGraphicsTextItem * output = new QGraphicsTextItem;
        output->setPos(0,0);
        output->setPlainText(msg);
        scene->addItem(output);
    }
    else {
        emit QCoreApplication::quit();
    }
}