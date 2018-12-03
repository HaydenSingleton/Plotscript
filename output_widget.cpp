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
        drawPoint(coordinates.first, coordinates.second, diam);
    }
    else if (e.isLine()) {
        std::vector<Expression> vec = e.asVector();
        Expression p1 = vec[0], p2 = vec[1];
        if(p1.isPoint() && p2.isPoint()){
            std::pair<double, double> X = p1.getPointCoordinates();
            std::pair<double, double> Y = p2.getPointCoordinates();
            double thicc = e.getNumericalProperty("\"thickness\"");
            if(thicc < 0){
                catch_failure("Error in make-line call: thickness value not positive");
                return;
            }
            drawLine(X.first, X.second, Y.first, Y.second, thicc);
        }
        else {
            catch_failure("Error: argument to make-line not a point");
            return;
        }
    }
    else if (e.isText()) {

        std::string repl = e.toString();
        std::string text_string = repl.substr(2, repl.length()-4);
        double xcor, ycor, scaleFactor, rotationAngle; bool isValid;
        std::tie(xcor, ycor, scaleFactor, rotationAngle, isValid) = e.getTextProperties();
        rotationAngle = rotationAngle * 180 / M_PI;
        if(isValid) {
            drawText(QString::fromStdString(text_string), scaleFactor, rotationAngle, xcor, ycor);
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
        size_t pos = 0;
        for(auto & item : data){

            if(item.isLine()){
                item.setLineThickness(0);
            }
            else if (item.isPoint()){
                item.setPointSize(P);
            }
            else {
                break;
            }
            catch_result(item);
            pos++;
        }

        // Draw AL AU OL OU
        std::string AL_s, AU_s, OL_s, OU_s;
        AL_s = removeQuotes(data[pos++].head().asString());
        AU_s = removeQuotes(data[pos++].head().asString());
        OL_s = removeQuotes(data[pos++].head().asString());
        OU_s = removeQuotes(data[pos++].head().asString());
        double AL = std::stod(AL_s);
        double AU = std::stod(AU_s);
        double OL = std::stod(OL_s);
        double OU = std::stod(OU_s);
        double xscale = N/(AU-AL), yscale = N/(OU-OL);
        AL *= xscale;
        AU *= xscale;
        OL *= yscale * -1;
        OU *= yscale * -1;
        double AM = (AU+AL)/2, OM = (OU+OL)/2;
        // std::cout << "Xmin, Xmax: " << AL << " " << AU << "\nYmin, max: "
        // << OL << ", " << OU << "\nXmid, Ymid: " << AM << " " << OM << "\n\n";

        drawText(QString::fromStdString(AL_s), 1, 0, AL, OL+C);
        drawText(QString::fromStdString(AU_s), 1, 0, AU, OL+C);
        drawText(QString::fromStdString(OL_s), 1, 0, AL-D, OL);
        drawText(QString::fromStdString(OU_s), 1, 0, AL-D, OU);

        // Graph options
        std::string title = removeQuotes(data[pos++].head().asString());
        std::string a_label = removeQuotes(data[pos++].head().asString());
        std::string o_label = removeQuotes(data[pos++].head().asString());

        double textscale;
        if(data.size() > pos)
            textscale = data[pos++].head().asNumber();
        else
            textscale = 1.0;

        // Add the graph labels
        drawText(QString::fromStdString(title), textscale, 0, AM, (OU-A));
        drawText(QString::fromStdString(a_label), textscale, 0, AM, (OL+A));
        drawText(QString::fromStdString(o_label), textscale, -90, (AL-B), OM);

    }
    else if (e.isCP()){

        std::vector<Expression> data = e.asVector();

        for(auto & item : data){

            if(item.isLine()){
                item.setLineThickness(0);
            }
            else if (item.isPoint()){
                item.setPointSize(0);
                std::cout << "A man of your talents-- drawing Points, really?" << std::endl;
            }
            catch_result(item);
        }
    }
    else if(!e.isLambda()) {
        // Not a special case or user-defined function, display normally
        scene->addText(QString::fromStdString(e.toString()));
    }

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::catch_failure(std::string message) {
    scene->clear();
    QString msg = QString::fromStdString(message);
    QGraphicsTextItem * output = new QGraphicsTextItem;
    output->setPos(0,0);
    output->setPlainText(msg);
    scene->addItem(output);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::clear_screen() {
    scene->clear();
}

void OutputWidget::resizeEvent(QResizeEvent *event) {
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    QWidget::resizeEvent(event);
}

void OutputWidget::drawText(QString qstr, double scaleFactor, double rotationAngle, double X, double Y) {
    QGraphicsTextItem *text = scene->addText(qstr);
    auto font = QFont("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(1);
    text->setFont(font);
    QRectF rect = text->sceneBoundingRect();
    QPointF text_center = QPointF(X - rect.width()/2, Y - rect.height()/2);
    text->setPos(text_center);

    QPointF _center = text->boundingRect().center();
    text->setTransformOriginPoint(_center);

    text->setScale(scaleFactor);
    text->setRotation(rotationAngle);
}

void OutputWidget::drawLine(double P1x, double P1y, double P2x, double P2y, double thicc){
    QLineF line = QLineF(QPointF(P1x, P1y), QPointF(P2x, P2y));
    scene->addLine(line, QPen(QBrush(Qt::SolidLine), thicc));
}

void OutputWidget::drawPoint(double X, double Y, double D){
    QRectF corners = QRectF(0, 0, D, D);
    corners.moveCenter(QPointF(X, Y));
    scene->addEllipse(corners, QPen(Qt::NoPen), QBrush(Qt::SolidPattern));
}