/********************************************************************************
** Form generated from reading UI file 'entry.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTRY_H
#define UI_ENTRY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_entry
{
public:
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
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;

    void setupUi(QWidget *entry)
    {
        if (entry->objectName().isEmpty())
            entry->setObjectName(QString::fromUtf8("entry"));
        entry->resize(1680, 860);
        info_toolBox = new QToolBox(entry);
        info_toolBox->setObjectName(QString::fromUtf8("info_toolBox"));
        info_toolBox->setGeometry(QRect(1360, 10, 231, 761));
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
        pushButton = new QPushButton(entry);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(40, 30, 89, 25));
        pushButton_2 = new QPushButton(entry);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(150, 30, 89, 25));
        pushButton_3 = new QPushButton(entry);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(260, 30, 141, 25));

        retranslateUi(entry);

        info_toolBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(entry);
    } // setupUi

    void retranslateUi(QWidget *entry)
    {
        entry->setWindowTitle(QCoreApplication::translate("entry", "Form", nullptr));
        label->setText(QCoreApplication::translate("entry", "Name of the Work", nullptr));
        label_2->setText(QCoreApplication::translate("entry", "Name of the Client", nullptr));
        label_3->setText(QCoreApplication::translate("entry", "Name of the Consultant", nullptr));
        label_4->setText(QCoreApplication::translate("entry", "Name of the Contractor", nullptr));
        info_toolBox->setItemText(info_toolBox->indexOf(basic_info), QCoreApplication::translate("entry", "Basic Info", nullptr));
        label_5->setText(QCoreApplication::translate("entry", "Date of Sampling", nullptr));
        dateEdit->setDisplayFormat(QCoreApplication::translate("entry", "dd/mm/yyyy", nullptr));
        label_6->setText(QCoreApplication::translate("entry", "Date of Testing", nullptr));
        dateEdit_2->setDisplayFormat(QCoreApplication::translate("entry", "dd/mm/yyyy", nullptr));
        label_7->setText(QCoreApplication::translate("entry", "Sample no.", nullptr));
        label_8->setText(QCoreApplication::translate("entry", "Source of Material", nullptr));
        label_9->setText(QCoreApplication::translate("entry", "Type of Material", nullptr));
        label_10->setText(QCoreApplication::translate("entry", "Sample At", nullptr));
        info_toolBox->setItemText(info_toolBox->indexOf(exp_info), QCoreApplication::translate("entry", "Experiment Info", nullptr));
        pushButton->setText(QCoreApplication::translate("entry", "Save", nullptr));
        pushButton_2->setText(QCoreApplication::translate("entry", "Save As", nullptr));
        pushButton_3->setText(QCoreApplication::translate("entry", "Export to PDF", nullptr));
    } // retranslateUi

};

namespace Ui {
    class entry: public Ui_entry {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTRY_H
