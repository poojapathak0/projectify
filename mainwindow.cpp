#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<QString>
#include<QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDate>
#include<QLabel>
#include <QRegularExpression>
#include <QMessageBox>
#include <QUuid>
#include <QInputDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>


// Your remaining implementation...


void checkDatabaseDriver()
{
    QStringList drivers = QSqlDatabase::drivers();
    qDebug() << "Available drivers:" << drivers;
    qDebug() << "SQLite Driver Available:" << QSqlDatabase::isDriverAvailable("QSQLITE");
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,totalBalance(0.0)

{setFixedSize(850, 650);
    ui->setupUi(this);
    ui->lineEditpassword->setEchoMode(QLineEdit::Password);
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);

     // Initialize the database
    initializeDatabase();
    // Assuming ui->stackedWidget is your QStackedWidget
    ui->stackedWidget->setCurrentIndex(0); // Set to the first page by default

    // Make sure the stacked widget fits the window
    setCentralWidget(ui->stackedWidget);

    // Get the current date
    QDate currentDate = QDate::currentDate();

     //Date setup
    // Set the date in the desired format
    ui->dateLabel->setText(currentDate.toString("yyyy/MM/dd"));

    //month setup
    ui->monthLabel->setText(currentDate.toString("MMM"));
    //year setup
    ui->yearLabel->setText(currentDate.toString("yyyy"));
setupTransactionTable();

    //----------------------------------------------------------------------------


// Connect signals to slots
    // Connect buttons to slots
    //goku commented this out
    /*
    connect(ui->pushButtonAddIncome, &QPushButton::clicked, this, &MainWindow::on_pushButtonAddIncome_clicked);
    connect(ui->pushButtonAddExpense, &QPushButton::clicked, this, &MainWindow::on_pushButtonAddExpense_clicked);
    connect(ui->pushButtonConfirmIncome, &QPushButton::clicked, this, &MainWindow::on_pushButtonConfirmIncome_clicked);
    connect(ui->pushButtonConfirmExpense, &QPushButton::clicked, this, &MainWindow::on_pushButtonConfirmExpense_clicked);*/
}

//just created struct not in use right now
struct accountInfo{
    QString username;
    QString password;
};



MainWindow::~MainWindow()
{
    delete ui;
    db.close();
}
void MainWindow::on_pushButtonLogout_clicked()
{
    // Clear session data
    loggedInUsername.clear();

    // Clear the username and password fields
    ui->lineEditusername->clear();
    ui->lineEditpassword->clear();

    // Switch to the login page
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::initializeDatabase()
{
    // Set up the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
   // db.setDatabaseName("C:/Users/ACER/Desktop/semester_project/database/users.db");  // Use the full path
    db.setDatabaseName("users.db");  // If the database is in the working directory
 //   QString path = "C:/Users/ACER/Desktop/semester_project/database/users.db";
  //  qDebug() << "Database Path:" << path;
  //  db.setDatabaseName(path);

    // Open the database
    if (!db.open()) {
        qDebug() << "Database connection failed!";
    } else {
        qDebug() << "Database connected successfully!";
    }

    QSqlQuery query;

    // Create users table
    QString createUsersTableQuery = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT,
            middle_name TEXT,
            last_name TEXT,
            username TEXT UNIQUE,
            mobile_number TEXT,
            password TEXT,
            security_answer1 TEXT,
            security_answer2 TEXT,
            security_answer3 TEXT
        )
    )";
    if (!query.exec(createUsersTableQuery)) {
        qDebug() << "Error creating users table: " << query.lastError();
    } else {
        qDebug() << "Users table created successfully.";
    }

    // Create transactions table
    QString createTransactionsTableQuery = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            transaction_id TEXT PRIMARY KEY,
            date TEXT,
            amount REAL,
            category TEXT,
            description TEXT,
            username TEXT,
            FOREIGN KEY(username) REFERENCES users(username)
        )
    )";
    if (!query.exec(createTransactionsTableQuery)) {
        qDebug() << "Error creating transactions table: " << query.lastError();
    } else {
        qDebug() << "Transactions table created successfully.";
    }
}

void MainWindow::on_pushButtonsignup_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
//navigation
void MainWindow::on_pushButtonlogin_clicked()
{
    initializeDatabase();

    QString userName = ui->lineEditusername->text();
    QString userPassword = ui->lineEditpassword->text();

    // Validate input
    if (userName.isEmpty() || userPassword.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and password cannot be empty.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", userName);
    query.bindValue(":password", userPassword);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to query database: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        // Login successful
        loggedInUsername = userName;  // Store the logged-in username
        QMessageBox::information(this, "Login Successful", "Welcome, " + userName + "!");
        ui->stackedWidget->setCurrentIndex(3); // Switch to the next page
        viewTransactions();
        updateFinancialSummary();
    } else {
        // Login failed
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }

}

bool MainWindow::validatePassword(const QString &password)
{
    // Define forbidden special characters
    //QString forbiddenChars = "~!@#";

    // Check password length
    if (password.length() < 8) {
        QMessageBox::warning(this, "Password Error", "Password must be at least 8 characters long.");
        return false;
    }

    // Check for forbidden characters
    // for (const QChar &ch : password) {
    //     if (forbiddenChars.contains(ch)) {
    //         QMessageBox::warning(this, "Password Error", "Password contains forbidden special characters.");
    //         return false;
    //     }
    // }

    return true;
}

bool MainWindow::validateInputs()
{
    // Collect data
    firstName = ui->lineEditFirstName->text();
    middleName = ui->lineEditMiddleName->text();
    lastName = ui->lineEditLastName->text();
    username = ui->lineEditUsername->text();
    mobileNumber = ui->lineEditMobileNumber->text();
    password = ui->lineEditPassword->text();

    // Validate non-empty fields
    if (firstName.isEmpty() || lastName.isEmpty() || username.isEmpty() || mobileNumber.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all required fields.");
        return false;
    }

    return true;
}

bool MainWindow::validateSecurityAnswers()
{
    QString answer1 = ui->lineEditSecurityAnswer1->text();
    QString answer2 = ui->lineEditSecurityAnswer2->text();
    QString answer3 = ui->lineEditSecurityAnswer3->text();

    // Validate non-empty security answers
    if (answer1.isEmpty() || answer2.isEmpty() || answer3.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please answer all security questions.");
        return false;
    }

    return true;
}

void MainWindow::on_pushButtonCreateAccount_clicked()
{
    if (!validateInputs() || !validatePassword(password)) {
        // Stay on the create account page if validation fails
        return;
    }

    // Switch to the next page to collect security questions
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButtonconfirm_clicked()
{
    QString firstName = ui->lineEditFirstName->text();
    QString middleName = ui->lineEditMiddleName->text();
    QString lastName = ui->lineEditLastName->text();
    QString username = ui->lineEditUsername->text();
    QString mobileNumber = ui->lineEditMobileNumber->text();
    QString password = ui->lineEditPassword->text();
    QString answer1 = ui->lineEditSecurityAnswer1->text();
    QString answer2 = ui->lineEditSecurityAnswer2->text();
    QString answer3 = ui->lineEditSecurityAnswer3->text();

    // Validate inputs
    if (firstName.isEmpty() || lastName.isEmpty() || username.isEmpty() || mobileNumber.isEmpty() ||
        password.isEmpty() || answer1.isEmpty() || answer2.isEmpty() || answer3.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled.");
        ui->stackedWidget->setCurrentIndex(1); // Go back to the create account page
        return;
    }

    if (!validatePassword(password)) {
        ui->stackedWidget->setCurrentIndex(1); // Go back to the create account page if validation fails
        return;
    }

    // Validate mobile number
    if (mobileNumber.length() != 10 || !mobileNumber.contains(QRegularExpression("^\\d{10}$"))) {
        QMessageBox::warning(this, "Phone Number Error", "Phone number must be exactly 10 digits number only.");
        ui->stackedWidget->setCurrentIndex(1); // Go back to the create account page
        return;
    }



    initializeDatabase();
    QSqlQuery query;
    query.prepare("INSERT INTO users (first_name, middle_name, last_name, username, mobile_number, password, security_answer1, security_answer2, security_answer3) "
                  "VALUES (:firstName, :middleName, :lastName, :username, :mobileNumber, :password, :answer1, :answer2, :answer3)");
    query.bindValue(":firstName", firstName);
    query.bindValue(":middleName", middleName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":username", username);
    query.bindValue(":mobileNumber", mobileNumber);
    query.bindValue(":password", password);
    query.bindValue(":answer1", answer1);
    query.bindValue(":answer2", answer2);
    query.bindValue(":answer3", answer3);

    if (!query.exec()) {
        // Check for unique constraint violation in the error message
        QString errorMsg = query.lastError().text();
        if (errorMsg.contains("UNIQUE constraint failed") || errorMsg.contains("UNIQUE constraint")) {
            QMessageBox::warning(this, "Registration Error", "Username or mobile number already exists.");
            ui->stackedWidget->setCurrentIndex(1); // Go back to the create account page
        } else {
            QMessageBox::critical(this, "Error", "Failed to add user: " + errorMsg);
        }
    } else {
        QMessageBox::information(this, "Success", "User added successfully");
        // Clear input fields after successful registration
        ui->lineEditFirstName->clear();
        ui->lineEditMiddleName->clear();
        ui->lineEditLastName->clear();
        ui->lineEditUsername->clear();
        ui->lineEditMobileNumber->clear();
        ui->lineEditPassword->clear();
        ui->lineEditSecurityAnswer1->clear();
        ui->lineEditSecurityAnswer2->clear();
        ui->lineEditSecurityAnswer3->clear();
        ui->stackedWidget->setCurrentIndex(0); // Go to the login page
    }
    //if (db.isOpen()) {
        // QSqlQuery query;
        // query.prepare("INSERT INTO users (first_name, middle_name, last_name, username, mobile_number, password, security_answer1, security_answer2, security_answer3) "
        //               "VALUES (:firstName, :middleName, :lastName, :username, :mobileNumber, :password, :answer1, :answer2, :answer3)");
        // query.bindValue(":firstName", firstName);
        // query.bindValue(":middleName", middleName);
        // query.bindValue(":lastName", lastName);
        // query.bindValue(":username", username);
        // query.bindValue(":mobileNumber", mobileNumber);
        // query.bindValue(":password", password);
        // query.bindValue(":answer1", answer1);
        // query.bindValue(":answer2", answer2);
        // query.bindValue(":answer3", answer3);

        // if (!query.exec()) {
        //     // Check for unique constraint violation in the error message
        //     QString errorMsg = query.lastError().text();
        //     if (errorMsg.contains("UNIQUE constraint failed") || errorMsg.contains("UNIQUE constraint")) {
        //         QMessageBox::warning(this, "Registration Error", "Username or mobile number already exists.");
        //         ui->stackedWidget->setCurrentIndex(1); // Go back to the create account page
        //     } else {
        //         QMessageBox::critical(this, "Error", "Failed to add user: " + errorMsg);
        //     }
        // } else {
        //     QMessageBox::information(this, "Success", "User added successfully");
        //     // Clear input fields after successful registration
        //     ui->lineEditFirstName->clear();
        //     ui->lineEditMiddleName->clear();
        //     ui->lineEditLastName->clear();
        //     ui->lineEditUsername->clear();
        //     ui->lineEditMobileNumber->clear();
        //     ui->lineEditPassword->clear();
        //     ui->lineEditSecurityAnswer1->clear();
        //     ui->lineEditSecurityAnswer2->clear();
        //     ui->lineEditSecurityAnswer3->clear();
        //     ui->stackedWidget->setCurrentIndex(0); // Go to the login page
        // }
   // } else {
      //  QMessageBox::critical(this, "Error", "Database connection failed !");
    //}
}
void MainWindow::on_pushButtonbacklogin_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pushButtonback_e_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_pushButtonadd_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButtonAddExpense_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}


void MainWindow::on_pushButtonAddIncome_clicked()
{
     ui->stackedWidget->setCurrentIndex(6);
}


void MainWindow::on_pushButtoncancel_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
     viewTransactions();
}

void MainWindow::on_pushButtonbacktomain_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
     viewTransactions();
}

void MainWindow::on_pushButtontracker_clicked()
{
    updateCategoryExpenses();
    // createPieChart();  // Call viewTransactions to update the displayed transactions
    ui->stackedWidget->setCurrentIndex(7);  // Switch to the transactions view
}

void MainWindow::on_pushButtongback_in_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButtonConfirmIncome_clicked()
{
    qDebug() << "Confirm Income button clicked.";
    ui->stackedWidget->setCurrentIndex(3);
    double amount = ui->lineEditAmountIncome->text().toDouble();
    QString description = ui->comboBoxincome->currentText();
    addTransaction(amount, "Income", description);
     viewTransactions();
}

void MainWindow::on_pushButtonConfirmExpense_clicked()
{
    qDebug() << "Confirm Expense button clicked.";
    ui->stackedWidget->setCurrentIndex(3);
    double amount = ui->lineEditAmountExpense->text().toDouble();
    QString description = ui->comboBoxexpense->currentText();
    addTransaction(amount, "Expense", description);
     viewTransactions();
}


void MainWindow::addTransaction(double amount, const QString& category, const QString& description)
{
    qDebug() << "Adding transaction:" << amount << category << description;
    QSqlQuery query;
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    query.prepare("INSERT INTO transactions (transaction_id, date, amount, category, description, username) VALUES (:transaction_id, :date, :amount, :category, :description, :username)");
    query.bindValue(":transaction_id", QUuid::createUuid().toString());
    query.bindValue(":date", currentDateTime);
    query.bindValue(":amount", amount);
    query.bindValue(":category", category);
    query.bindValue(":description", description);
    query.bindValue(":username", loggedInUsername);  // Add the username

    if (!query.exec()) {
        qDebug() << "Error adding transaction:" << query.lastError();
    } else {
        qDebug() << "Transaction added successfully.";
        updateFinancialSummary();
        updateCategoryExpenses(); // Update the category expenses
    }
}

void MainWindow::deleteTransaction(const QString& transactionId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM transactions WHERE transaction_id = :transaction_id");
    query.bindValue(":transaction_id", transactionId);

    if (!query.exec()) {
        qDebug() << "Error deleting transaction:" << query.lastError();
    } else {
        qDebug() << "Transaction deleted successfully.";
        updateFinancialSummary();
        updateCategoryExpenses(); // Update the category expenses
    }
}
void MainWindow::setupTransactionTable()
{
    ui->tableWidgetTransactions->setColumnCount(6); // Increase column count to include delete button
    QStringList headers = {"Transaction ID", "Date", "Amount", "Category", "Description", "Delete"};
    ui->tableWidgetTransactions->setHorizontalHeaderLabels(headers);
}


void MainWindow::viewTransactions()
{
    if (db.isOpen()) {
        qDebug() << "Database connection is not open!";
        QMessageBox::critical(this, "Database Error", "Database connection failed!");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT transaction_id, date, amount, category, description FROM transactions WHERE username = :username");
    query.bindValue(":username", loggedInUsername);  // Assuming loggedInUsername stores the username of the logged-in user

    if (!query.exec()) {
        qDebug() << "Error fetching transactions:" << query.lastError();
        QMessageBox::critical(this, "Database Error", "Failed to fetch transactions: " + query.lastError().text());
        return;
    }

    // Clear the table first
    ui->tableWidgetTransactions->clearContents();
    ui->tableWidgetTransactions->setRowCount(0);

    int row = 0;
    while (query.next()) {
        ui->tableWidgetTransactions->insertRow(row);

        QTableWidgetItem *transactionIdItem = new QTableWidgetItem(query.value("transaction_id").toString());
        QTableWidgetItem *dateItem = new QTableWidgetItem(query.value("date").toString());
        QTableWidgetItem *amountItem = new QTableWidgetItem(query.value("amount").toString());
        QTableWidgetItem *categoryItem = new QTableWidgetItem(query.value("category").toString());
        QTableWidgetItem *descriptionItem = new QTableWidgetItem(query.value("description").toString());
        QPushButton *deleteButton = new QPushButton("Delete");

        transactionIdItem->setFlags(transactionIdItem->flags() & ~Qt::ItemIsEditable);
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        amountItem->setFlags(amountItem->flags() & ~Qt::ItemIsEditable);
        categoryItem->setFlags(categoryItem->flags() & ~Qt::ItemIsEditable);
        descriptionItem->setFlags(descriptionItem->flags() & ~Qt::ItemIsEditable);

        ui->tableWidgetTransactions->setItem(row, 0, transactionIdItem);
        ui->tableWidgetTransactions->setItem(row, 1, dateItem);
        ui->tableWidgetTransactions->setItem(row, 2, amountItem);
        ui->tableWidgetTransactions->setItem(row, 3, categoryItem);
        ui->tableWidgetTransactions->setItem(row, 4, descriptionItem);
        ui->tableWidgetTransactions->setCellWidget(row, 5, deleteButton);

        connect(deleteButton, &QPushButton::clicked, this, [=]() {
            QString transactionId = transactionIdItem->text();
            deleteTransaction(transactionId);
            viewTransactions();  // Refresh the table after deletion
        });

        row++;
    }

    // Set specific width for transaction_id column
    ui->tableWidgetTransactions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableWidgetTransactions->setColumnWidth(0, 250); // Set the width as per your requirement

    // Make other columns stretch to fit the table width
    for (int i = 1; i < ui->tableWidgetTransactions->columnCount() - 1; ++i) {
        ui->tableWidgetTransactions->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    // Set the size policy to make the table expand
    ui->tableWidgetTransactions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
void MainWindow::on_pushButtonDeleteAllTransactions_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete All Transactions", "Are you sure you want to delete all transactions?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM transactions WHERE username = :username");
        query.bindValue(":username", loggedInUsername);

        if (!query.exec()) {
            qDebug() << "Error deleting all transactions:" << query.lastError();
            QMessageBox::critical(this, "Error", "Failed to delete all transactions: " + query.lastError().text());
        } else {
            qDebug() << "All transactions deleted successfully.";
            QMessageBox::information(this, "Success", "All transactions deleted successfully.");
            viewTransactions();
            updateFinancialSummary();
        }
    }
}


double MainWindow::calculateTotalExpense()
{
    if (db.isOpen()) {
        qDebug() << "Database connection is not open!";
        QMessageBox::critical(this, "Database Error", "Database connection failed!");
        return 0.0;
    }

    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM transactions WHERE username = :username AND category = 'Expense'");
    query.bindValue(":username", loggedInUsername);

    if (!query.exec()) {
        qDebug() << "Error calculating total expense:" << query.lastError();
        QMessageBox::critical(this, "Database Error", "Failed to calculate total expense: " + query.lastError().text());
        return 0.0;
    }

    if (query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

double MainWindow::calculateTotalIncome()
{
    if (db.isOpen()) {
        qDebug() << "Database connection is not open!";
        QMessageBox::critical(this, "Database Error", "Database connection failed!");
        return 0.0;
    }

    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM transactions WHERE username = :username AND category = 'Income'");
    query.bindValue(":username", loggedInUsername);

    if (!query.exec()) {
        qDebug() << "Error calculating total income:" << query.lastError();
        QMessageBox::critical(this, "Database Error", "Failed to calculate total income: " + query.lastError().text());
        return 0.0;
    }

    if (query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

void MainWindow::updateFinancialSummary()
{
    double totalExpense = calculateTotalExpense();
    double totalIncome = calculateTotalIncome();
    double totalBalance = totalIncome - totalExpense;

    ui->labelTotalExpense->setText(QString::number(totalExpense, 'f', 2));
    ui->labelTotalIncome->setText(QString::number(totalIncome, 'f', 2));
    ui->labelTotalBalance->setText(QString::number(totalBalance, 'f', 2));
}
void MainWindow::on_pushButtonForgotPassword_clicked()
{
    bool ok;
    QString username = QInputDialog::getText(this, "Forget Password", "Enter your username:", QLineEdit::Normal, "", &ok);

    if (ok && !username.isEmpty()) {
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE username = :username");
        query.bindValue(":username", username);

        if (!query.exec()) {
            QMessageBox::critical(this, "Database Error", "Failed to query database: " + query.lastError().text());
            return;
        }

        if (query.next()) {
            // Username exists
            checkSecurityAnswers(username);
        } else {
            // Username does not exist
            QMessageBox::warning(this, "Error", "Username does not exist.");
        }
    }
}

void MainWindow::checkSecurityAnswers(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT security_answer1, security_answer2, security_answer3 FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Database Error", "Failed to query database: " + query.lastError().text());
        return;
    }

    QString answer1 = query.value("security_answer1").toString();
    QString answer2 = query.value("security_answer2").toString();
    QString answer3 = query.value("security_answer3").toString();

    bool ok;
    QString userAnswer1 = QInputDialog::getText(this, "Security Question", "Enter your answer to security question 1:\nWhat is your birth place?", QLineEdit::Normal, "", &ok);
    if (!ok || userAnswer1 != answer1) {
        QMessageBox::warning(this, "Error", "Incorrect answer to security question 1.");
        return;
    }

    QString userAnswer2 = QInputDialog::getText(this, "Security Question", "Enter your answer to security question 2:\nWhat is your favourite food?", QLineEdit::Normal, "", &ok);
    if (!ok || userAnswer2 != answer2) {
        QMessageBox::warning(this, "Error", "Incorrect answer to security question 2.");
        return;
    }

    QString userAnswer3 = QInputDialog::getText(this, "Security Question", "Enter your answer to security question 3:\nWhat is your favourite movie? ", QLineEdit::Normal, "", &ok);
    if (!ok || userAnswer3 != answer3) {
        QMessageBox::warning(this, "Error", "Incorrect answer to security question 3.");
        return;
    }

    changePassword(username);
}

void MainWindow::changePassword(const QString &username)
{
    bool ok;
    QString newPassword = QInputDialog::getText(this, "Change Password", "Enter your new password:", QLineEdit::Password, "", &ok);

    if (ok && !newPassword.isEmpty() && validatePassword(newPassword)) {
        QSqlQuery query;
        query.prepare("UPDATE users SET password = :password WHERE username = :username");
        query.bindValue(":password", newPassword);
        query.bindValue(":username", username);

        if (!query.exec()) {
            QMessageBox::critical(this, "Database Error", "Failed to update password: " + query.lastError().text());
        } else {
            QMessageBox::information(this, "Success", "Password changed successfully.");
        }
    }
}

double MainWindow::calculateAmountForCategory(const QString& category)
{
    if (db.isOpen()) {
        qDebug() << "Database connection is not open!";
        QMessageBox::critical(this, "Database Error", "Database connection failed!");
        return 0.0;
    }

    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM transactions WHERE username = :username AND description = :category");
    query.bindValue(":username", loggedInUsername);
    query.bindValue(":category", category);

    if (!query.exec()) {
        qDebug() << "Error calculating amount for category:" << query.lastError();
        QMessageBox::critical(this, "Database Error", "Failed to calculate amount for category: " + query.lastError().text());
        return 0.0;
    }

    if (query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}


void MainWindow::updateCategoryExpenses()
{
    QLayoutItem *item;
    while ((item = ui->chartLayout->takeAt(0)) != nullptr) {
        delete item->widget(); // Delete the widget
        delete item; // Delete the layout item
    }

    double totalIncome = calculateTotalIncome();
    double totalExpense = calculateTotalExpense();
    double totalBalance = totalIncome - totalExpense;

    // Fetch amounts for each category
    double foodAmount = calculateAmountForCategory("Food");
    double entertainmentAmount = calculateAmountForCategory("Entertainment");
    double educationAmount = calculateAmountForCategory("Education");
    double housingAmount = calculateAmountForCategory("Housing");
    double insuranceAmount = calculateAmountForCategory("Insurance");
    double othersAmount = calculateAmountForCategory("Others");

    // Determine whether to use total income or total expenses as the total
    double total;
    double savingsPercentage = 0.0; // Initialize savingsPercentage

    if (totalBalance > 0) {
        total = totalIncome; // Use total income if balance is positive
        double savings = totalBalance; // Calculate savings as totalBalance

        if (total != 0) {
            // Calculate the percentage for savings
            savingsPercentage = (savings / total) * 100;
        } else {
            savingsPercentage = 0; // Avoid division by zero
        }
    } else {
        total = totalExpense; // Use total expenses if balance is negative
    }

    // Calculate the percentage for each category
    double foodPercentage = 0.0;
    double entertainmentPercentage = 0.0;
    double educationPercentage = 0.0;
    double housingPercentage = 0.0;
    double insurancePercentage = 0.0;
    double othersPercentage = 0.0;

    if (total != 0) {
        foodPercentage = (foodAmount / total) * 100;
        entertainmentPercentage = (entertainmentAmount / total) * 100;
        educationPercentage = (educationAmount / total) * 100;
        housingPercentage = (housingAmount / total) * 100;
        insurancePercentage = (insuranceAmount / total) * 100;
        othersPercentage = (othersAmount / total) * 100;
    }

    // Create the pie series
    QPieSeries *series = new QPieSeries();

    // Append categories and set labels with percentages
    QPieSlice *foodSlice = series->append("Food", foodPercentage);
    foodSlice->setLabel(QString("Food %1%").arg(foodPercentage, 0, 'f', 1));

    QPieSlice *entertainmentSlice = series->append("Entertainment", entertainmentPercentage);
    entertainmentSlice->setLabel(QString("Entertainment %1%").arg(entertainmentPercentage, 0, 'f', 1));

    QPieSlice *educationSlice = series->append("Education", educationPercentage);
    educationSlice->setLabel(QString("Education %1%").arg(educationPercentage, 0, 'f', 1));

    QPieSlice *housingSlice = series->append("Housing", housingPercentage);
    housingSlice->setLabel(QString("Housing %1%").arg(housingPercentage, 0, 'f', 1));

    QPieSlice *insuranceSlice = series->append("Insurance", insurancePercentage);
    insuranceSlice->setLabel(QString("Insurance %1%").arg(insurancePercentage, 0, 'f', 1));

    QPieSlice *othersSlice = series->append("Others", othersPercentage);
    othersSlice->setLabel(QString("Others %1%").arg(othersPercentage, 0, 'f', 1));

    if (totalBalance > 0) {
        QPieSlice *savingsSlice = series->append("Savings", savingsPercentage);
        savingsSlice->setLabel(QString("Savings %1%").arg(savingsPercentage, 0, 'f', 1));
    }

    // Create the chart and set the series
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Expenses Breakdown");

    // Create the chart view and set the chart
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Display the chart in a layout or replace an existing widget
    ui->chartLayout->addWidget(chartView); // Assuming you have a layout named chartLayout in your UI
}


