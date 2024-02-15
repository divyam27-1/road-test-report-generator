QT       += widgets core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../executable/wkhtmltopdf \
    ../executable/wkhtmltopdf.exe \
    ../templates/aiv.html \
    ../templates/bld.html \
    ../templates/cmb.html \
    ../templates/fei.html \
    ../templates/ind.html \
    ../templates/spc.html \
    ../templates/summary.html

QMAKE_CXXFLAGS += -Wa,-mbig-obj

RESOURCES += \
    html_templates.qrc \
    icons.qrc
