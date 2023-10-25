/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Project;
    QAction *actionOpen_Project;
    QAction *actionSave_Project;
    QAction *actionSave_Project_As;
    QAction *actionExport_to_PDF;
    QAction *actionView_JSON;
    QAction *actionExport_JSON;
    QAction *actionClose_Project;
    QAction *actionExit;
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab_1;
    QTabWidget *spc_gravity_tabs;
    QWidget *tab_1_1;
    QWidget *tab_1_2;
    QWidget *tab_1_3;
    QWidget *tab_1_4;
    QPushButton *pushButton_2;
    QToolBox *info_toolBox;
    QWidget *basic_info;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTextEdit *textEdit;
    QSpacerItem *side_socr_1;
    QLabel *label_2;
    QTextEdit *textEdit_2;
    QSpacerItem *side_spcr_2;
    QLabel *label_3;
    QTextEdit *textEdit_3;
    QSpacerItem *side_spcr_3;
    QLabel *label_4;
    QTextEdit *textEdit_4;
    QWidget *exp_info;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_5;
    QDateEdit *dateEdit;
    QSpacerItem *verticalSpacer;
    QLabel *label_6;
    QDateEdit *dateEdit_2;
    QSpacerItem *verticalSpacer_2;
    QFormLayout *formLayout;
    QLabel *label_7;
    QSpinBox *spinBox;
    QLabel *label_8;
    QLineEdit *lineEdit;
    QLabel *label_9;
    QLineEdit *lineEdit_2;
    QLabel *label_10;
    QLineEdit *lineEdit_3;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *verticalSpacer_5;
    QPushButton *pushButton;
    QWidget *tab_2;
    QMenuBar *menubar;
    QMenu *menuHome;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1680, 860);
        actionNew_Project = new QAction(MainWindow);
        actionNew_Project->setObjectName(QString::fromUtf8("actionNew_Project"));
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        actionSave_Project = new QAction(MainWindow);
        actionSave_Project->setObjectName(QString::fromUtf8("actionSave_Project"));
        actionSave_Project_As = new QAction(MainWindow);
        actionSave_Project_As->setObjectName(QString::fromUtf8("actionSave_Project_As"));
        actionExport_to_PDF = new QAction(MainWindow);
        actionExport_to_PDF->setObjectName(QString::fromUtf8("actionExport_to_PDF"));
        actionView_JSON = new QAction(MainWindow);
        actionView_JSON->setObjectName(QString::fromUtf8("actionView_JSON"));
        actionExport_JSON = new QAction(MainWindow);
        actionExport_JSON->setObjectName(QString::fromUtf8("actionExport_JSON"));
        actionClose_Project = new QAction(MainWindow);
        actionClose_Project->setObjectName(QString::fromUtf8("actionClose_Project"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 1681, 771));
        tabWidget->setTabPosition(QTabWidget::West);
        tabWidget->setTabsClosable(true);
        tab_1 = new QWidget();
        tab_1->setObjectName(QString::fromUtf8("tab_1"));
        spc_gravity_tabs = new QTabWidget(tab_1);
        spc_gravity_tabs->setObjectName(QString::fromUtf8("spc_gravity_tabs"));
        spc_gravity_tabs->setGeometry(QRect(-4, 39, 1411, 731));
        tab_1_1 = new QWidget();
        tab_1_1->setObjectName(QString::fromUtf8("tab_1_1"));
        spc_gravity_tabs->addTab(tab_1_1, QString());
        tab_1_2 = new QWidget();
        tab_1_2->setObjectName(QString::fromUtf8("tab_1_2"));
        spc_gravity_tabs->addTab(tab_1_2, QString());
        tab_1_3 = new QWidget();
        tab_1_3->setObjectName(QString::fromUtf8("tab_1_3"));
        spc_gravity_tabs->addTab(tab_1_3, QString());
        tab_1_4 = new QWidget();
        tab_1_4->setObjectName(QString::fromUtf8("tab_1_4"));
        pushButton_2 = new QPushButton(tab_1_4);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 670, 89, 25));
        spc_gravity_tabs->addTab(tab_1_4, QString());
        info_toolBox = new QToolBox(tab_1);
        info_toolBox->setObjectName(QString::fromUtf8("info_toolBox"));
        info_toolBox->setGeometry(QRect(1410, 0, 231, 761));
        basic_info = new QWidget();
        basic_info->setObjectName(QString::fromUtf8("basic_info"));
        basic_info->setGeometry(QRect(0, 0, 231, 699));
        layoutWidget = new QWidget(basic_info);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 10, 258, 691));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        textEdit = new QTextEdit(layoutWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout->addWidget(textEdit);

        side_socr_1 = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(side_socr_1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        textEdit_2 = new QTextEdit(layoutWidget);
        textEdit_2->setObjectName(QString::fromUtf8("textEdit_2"));

        verticalLayout->addWidget(textEdit_2);

        side_spcr_2 = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(side_spcr_2);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        textEdit_3 = new QTextEdit(layoutWidget);
        textEdit_3->setObjectName(QString::fromUtf8("textEdit_3"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textEdit_3->sizePolicy().hasHeightForWidth());
        textEdit_3->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(textEdit_3);

        side_spcr_3 = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(side_spcr_3);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        textEdit_4 = new QTextEdit(layoutWidget);
        textEdit_4->setObjectName(QString::fromUtf8("textEdit_4"));

        verticalLayout->addWidget(textEdit_4);

        info_toolBox->addItem(basic_info, QString::fromUtf8("Basic Info"));
        exp_info = new QWidget();
        exp_info->setObjectName(QString::fromUtf8("exp_info"));
        exp_info->setGeometry(QRect(0, 0, 231, 699));
        verticalLayoutWidget = new QWidget(exp_info);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(9, 10, 221, 691));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(verticalLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(label_5);

        dateEdit = new QDateEdit(verticalLayoutWidget);
        dateEdit->setObjectName(QString::fromUtf8("dateEdit"));

        verticalLayout_2->addWidget(dateEdit);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer);

        label_6 = new QLabel(verticalLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy1.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(label_6);

        dateEdit_2 = new QDateEdit(verticalLayoutWidget);
        dateEdit_2->setObjectName(QString::fromUtf8("dateEdit_2"));

        verticalLayout_2->addWidget(dateEdit_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer_2);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
        formLayout->setVerticalSpacing(6);
        formLayout->setContentsMargins(-1, 0, -1, -1);
        label_7 = new QLabel(verticalLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_7);

        spinBox = new QSpinBox(verticalLayoutWidget);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, spinBox);

        label_8 = new QLabel(verticalLayoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_8);

        lineEdit = new QLineEdit(verticalLayoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEdit);

        label_9 = new QLabel(verticalLayoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_9);

        lineEdit_2 = new QLineEdit(verticalLayoutWidget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        formLayout->setWidget(4, QFormLayout::FieldRole, lineEdit_2);

        label_10 = new QLabel(verticalLayoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_10);

        lineEdit_3 = new QLineEdit(verticalLayoutWidget);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));

        formLayout->setWidget(6, QFormLayout::FieldRole, lineEdit_3);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        formLayout->setItem(1, QFormLayout::FieldRole, verticalSpacer_3);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        formLayout->setItem(3, QFormLayout::FieldRole, verticalSpacer_4);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        formLayout->setItem(5, QFormLayout::FieldRole, verticalSpacer_5);


        verticalLayout_2->addLayout(formLayout);

        info_toolBox->addItem(exp_info, QString::fromUtf8("Experiment Info"));
        pushButton = new QPushButton(tab_1);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 10, 71, 25));
        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1680, 22));
        menuHome = new QMenu(menubar);
        menuHome->setObjectName(QString::fromUtf8("menuHome"));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuHome->menuAction());
        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menuFile->addAction(actionNew_Project);
        menuFile->addAction(actionOpen_Project);
        menuFile->addSeparator();
        menuFile->addAction(actionSave_Project);
        menuFile->addAction(actionSave_Project_As);
        menuFile->addAction(actionExport_JSON);
        menuFile->addSeparator();
        menuFile->addAction(actionExport_to_PDF);
        menuFile->addSeparator();
        menuFile->addAction(actionClose_Project);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);
        spc_gravity_tabs->setCurrentIndex(3);
        info_toolBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionNew_Project->setText(QCoreApplication::translate("MainWindow", "New Project", nullptr));
        actionOpen_Project->setText(QCoreApplication::translate("MainWindow", "Open Project", nullptr));
        actionSave_Project->setText(QCoreApplication::translate("MainWindow", "Save Project", nullptr));
        actionSave_Project_As->setText(QCoreApplication::translate("MainWindow", "Save Project As...", nullptr));
        actionExport_to_PDF->setText(QCoreApplication::translate("MainWindow", "Export to PDF", nullptr));
        actionView_JSON->setText(QCoreApplication::translate("MainWindow", "View JSON", nullptr));
        actionExport_JSON->setText(QCoreApplication::translate("MainWindow", "Export JSON", nullptr));
        actionClose_Project->setText(QCoreApplication::translate("MainWindow", "Close Project Without Saving", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        spc_gravity_tabs->setTabText(spc_gravity_tabs->indexOf(tab_1_1), QCoreApplication::translate("MainWindow", "40MM", nullptr));
        spc_gravity_tabs->setTabText(spc_gravity_tabs->indexOf(tab_1_2), QCoreApplication::translate("MainWindow", "20MM", nullptr));
        spc_gravity_tabs->setTabText(spc_gravity_tabs->indexOf(tab_1_3), QCoreApplication::translate("MainWindow", "10MM", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "ADD", nullptr));
        spc_gravity_tabs->setTabText(spc_gravity_tabs->indexOf(tab_1_4), QCoreApplication::translate("MainWindow", "Stone Dust", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Name of the Work", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Name of the Client", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Name of the Consultant", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Name of the Contractor", nullptr));
        info_toolBox->setItemText(info_toolBox->indexOf(basic_info), QCoreApplication::translate("MainWindow", "Basic Info", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Date of Sampling", nullptr));
        dateEdit->setDisplayFormat(QCoreApplication::translate("MainWindow", "dd/mm/yyyy", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Date of Testing", nullptr));
        dateEdit_2->setDisplayFormat(QCoreApplication::translate("MainWindow", "dd/mm/yyyy", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Sample no.", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Source of Material", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Type of Material", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Sample At", nullptr));
        info_toolBox->setItemText(info_toolBox->indexOf(exp_info), QCoreApplication::translate("MainWindow", "Experiment Info", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Save ", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QCoreApplication::translate("MainWindow", "Specific Gravity", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Tab 2", nullptr));
        menuHome->setTitle(QCoreApplication::translate("MainWindow", "Home", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
