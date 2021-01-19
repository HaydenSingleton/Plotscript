#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QSet>
#include <QApplication>

class InputWidget: public QPlainTextEdit {
    Q_OBJECT

    public:
        InputWidget(QWidget *parent = nullptr);

    signals:
        void send_input(QString s);
        void clear_output();

    private:
        // to store which keys have been pressed
        QSet<int> pressedKeys;

        // to catch combinations of key presses
        bool eventFilter(QObject * obj, QEvent * event);
};

#endif