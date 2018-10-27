#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
    setObjectName("output");
    auto layout = new QHBoxLayout(this);
    layout->addWidget(view);
    view->setScene(scene);
}

void OutputWidget::catch_result(Expression e){

    if(clear_on_print) {
        scene->clear();;
    }

    if(e.isPoint()) {
        std::pair<double, double> coordinates = e.getPointCoordinates();
        double diam = e.getNumericalProperty("\"size\"");
        if(diam < 0){
                catch_failure("Error in make-point call: diameter not positive");
                return;
        }
        QRectF corners = QRectF(0, 0, diam, diam);
        corners.moveCenter(QPointF(coordinates.first, coordinates.second));
        scene->addEllipse(corners, QPen(), QBrush(Qt::SolidPattern));
    }
    else if (e.isLine()) {
        std::vector<Expression> vec = e.asVector();
        Expression p1 = vec[0], p2 = vec[1];
        if(p1.isPoint() && p2.isPoint()){
            std::pair<double, double> X = p1.getPointCoordinates();
            std::pair<double, double> Y = p2.getPointCoordinates();

            QLineF line = QLineF(X.first, Y.first, X.second, Y.second);
            double thicc = e.getNumericalProperty("\"thickness\"");
            if(thicc < 0){
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

        double xcor, ycor; bool isValid;
        std::tie(xcor, ycor, isValid) = e.getPosition();

        if(isValid)
            text->setPos(QPointF(xcor, ycor));
        else {
            catch_failure("Error in make-text: not a valid position point in property list");
            return;
        }
    }
    else if (e.isList()) {
        clear_on_print = false;
        for(auto & e_part : e.asVector()){
            catch_result(e_part);
        }
        clear_on_print = true;
    }
    else if(!e.isLambda()) {
        // Not a special case or user-defined function, display normally
        scene->addText(QString::fromStdString(e.toString()));
    }
}

void OutputWidget::catch_failure(std::string message) {
    scene->clear();
    QString msg = QString::fromStdString(message);
    QGraphicsTextItem * output = new QGraphicsTextItem;
    output->setPos(0,0);
    output->setPlainText(msg);
    scene->addItem(output);
}

void OutputWidget::clear_screen() {
    scene->clear();
}