BUILD=exe
ODIR=.obj
OUT=hammer

all: $(OUT)
.PHONY: clean
CFLAGS= -msse4.2 -ggdb
OBJECTS := $(patsubst %.cc, $(ODIR)/%.o, $(wildcard *.cc))

$(ODIR)/%.o: %.cc
	mkdir -p $(ODIR)
	g++ -o $@ -c $< $(CFLAGS)


$(OUT): $(OBJECTS)
	mkdir -p $(BUILD)
	g++ -o $(BUILD)/$@ $^ $(CFLAGS)
	chmod +x $(BUILD)/$@
clean:
	rm -rf $(BUILD)
	rm -rf $(ODIR)
	rm -rf *.log


