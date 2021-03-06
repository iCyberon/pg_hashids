\echo Use "CREATE EXTENSION pg_hashids" to load this file. \quit

-- v1
CREATE OR REPLACE FUNCTION hash_encode(BIGINT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION hash_encode(BIGINT, TEXT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION hash_encode(BIGINT, TEXT, INT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION hash_decode(TEXT, TEXT, INT) RETURNS INT
  AS 'MODULE_PATHNAME', 'id_decode_once' LANGUAGE C IMMUTABLE STRICT;

-- v1.3
CREATE OR REPLACE FUNCTION id_encode(BIGINT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_encode(BIGINT, TEXT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_encode(BIGINT, TEXT, INT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_encode(BIGINT, TEXT, INT, TEXT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION id_encode(BIGINT[]) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode_array' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_encode(BIGINT[], TEXT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode_array' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_encode(BIGINT[], TEXT, INT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode_array' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_encode(BIGINT[], TEXT, INT, TEXT) RETURNS TEXT
  AS 'MODULE_PATHNAME', 'id_encode_array' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION id_decode(TEXT) RETURNS BIGINT[]
  AS 'MODULE_PATHNAME', 'id_decode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_decode(TEXT, TEXT) RETURNS BIGINT[]
  AS 'MODULE_PATHNAME', 'id_decode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_decode(TEXT, TEXT, INT) RETURNS BIGINT[]
  AS 'MODULE_PATHNAME', 'id_decode' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_decode(TEXT, TEXT, INT, TEXT) RETURNS BIGINT[]
  AS 'MODULE_PATHNAME', 'id_decode' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION id_decode_once(TEXT) RETURNS BIGINT
  AS 'MODULE_PATHNAME', 'id_decode_once' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_decode_once(TEXT, TEXT) RETURNS BIGINT
  AS 'MODULE_PATHNAME', 'id_decode_once' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_decode_once(TEXT, TEXT, INT) RETURNS BIGINT
  AS 'MODULE_PATHNAME', 'id_decode_once' LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION id_decode_once(TEXT, TEXT, INT, TEXT) RETURNS BIGINT
  AS 'MODULE_PATHNAME', 'id_decode_once' LANGUAGE C IMMUTABLE STRICT;
