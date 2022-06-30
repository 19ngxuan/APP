#! /bin/sh

g++ ec_gui.cpp -L/usr/local/lib -lfltk \
   -lfltk_images -lfltk_jpeg -lfltk_png \
   -lfltk -lz \
   -lpthread -framework Cocoa -std=c++2a \
   ../obj/ec_helper.o ../obj/ec_tokenizer.o \
   ../obj/ec_rules.o ../obj/ec_knowledgebase.o \
   ../obj/ec_parser.o ../obj/ec_inferenceengine.o \
   ec_fltkgui.cpp 
