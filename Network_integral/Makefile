CC = gcc -pthread
CPPFLAGS = -MMD -MP

LIB_SRC_PATH = ./lib/src/
GENERAL_PATH = ./

SOURCES = $(LIB_SRC_PATH)Integral.c $(LIB_SRC_PATH)Connection.c $(GENERAL_PATH)Client.c $(GENERAL_PATH)Server.c
OBJECTS = $(SOURCES:.c=.o)

CLIENT_SRC = Client.c Connection.c Integral.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

SERVER_SRC = Server.c Connection.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)

all: Client Server

$(OBJECTS): $(SOURCES)
	$(CC) $(CPPFLAGS) -c $(SOURCES)

Server: $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o Server

Client: $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o Client -lm

.PHONY: clean
clean:
	rm -rf *.d *.o Server Client
 
-include *.d