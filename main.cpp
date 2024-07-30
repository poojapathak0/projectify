#include "mainwindow.h"

 #include <QApplication>
 #include <QLocale>
 #include <QTranslator>
 #include <QMainWindow>
 #include <QStackedWidget>
 #include <QVBoxLayout>
 #include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow mainWindow;
    MainWindow w;
    w.show();
    
    //gave error so i commented

    //  if (!createConnection())
    //     return 1;
    // TableEditor editor("person");
    // editor.show();
    
    
    return a.exec();
}


