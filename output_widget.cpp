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

    if(e.checkProperty("object-name", "point")) {

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
    else if (e.checkProperty("object-name", "line")) {

        Expression p1, p2;
        std::vector<Expression> list = e.contents();
        p1 = list[0];
        p2 = list[1];

        if(p1.checkProperty("object-name", "point") && p2.checkProperty("object-name", "point")){
            double a = p1.tailConstBegin()->head().asNumber();
            double b = p1.tailConstEnd()->head().asNumber();
            double c = p2.tailConstBegin()->head().asNumber();
            double d = p2.tailConstEnd()->head().asNumber();
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
    else if (e.checkProperty("object-name", "text")) {

        std::string text_string = e.head().asSymbol();

        double xcor, ycor, scaleFactor, rotRads, rotDeg;
        std::tie(xcor, ycor, scaleFactor, rotRads) = e.getTextProperties();
        rotDeg = rotRads * (180 / std::atan2(0, -1));
        
        drawText(QString::fromStdString(text_string), scaleFactor, rotDeg, xcor, ycor);
    }
    else if (e.isList()) {
        clear_on_print = false;
        for (auto &item: e.contents()) {
            drawListItem(item);
        }
        clear_on_print = true;
    }
    else if (e.isDP()) {
        clear_on_print = false;
        drawDP(e);
    }
    else if (e.isCP()){
        clear_on_print = false;
        //TODO
    }
    else if(e.isLambda()) {
        return;
    }
    else{
        std::ostringstream os;
        os << e;
        scene->addText(QString::fromStdString(os.str()));
    }
    clear_on_print = true;
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

void OutputWidget::rescale() {
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
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

void OutputWidget::drawLine(double x1, double y1, double x2, double y2, double thicc){

    QGraphicsLineItem *line = scene->addLine(x1, y1, x2, y2);
    line->setPen(QPen(QBrush(), thicc));
}

void OutputWidget::drawPoint(double X, double Y, double Diam){
    QRectF corners = QRectF(X, Y, Diam, Diam);
    corners.moveCenter(QPointF(X, Y));
    scene->addEllipse(corners, QPen(Qt::PenStyle::NoPen), QBrush(Qt::BrushStyle::SolidPattern));
}

void OutputWidget::drawListItem(Expression e) {
    if (!e.isNone()) {
        catch_result(e);
    }
    std::string val;
    std::ostringstream os;
    os << e;
    val = os.str();
    QString qstr = QString::fromStdString(val.substr(1, val.size() - 2));
    scene->addText(qstr);
}

void OutputWidget::drawDP(Expression e) {

    // Graph constants
    double N = 20, A = 3, B = 3, C = 2, D = 2, P = 0.5;

    std::vector<Expression> data = e.contents();
    int i = 0;

    // Draw bounding box lines
    for(i; i < 4; i++) {
        catch_result(data[i]);
    }

    // Draw axis bounds and labels
    Atom ALa = data[i++].head();
    Atom AUa = data[i++].head();
    Atom OLa = data[i++].head();
    Atom OUa = data[i++].head();
    double AL = ALa.asNumber();
    double AU = AUa.asNumber();
    double OL = OLa.asNumber();
    double OU = OUa.asNumber();
    double xscale = N/(AU-AL), yscale = N/(OU-OL);
    AL *= xscale;
    AU *= xscale;
    OL *= yscale * -1;
    OU *= yscale * -1;
    drawText(QString::fromStdString(ALa.asString()), 1, 0, AL, OL+C);
    drawText(QString::fromStdString(AUa.asString()), 1, 0, AU, OL+C);
    drawText(QString::fromStdString(OLa.asString()), 1, 0, AL-D, OL);
    drawText(QString::fromStdString(OUa.asString()), 1, 0, AL-D, OU);

    size_t num_opt = (int)e.getProperty("numoptions").asNumber();
    for (i; i < 11; i++) {
        catch_result(data[i]);
    }
    double text_scale = 1;
    if (num_opt == 4) {
        text_scale = data[i++].head().asNumber();
    }


    size_t num_data = (int)e.getProperty("numpoints").asNumber();
    for (i; i < 15; i++) {
        catch_result(data[i]);
    }

    // Draw data points and stem lines
    for(i; i < num_data; i++) {
        catch_result(data[i]);
    }

    // Draw x axis if needed
    if (i < data.size()) {
        catch_result(data[i++]);
    }
    // Draw y axis if needed
    if (i < data.size()) {
        catch_result(data[i++]);
    }

} 