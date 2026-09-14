# Makefile for EDM Bus Reservation System
CC     = gcc
CFLAGS = -Wall -Wextra -g

all: edm.server edm.client

edm.server: BusServer.c
	$(CC) $(CFLAGS) -o edm.server BusServer.c

edm.client: BusClient.c
	$(CC) $(CFLAGS) -o edm.client BusClient.c

clean:
	rm -f edm.server edm.client