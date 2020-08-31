# Project: M17_UDP_listener

CPP      = g++
CC       = gcc
WINDRES  = 
OBJ      = main.o codebook.o codebookd.o codebookdt.o codebookge.o codebookjvm.o codebooklspmelvq.o codebookmel.o codebooknewamp1.o codebooknewamp1_energy.o codebookres.o codebookvq.o codec2.o codec2_fft.o dump.o gp_interleaver.o interldpc.o interp.o kiss_fft.o kiss_fftr.o lpc.o lsp.o mbest.o modem_stats.o mpdecode_core.o newamp1.o nlp.o ofdm.o pack.o phase.o postfilter.o quantise.o sine.o
LINKOBJ  = main.o codebook.o codebookd.o codebookdt.o codebookge.o codebookjvm.o codebooklspmelvq.o codebookmel.o codebooknewamp1.o codebooknewamp1_energy.o codebookres.o codebookvq.o codec2.o codec2_fft.o dump.o gp_interleaver.o interp.o kiss_fft.o kiss_fftr.o lpc.o lsp.o mbest.o modem_stats.o mpdecode_core.o newamp1.o nlp.o ofdm.o pack.o phase.o postfilter.o quantise.o sine.o
LIBS     = -static-libgcc -lm
INCS     = 
CXXINCS  = 
BIN      = m17_udp_listener
CXXFLAGS = $(CXXINCS) -march=native -msse3 -O2 -std=gnu99
CFLAGS   = $(INCS) -march=native -msse3 -O2 -std=gnu99
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@ 

