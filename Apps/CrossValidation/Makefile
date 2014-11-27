EXEC = CrossValidation
CFLAGS = -O3 -I../../BaseLibrary  -I../../PicesLibraries/PicesLibrary  -I../../PicesLibraries/SipperInstruments
# LIBS = -lPicesLibrary   -lSipperInstruments    -lBaseLibrary   -lfftw3 -lrt 

LIBS = -lPicesLibrary   -lSipperInstruments    -lBaseLibrary    -lmysqlclient -lrt

LIBDIR = -L../../BaseLibrary  -L../../PicesLibraries/SipperInstruments  -L../../PicesLibraries/PicesLibrary  -L/usr/lib64/mysql/

CC = g++

OBJECTS	 = CrossValidation.o

$(EXEC): $(OBJECTS)  
	$(CC) -o $(EXEC) $(OBJECTS) $(LIBS) $(LIBDIR)

CrossValidation.o :    CrossValidation.cpp
	$(CC) $(CFLAGS)   -c ./CrossValidation.cpp


