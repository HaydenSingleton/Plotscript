#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QKeySequence>

class InputWidget: public QPlainTextEdit {
    Q_OBJECT

    public:
        InputWidget(QWidget *parent = nullptr) : QPlainTextEdit(parent) {
            setObjectName("input");
            setBackgroundVisible(true);
            setPlaceholderText(QString("Type code here."));
        };

    protected:
        void mousePressEvent(QMouseEvent *event) override {
            if (event->button() == Qt::LeftButton) {
                setPlainText(QString());
            }
        };

        // void keyPressEvent(QKeyEvent *event) {
        //     if(event->text() == QKeySequence(tr("Shift+Enter")).toString() ) {
        //         setPlainText("REEEEEEEEEEEEEEEEEE VAL");
        //     }
        // };

    signals:
        void send_result(QString r);

};

#endif