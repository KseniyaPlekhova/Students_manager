#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QComboBox>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    // Подключение к бд
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Connection", "Failed to connect to the database.");
        return;
    }
    QMessageBox::information(this, "Database Connection", "Successfully connected to the database.");


    QSqlTableModel *model = createTableModel();

    QTableView *tableView = createTableView(model);
    QLayout *inputLayout = createInputLayout(model);
    QLayout *deleteLayout = createDeleteLayout(model);
    QLayout *editLayout = createEditLayout(model);
    QLayout *searchLayout = createSearchLayout(model);
    QLayout *sortLayout = createSortLayout(model);
    QLayout *statusEditLayout = createStatusEditLayout(model);

    // Добавить эл-ты в комановку
    mainLayout->addWidget(tableView);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(deleteLayout);
    mainLayout->addLayout(editLayout);
    mainLayout->addLayout(searchLayout);
    mainLayout->addLayout(sortLayout);
     mainLayout->addLayout(statusEditLayout);

    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Student_management");
    db.setUserName("postgres");
    db.setPassword("1825");
    db.setPort(5432);

    if (!db.open()) {
        qDebug() << "Error: " << db.lastError().text();
        return false;
    }
    return true;
}



// Создание модели
QSqlTableModel* MainWindow::createTableModel()
{
    QSqlTableModel *model = new QSqlTableModel(this, db);
    model->setTable("students_view");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    return model;
}

// Создание таблицы
QTableView* MainWindow::createTableView(QSqlTableModel *model)
{
    QTableView *tableView = new QTableView(this);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    return tableView;
}

 // Добавление
QLayout* MainWindow::createInputLayout(QSqlTableModel *model)
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLineEdit *facultyIdInput = new QLineEdit();
    QLineEdit *statusIdInput = new QLineEdit();
    QLineEdit *groupIdInput = new QLineEdit();
    QLineEdit *firstNameInput = new QLineEdit();
    QLineEdit *lastNameInput = new QLineEdit();
    QDateEdit *birthDateInput = new QDateEdit();
    birthDateInput->setCalendarPopup(true);
    birthDateInput->setDisplayFormat("yyyy-MM-dd");

    facultyIdInput->setPlaceholderText("ID факультета");
    statusIdInput->setPlaceholderText("ID статуса");
    groupIdInput->setPlaceholderText("ID группы");
    firstNameInput->setPlaceholderText("Имя");
    lastNameInput->setPlaceholderText("Фамилия");


    QPushButton *addButton = new QPushButton("Добавить");
    layout->addWidget(facultyIdInput);
    layout->addWidget(statusIdInput);
    layout->addWidget(groupIdInput);
    layout->addWidget(firstNameInput);
    layout->addWidget(lastNameInput);
    layout->addWidget(birthDateInput);
    layout->addWidget(addButton);


    connect(addButton, &QPushButton::clicked, this, [=]() {
        QSqlQuery query(db);
        query.prepare("INSERT INTO students (faculty_id, status_id, group_id, first_name, last_name, birth_date) "
                      "VALUES (:faculty_id, :status_id, :group_id, :first_name, :last_name, :birth_date)");
        query.bindValue(":faculty_id", facultyIdInput->text());
        query.bindValue(":status_id", statusIdInput->text());
        query.bindValue(":group_id", groupIdInput->text());
        query.bindValue(":first_name", firstNameInput->text());
        query.bindValue(":last_name", lastNameInput->text());
        query.bindValue(":birth_date", birthDateInput->date().toString("yyyy-MM-dd"));

        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
            return;
        }

        model->select();
        QMessageBox::information(this, "Успешно", "Данные внесены!");

        facultyIdInput->clear();
        statusIdInput->clear();
        groupIdInput->clear();
        firstNameInput->clear();
        lastNameInput->clear();
    });

    return layout;
}



// Удаление
QLayout* MainWindow::createDeleteLayout(QSqlTableModel *model)
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLineEdit *deleteIdInput = new QLineEdit();
    QPushButton *deleteButton = new QPushButton("Удалить");

    deleteIdInput->setPlaceholderText("ID студента");
    layout->addWidget(deleteIdInput);
    layout->addWidget(deleteButton);

    connect(deleteButton, &QPushButton::clicked, this, [=]() {
        QSqlQuery query(db);
        query.prepare("DELETE FROM students WHERE id = :id");
        query.bindValue(":id", deleteIdInput->text());

        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
            return;
        }

        model->select();
        QMessageBox::information(this, "Успешно", "Данные удалены!");
        deleteIdInput->clear();
    });

    return layout;
}

// Редактирование
QLayout* MainWindow::createEditLayout(QSqlTableModel *model)
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLineEdit *editIdInput = new QLineEdit();
    QLineEdit *editFirstNameInput = new QLineEdit();
    QLineEdit *editLastNameInput = new QLineEdit();
    QPushButton *editButton = new QPushButton("Редактировать");

    editIdInput->setPlaceholderText("ID студента");
    editFirstNameInput->setPlaceholderText("Новое имя");
    editLastNameInput->setPlaceholderText("Новая фамилия");
    layout->addWidget(editIdInput);
    layout->addWidget(editFirstNameInput);
    layout->addWidget(editLastNameInput);
    layout->addWidget(editButton);

    connect(editButton, &QPushButton::clicked, this, [=]() {
        QSqlQuery query(db);
        query.prepare("UPDATE students SET first_name = :first_name, last_name = :last_name WHERE id = :id");
        query.bindValue(":id", editIdInput->text());
        query.bindValue(":first_name", editFirstNameInput->text());
        query.bindValue(":last_name", editLastNameInput->text());

        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
            return;
        }

        model->select();
        QMessageBox::information(this, "Успешно", "Данные обновлены!");
        editIdInput->clear();
    });

    return layout;
}

// Поиск
QLayout* MainWindow::createSearchLayout(QSqlTableModel *model)
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLineEdit *searchIdInput = new QLineEdit();
    QPushButton *searchButton = new QPushButton("Поиск по ID");
    QPushButton *resetSearchButton = new QPushButton("Очистить");

    searchIdInput->setPlaceholderText("ID студента");
    layout->addWidget(searchIdInput);
    layout->addWidget(searchButton);
    layout->addWidget(resetSearchButton);

    connect(searchButton, &QPushButton::clicked, this, [=]() {
        model->setFilter(QString("id = %1").arg(searchIdInput->text()));
        model->select();
    });

    connect(resetSearchButton, &QPushButton::clicked, this, [=]() {
        model->setFilter("");
        model->select();
    });

    return layout;
}

// Сортировка
QLayout* MainWindow::createSortLayout(QSqlTableModel *model)
{
    QHBoxLayout *layout = new QHBoxLayout();
    QComboBox *sortComboBox = new QComboBox();
    QPushButton *sortButton = new QPushButton("Сортировка");

    sortComboBox->addItem("Сортировка по имени", "first_name");
    sortComboBox->addItem("Сортировка по фамилии", "last_name");
    sortComboBox->addItem("Сортировка по факультету", "faculty_name");

    layout->addWidget(sortComboBox);
    layout->addWidget(sortButton);

    connect(sortButton, &QPushButton::clicked, this, [=]()
    {
        model->setSort(model->fieldIndex(sortComboBox->currentData().toString()), Qt::AscendingOrder);
        model->select();
    });

    return layout;
}

QLayout* MainWindow::createStatusEditLayout(QSqlTableModel *model)
{
    QHBoxLayout *layout = new QHBoxLayout();


    QLineEdit *editIdInput = new QLineEdit();
    QComboBox *statusComboBox = new QComboBox();
    QPushButton *updateStatusButton = new QPushButton("Обновить статус");

    editIdInput->setPlaceholderText("Введите ID студента");


    QSqlQuery query(db);
    if (query.exec("SELECT id, status_name FROM statuses")) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            statusComboBox->addItem(name, id);
        }
    } else {
        qDebug() << "Ошибка " << query.lastError().text();
    }

    layout->addWidget(editIdInput);
    layout->addWidget(statusComboBox);
    layout->addWidget(updateStatusButton);

    connect(updateStatusButton, &QPushButton::clicked, this, [=]() {
        QString studentId = editIdInput->text();
        QVariant selectedStatusId = statusComboBox->currentData();

        if (studentId.isEmpty() || !selectedStatusId.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите ID студента и выберите статус.");
            return;
        }

        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE students SET status_id = :status_id WHERE id = :id");
        updateQuery.bindValue(":status_id", selectedStatusId);
        updateQuery.bindValue(":id", studentId);

        if (!updateQuery.exec()) {
            QMessageBox::critical(this, "Ошибка базы данных", updateQuery.lastError().text());
            return;
        }

        model->select();
        QMessageBox::information(this, "Успешно", "Статус успешно обновлен!");
        editIdInput->clear();
    });

    return layout;
}
