#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include<QtSql/QSqlDatabase>
#include <QMessageBox>
#include <QRegularExpression>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QWidget>
#include <QVector>



QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     void createPieChart();



private slots:

    void on_pushButtonlogin_clicked();
    
    void on_pushButtonbacklogin_clicked();

    void on_pushButtonCreateAccount_clicked();

    void on_pushButtonconfirm_clicked();

    void on_pushButtonback_e_clicked();

    void on_pushButtonConfirmExpense_clicked();

    void on_pushButtonadd_clicked();

    void on_pushButtonAddIncome_clicked();

    void on_pushButtonAddExpense_clicked();

    void on_pushButtoncancel_clicked();

    void on_pushButtongback_in_clicked();

    void on_pushButtonConfirmIncome_clicked();

    void on_pushButtonsignup_clicked();

    void on_pushButtontracker_clicked();

    void on_pushButtonbacktomain_clicked();

    void on_pushButtonDeleteAllTransactions_clicked();

    void on_pushButtonForgotPassword_clicked();
    
    void on_pushButtonLogout_clicked();



private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    double totalBalance;
    void initializeDatabase();
    QLabel *dateLabel;
    QLabel *monthYearLabel;
    void addTransaction(double amount,const QString& category, const QString& description);
    void viewTransactions();
    QString firstName, middleName, lastName, username, mobileNumber, password;
    QString loggedInUsername;
    bool validatePassword(const QString &password);
    bool validateInputs();
    bool validateSecurityAnswers();
    void setupTransactionTable();
    void deleteTransaction(const QString &transactionId);
    double calculateTotalExpense();
    double calculateTotalIncome();
    void updateFinancialSummary();
    void checkSecurityAnswers(const QString &username);
    void changePassword(const QString &username);
    void setupPieChart();
    void updateCategoryExpenses();
    double calculateAmountForCategory(const QString& category);
};
#endif // MAINWINDOW_H
