MODULE_big = pg_hashids
OBJS = pg_hashids.o hashids.o

REGRESS = pg_hashids

EXTENSION = pg_hashids
DATA =	pg_hashids--1.3.sql \
	pg_hashids--1.2.1--1.3.sql \
	pg_hashids--1.2--1.3.sql \
	pg_hashids--1.1--1.2.sql \
	pg_hashids--1.0--1.1.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
