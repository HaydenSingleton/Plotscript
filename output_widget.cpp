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
        if(diam < 0){
                catch_failure("Error in make-point call: diameter not positive");
                return;
        }
        QRectF *circle = new QRectF(0, 0, diam, diam);
        circle->moveCenter(QPointF(coordinates.first, coordinates.second));
        scene->addEllipse(*circle, QPen(), QBrush(Qt::SolidPattern));
    }
    else if (e.isLine()) {
        std::vector<Expression> vec = e.asVector();
        Expression p1 = vec[0], p2 = vec[1];
        if(p1.isPoint() && p2.isPoint()){
            auto start = p1.getPointCoordinates();
            auto end = p2.getPointCoordinates();
            QPointF start_point = QPointF(start.first, start.second);
            QPointF end_point = QPointF(end.first, end.second);

            QLineF line = QLineF(start_point, end_point);
            double thicc = e.getNumericalProperty("\"thickness\"");
            if(!(thicc >= 0)){
                catch_failure("Error in make-line call: thickness value not positive");
                return;
            }
            scene->addLine(line, QPen(QBrush(Qt::SolidPattern), thicc));
        }
        else {
            catch_failure("Error: argument to make-line not a point");
            return;
        }
    }
    else if (e.isText()) {
        std::string repl = e.toString();
        QString qstr = QString::fromStdString(repl.substr(2, repl.length()-4));
        QGraphicsTextItem *text = scene->addText(qstr);

        double xcor, ycor;
        bool isValidPoint;
        std::tie(xcor, ycor, isValidPoint) = e.getPosition();
        if(isValidPoint)
            text->setPos(QPointF(xcor, ycor));
        else {
            catch_failure("Error in make-text: not a valid position in property list");
            return;
        }
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

void OutputWidget::catch_failure(std::string message) {
    clear_screen();
    QString msg = QString::fromStdString(message);
    QGraphicsTextItem * output = new QGraphicsTextItem;
    output->setPos(0,0);
    output->setPlainText(msg);
    scene->addItem(output);
}