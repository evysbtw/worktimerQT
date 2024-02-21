#include "mainwindow.h"
#include <QApplication>
#include <QStackedWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStackedWidget stackedWidget;

    // Create the main window and pass the QStackedWidget to it
    MainWindow mainWindow(&stackedWidget);

    // Create the login page and pass the QStackedWidget to it

    // Add widgets to the QStackedWidget
    stackedWidget.addWidget(&mainWindow);

    // Set the initial page to be the main window
    stackedWidget.setCurrentWidget(&mainWindow);

    // Show the QStackedWidget
    stackedWidget.show();

    return a.exec();
}
