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
            drawText(text_string, scaleFactor, rotationAngle, xcor, ycor);
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
        // std::cout << "Data size: " << data.size() << std::endl;

        // Draw bounding box
        for(size_t i = 0; i < 10; i++){
            data[i].setLineThickness(0);
            data[i].setPointSize(0.5);
            catch_result(data[i]);
        }

        // Draw AL AU OL OU
        std::string AL_s, AU_s, OL_s, OU_s;
        AL_s = removeQuotes(data[10].head().asString());
        AU_s = removeQuotes(data[11].head().asString());
        OL_s = removeQuotes(data[12].head().asString());
        OU_s = removeQuotes(data[13].head().asString());
        double AL = std::stod(AL_s);
        double AU = std::stod(AU_s);
        double OL = std::stod(OL_s);
        double OU = std::stod(OU_s);
        double AM = (AU+AL)/2, OM = (OU+OL)/2;
        double xscale = N/(AU-AL), yscale = N/(OU-OL);
        AL *= xscale;
        AU *= xscale;
        OL *= yscale;
        OU *= yscale;
        AM *= xscale;
        OM *= yscale;
        std::cout << "Xmin, Xmax: " << AL << " " << AU << "\nYmin, max: "
        << OL << ", " << OU << "\nXmid, Ymid: " << AM << " " << OM << "\n\n";

        drawText(AL_s, 1, 0, AL, OU + C);
        // std::cout << "AL: " << AL_s << " at " << AL << ", " << OL + C << std::endl;
        drawText(AU_s, 1, 0, AU, OU + C);
        // std::cout << "AU: " << AU_s << " at " << AU << ", " << OL + C << std::endl;
        drawText(OL_s, 1, 0, AL - D, OU);
        // std::cout << "OL: " << OL_s << " at " << AL - D << ", " << OU << std::endl;
        drawText(OU_s, 1, 0, AL - D, OL);
        // std::cout << "OU: " << OU_s << " at " << AL - D << ", " << OL << std::endl;

        // Graph options
        std::string title = removeQuotes(data[14].head().asString());
        std::string a_label = removeQuotes(data[15].head().asString());
        std::string o_label = removeQuotes(data[16].head().asString());

        double textscale;
        if(data.size()==18)
            textscale = data[17].head().asNumber();
        else
            textscale = 1.0;

        // Add the graph labels
        drawText(title, textscale, 0, AM, (-OU-A));
        drawText(a_label, textscale, 0, AM, (OU+A));
        drawText(o_label, textscale, -90, (AL-B), -OM);
        std::cout << "Ylabel: " << o_label << " at (" << AL - B << ", " << OM << ")" << std::endl;

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
// void drawText(qstr, font, scaleFactor, rotationAngle, xcor, ycor);
void OutputWidget::drawText(std::string words, double scaleFactor, double rotationAngle, double xcor, double ycor) {
    QString qstr = QString::fromStdString(words);
    QGraphicsTextItem *text = scene->addText(qstr);
    auto font = QFont("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(1);
    text->setFont(font);
    text->setTransformOriginPoint(QPointF(0, 0));
    text->setScale(scaleFactor);
    text->setRotation(rotationAngle);
    QRectF rect = text->sceneBoundingRect();
    QPointF text_center = QPointF(xcor - rect.width()/2, ycor - rect.height()/2);
    if(rotationAngle == -90){
        text_center = QPointF(xcor - rect.height()/2, ycor + rect.width()/2);
    }
    text->setPos(text_center);
}

void OutputWidget::drawLine(double P1x, double P1y, double P2x, double P2y, double thicc){
    QLineF line = QLineF(QPointF(P1x, P1y), QPointF(P2x, P2y));
    scene->addLine(line, QPen(QBrush(Qt::SolidLine), thicc));
}

void OutputWidget::drawPoint(double X, double Y, double D){
    QRectF corners = QRectF(X, Y, D, D);
    corners.moveCenter(QPointF(X, Y));
    scene->addEllipse(corners, QPen(), QBrush(Qt::SolidPattern));
}