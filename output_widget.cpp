#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
    setObjectName("output");
    auto layout = new QHBoxLayout(this);
    layout->addWidget(view);
    view->setScene(scene);
}

void OutputWidget::catch_result(Expression e){
    if(e.isPoint()) {
        std::pair<double, double> coordinates = e.getPointCoordinates();
        double diam = e.getNumericalProperty("\"size\"");
        if(diam < 0){
            catch_failure("Error in make-point call: diameter not positive");
            return;
        }
        QRectF corners = QRectF(coordinates.first, coordinates.second, diam, diam);
        corners.moveCenter(QPointF(coordinates.first, coordinates.second));
        scene->addEllipse(corners, QPen(), QBrush(Qt::SolidPattern));
    }
    else if (e.isLine()) {
        std::vector<Expression> vec = e.asVector();
        Expression p1 = vec[0], p2 = vec[1];
        if(p1.isPoint() && p2.isPoint()){
            std::pair<double, double> X = p1.getPointCoordinates();
            std::pair<double, double> Y = p2.getPointCoordinates();

            QLineF line = QLineF(QPointF(X.first, X.second), QPointF(Y.first, Y.second));

            double thicc = e.getNumericalProperty("\"thickness\"");
            if(thicc < 0){
                catch_failure("Error in make-line call: thickness value not positive");
                return;
            }

            scene->addLine(line, QPen(QBrush(Qt::SolidLine), thicc));

        }
        else {
            catch_failure("Error: argument to make-line not a point");
            return;
        }
    }
    else if (e.isText()) {

        auto font = QFont("Monospace");
        font.setStyleHint(QFont::TypeWriter);
        font.setPointSize(1);

        std::string repl = e.toString();
        QString qstr = QString::fromStdString(repl.substr(2, repl.length()-4));
        QGraphicsTextItem *text = scene->addText(qstr);

        text->setFont(font);

        double xcor, ycor, scaleFactor, rotationAngle; bool isValid;
        std::tie(xcor, ycor, scaleFactor, rotationAngle, isValid) = e.getTextProperties();

        rotationAngle = rotationAngle * 180 / M_PI;

        text->setScale(scaleFactor);
        text->setRotation(rotationAngle);

        if(isValid) {
            QRectF rect = text->sceneBoundingRect();
            QPointF text_center = QPointF(xcor - rect.width()/2, ycor - rect.height()/2);
            text->setPos(text_center);
        }
        else {
            catch_failure("Error in make-text: not a valid property in list for make-text");
            return;
        }

    }
    else if (e.isList()) {
        for(auto & e_part : e.asVector()){
            catch_result(e_part);
        }
    }
    else if (e.isDP()) {

        double N = 20, A = 3, B = 3, C = 2, D = 2, P = 0.5;

        std::vector<Expression> data = e.asVector();

        // Draw bounding box
        for(size_t i = 0; i < 10; i++){
            data[i].setLineThickness(0);
            catch_result(data[i]);
        }

        // Draw data points
        std::string temp; double x, y;
        for(size_t i = 10; i < data.size()-3; i += 2){
            temp = data[i].head().asString();
            temp = temp.substr(1, temp.size()-2);
            std::cout << temp << "\n";
            x = std::stod(temp);
            temp = data[i+1].head().asString();
            temp = temp.substr(1, temp.size()-2);
            y = std::stod(temp);

            QLineF line = QLineF(QPointF(x,0), QPointF(x,y));
            scene->addLine(line, QPen(QBrush(Qt::SolidLine), 1));

        }


    }
    else if(!e.isLambda()) {
        // Not a special case or user-defined function, display normally
        scene->addText(QString::fromStdString(e.toString()));
    }
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(scene->itemsBoundingRect());
}

void OutputWidget::catch_failure(std::string message) {
    QString msg = QString::fromStdString(message);
    QGraphicsTextItem * output = new QGraphicsTextItem;
    output->setPos(0,0);
    output->setPlainText(msg);
    scene->addItem(output);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(scene->itemsBoundingRect());
}

void OutputWidget::clear_screen() {
    scene->clear();
}

void OutputWidget::resizeEvent(QResizeEvent *event) {
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(scene->itemsBoundingRect());
    QWidget::resizeEvent(event);
}
