CC = g++
HADOOP_INSTALL = /usr/local/hadoop
INC = -I$(HADOOP_INSTALL)/include
LIBS = -L$(HADOOP_INSTALL)/lib/native -lhadooppipes -lhadooputils -lpthread -lssl -lcrypto
CPPFLAGS = $(INC) -Wall -g -02

wordcount: wordcount.cpp
	$(CC) $(CPPFLAGS) -o $@ $< $(LIBS)