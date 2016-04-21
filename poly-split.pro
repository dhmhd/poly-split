QT += core gui widgets

TARGET = poly-split
TEMPLATE = app

SOURCES += \
    main.cpp \
    line.cpp \
    polygon.cpp \
    renderarea.cpp \
    mainwindow.cpp

HEADERS += \
        line.h \
        polygon.h \
        vector.h \
        renderarea.h \
        mainwindow.h

FORMS += mainwindow.ui
