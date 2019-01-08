HPCOMBI_CONFIG = -DHPCOMBI_CONSTEXPR_FUN_ARGS
HPCOMBI_INC=-I../extern/HPCombi/include -I../extern/HPCombi/include/fallback

CXXFLAGS    = -fcilkplus -std=c++11 -g -Wall -O3 -funroll-loops -mavx -mtune=native -flax-vector-conversions ${HPCOMBI_INC} ${HPCOMBI_CONFIG}

PROGS = filter rsincl filter2 printmat gossip
all: ${PROGS}

clean:
	rm -rf ${PROGS}
