MODULE_big = pg_hashids
OBJS       = pg_hashids.o hashids.o

EXTENSION = pg_hashids
DATA = pg_hashids--1.0.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
