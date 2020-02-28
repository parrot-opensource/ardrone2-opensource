LDFLAGS +=-lusb

SRC= $(wildcard *.c)
OBJ:= $(SRC:.c=.o)
OBJ:= $(addprefix $(DESTDIR)/, $(OBJ))

all: $(DESTDIR)/lsusb

$(DESTDIR)/%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(DESTDIR)/lsusb: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(DESTDIR)/lsusb $(OBJ)
