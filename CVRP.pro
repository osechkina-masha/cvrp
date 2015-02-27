TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS+=-Wl,--no-as-needed
QMAKE_CXXFLAGS+=-pthread
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -Wl,--no-as-needed
QMAKE_LFLAGS += -pthread
QMAKE_LFLAGS += -std=c++11

INCLUDEPATH += "/usr/include/php5"
INCLUDEPATH += "/usr/include/php5/main"
INCLUDEPATH += "/usr/include/php5/Zend"
INCLUDEPATH += "/usr/include/php5/TSRM"

DEFINES += WITH_LOG
#DEFINES += WITH_DEBUG

SOURCES += main.cpp \
    route.cpp \
    unsplitroute.cpp \
    clarkewrightalgorithm.cpp \
    filereader.cpp \
    truck.cpp \
    customer.cpp \
    routermanager.cpp \
    routeinfo.cpp \
    #cvrp.cpp \
    costcalculator.cpp \
    restrictionchecker.cpp \
    #capacity.cpp
    velocitycalculator.cpp \
    adopter.cpp \
    progress.cpp

HEADERS += \
    customer.h \
    route.h \
    truck.h \
    unsplitroute.h \
    clarkewrightalgorithm.h \
    customerPairInfo.h \
    filereader.h \
    routermanager.h \
    routeinfo.h \
    #php_CVRP.h
    costcalculator.h \
    restrictionchecker.h \
    #capacity.h
    adopter.h \
    velocitycalculator.h \
    progress.h

OTHER_FILES += \
    config.m4

