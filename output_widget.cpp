#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
    setObjectName("output");
    auto layout = new QHBoxLayout(this);
    layout->addWidget(view);
    view->setScene(scene);
}

void OutputWidget::catch_result(Expression e){
    if(clear_on_print){
        clear_screen();
    }

    if(e.isPoint()) {
        std::vector<Expression> coordinates = e.contents();
        double x = coordinates[0].head().asNumber();
        double y = coordinates[1].head().asNumber();
        double diam = e.getNumericalProperty("\"size\"");
        if(diam < 0){
            catch_failure("Error: in make-point call: diameter not positive");
            return;
        }
        drawPoint(x, y, diam);
    }
    else if (e.isLine()) {

        Expression p1, p2;
        std::vector<Expression> list = e.contents();
        p1 = list[0];
        p2 = list[1];

        if(p1.isPoint() && p2.isPoint()){
            std::vector<Expression> coordinates = e.contents();
            double a, b, c, d;
            a = coordinates[0].head().asNumber();
            b = coordinates[1].head().asNumber();
            c = coordinates[2].head().asNumber();
            d = coordinates[3].head().asNumber();
            double thicc = e.getNumericalProperty("\"thickness\"");
            if(thicc < 0){
                catch_failure("Error: in make-line call: thickness value not positive");
                return;
            }
            drawLine(a, b, c, d, thicc);
        }
        else {
            catch_failure("Error: argument to make-line not a point");
            return;
        }
    }
    else if (e.isText()) {

        std::string text_string = e.head().asSymbol();

        double xcor, ycor, scaleFactor, rotRads, rotDeg;
        std::tie(xcor, ycor, scaleFactor, rotRads) = e.getTextProperties();
        rotDeg = rotRads * (180 / std::atan2(0, -1));
        
        drawText(QString::fromStdString(text_string), scaleFactor, rotDeg, xcor, ycor);
    }
    else if (e.isList()) {
        clear_on_print = false;
        for (auto &item: e.contents()) {
            catch_result(item);
        }
        clear_on_print = true;
    }
    else if (e.isDP()) {
        clear_on_print = false;
        double N = 20, A = 3, B = 3, C = 2, D = 2, P = 0.5;
        std::vector<Expression> data = e.contents();
        int pos = 0;
    
        // Draw bounding box
        for(int i = 0; i < 4; i++) {
            catch_result(data[i]);
            pos++;
        }
        
        // Draw AL AU OL OU (axis bound labels)
        std::string AL_s, AU_s, OL_s, OU_s;
        AL_s = data[pos++].head().asSymbol();
        AU_s = data[pos++].head().asSymbol();
        OL_s = data[pos++].head().asSymbol();
        OU_s = data[pos++].head().asSymbol();
        double AL = std::stod(AL_s);
        double AU = std::stod(AU_s);
        double OL = std::stod(OL_s);
        double OU = std::stod(OU_s);
        double xscale = N/(AU-AL), yscale = N/(OU-OL);
        AL *= xscale;
        AU *= xscale;
        OL *= yscale * -1;
        OU *= yscale * -1;

        drawText(QString::fromStdString(AL_s), 1, 0, AL, OL+C);
        drawText(QString::fromStdString(AU_s), 1, 0, AU, OL+C);
        drawText(QString::fromStdString(OL_s), 1, 0, AL-D, OL);
        drawText(QString::fromStdString(OU_s), 1, 0, AL-D, OU);

        // Graph options
        std::string title = data[pos++].head().asSymbol();
        std::string a_label = data[pos++].head().asSymbol();
        std::string o_label = data[pos++].head().asSymbol();

        double textscale;
        if(data.size() > pos)
            textscale = data[pos++].head().asNumber();
        else
            textscale = 1.0;

        std::cout << "Text scale: " << textscale << std::endl;

        // Add the graph labels
        double AM = (AU+AL)/2, OM = (OU+OL)/2;
        drawText(QString::fromStdString(title), textscale, 0, AM, (OU-A));
        drawText(QString::fromStdString(a_label), textscale, 0, AM, (OL+A));
        drawText(QString::fromStdString(o_label), textscale, -90, (AL-B), OM);


        clear_on_print = true;
    }
    else if (e.isCP()){
        clear_on_print = false;
        //TODO
        clear_on_print = true;
    }
    else if(!e.isLambda()) {
        // Not a special case or user-defined function, display normally
        scene->addText(QString::fromStdString(e.head().asString()));
    }

    rescale();
}

void OutputWidget::catch_failure(std::string message) {
    QString msg = QString::fromStdString(message);
    drawText(msg);
    rescale();
}

void OutputWidget::clear_screen() {
    scene->clear();
}

void OutputWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    rescale();
}

void OutputWidget::drawText(QString qstr, double scaleFactor, double rotationAngle, double X, double Y) {
    QGraphicsTextItem *text = scene->addText(qstr);
    auto font = QFont("Courier");
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(1);
    text->setFont(font);

    QRectF rect = text->sceneBoundingRect();
    QPointF text_center = QPointF(X - rect.width()/2, Y - rect.height()/2);
    text->setPos(text_center);

    QPointF _center = text->boundingRect().center();
    text->setTransformOriginPoint(_center);
    text->setRotation(rotationAngle);
    text->setScale(scaleFactor);
}

void OutputWidget::drawLine(double P1x, double P1y, double P2x, double P2y, double thicc){
    QLineF line = QLineF(QPointF(P1x, P1y), QPointF(P2x, P2y));
    scene->addLine(line, QPen(QBrush(Qt::SolidLine), thicc));
}

void OutputWidget::drawPoint(double X, double Y, double Diam){
    QRectF corners = QRectF(0, 0, Diam, Diam);
    corners.moveCenter(QPointF(X, Y));
    scene->addEllipse(corners, QPen(Qt::NoPen), QBrush(Qt::SolidPattern));
}