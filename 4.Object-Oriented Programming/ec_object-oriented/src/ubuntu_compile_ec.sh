#! /bin/sh

g++ ec_gui.cpp -std=c++2a\
   -L/usr/local/lib -lfltk -lm -lX11 -lpthread -lm -ldl \
   ../obj/ec_helper.o ../obj/ec_tokenizer.o \
   ../obj/ec_rules.o ../obj/ec_knowledgebase.o \
   ../obj/ec_parser.o ../obj/ec_inferenceengine.o \
   ec_fltkgui.cpp \
   -lfmt -o ugui
