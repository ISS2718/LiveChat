CC := gcc
OUTFILE := a
OUTDIR := bin
OBJDIR := $(OUTDIR)/obj

# Flags comuns para todos os sistemas
COMMON_FLAGS := -march=native -O3

ifeq ($(OS), Windows_NT)
	CFLAGS := -lglfw3.dll -lglew32 -lopengl32
	OUTFILE := $(OUTFILE)_win
	OBJ_EXT := win.o
else
	UNAME := $(shell uname)
	ifeq ($(UNAME), Linux)
		CFLAGS := -lglfw -lGLEW -lGL -lm
		OUTFILE := $(OUTFILE)_linux.out
		OBJ_EXT := linux.o
	endif
endif

# Flags de depuração
DBFLAGS := -Wall -g3

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.$(OBJ_EXT),$(SRCS))

run: $(OUTDIR)/$(OUTFILE)
	$(OUTDIR)/$(OUTFILE)

rund: CFLAGS += $(COMMON_FLAGS) $(DBFLAGS)
rund: $(OUTDIR)/$(OUTFILE)_dbg
	$(OUTDIR)/$(OUTFILE)_dbg

release: CFLAGS += $(COMMON_FLAGS)
release: $(OUTDIR) $(OBJDIR) $(OUTDIR)/$(OUTFILE)

debug: CFLAGS += $(COMMON_FLAGS) $(DBFLAGS)
debug: $(OUTDIR) $(OBJDIR) $(OUTDIR)/$(OUTFILE)_dbg

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.$(OBJ_EXT): %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTDIR)/$(OUTFILE): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

$(OUTDIR)/$(OUTFILE)_dbg: $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -rf $(OUTDIR)

.PHONY: run rund release debug clean