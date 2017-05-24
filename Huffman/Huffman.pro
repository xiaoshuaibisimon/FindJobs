TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    list.c \
    pqueue.c \
    bit.c \
    bitree.c \
    heap.c \
    huffman.c

HEADERS += \
    list.h \
    pqueue.h \
    bit.h \
    bitree.h \
    heap.h \
    huffman.h
