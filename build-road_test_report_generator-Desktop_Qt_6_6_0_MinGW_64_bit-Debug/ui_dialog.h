/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QTextBrowser *textBrowser;
    QLabel *label_3;
    QPushButton *pushButton_5;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(1680, 860);
        pushButton = new QPushButton(Dialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(18, 150, 211, 41));
        pushButton_2 = new QPushButton(Dialog);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(20, 220, 211, 41));
        pushButton_3 = new QPushButton(Dialog);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(20, 280, 211, 41));
        pushButton_4 = new QPushButton(Dialog);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setGeometry(QRect(20, 340, 211, 41));
        textBrowser = new QTextBrowser(Dialog);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setGeometry(QRect(30, 400, 171, 31));
        label_3 = new QLabel(Dialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(26, 449, 261, 261));
        pushButton_5 = new QPushButton(Dialog);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setGeometry(QRect(30, 750, 161, 25));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        pushButton->setText(QCoreApplication::translate("Dialog", "New Project", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Dialog", "Open Projects", nullptr));
        pushButton_3->setText(QCoreApplication::translate("Dialog", "Save Projects", nullptr));
        pushButton_4->setText(QCoreApplication::translate("Dialog", "Export to PDF", nullptr));
        textBrowser->setHtml(QCoreApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Donate (if you like it!)</p></body></html>", nullptr));
        label_3->setText(QString());
        pushButton_5->setText(QCoreApplication::translate("Dialog", "EXIT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
