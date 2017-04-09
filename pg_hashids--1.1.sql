-- use CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_hashids" to load this file. \quit

-- v1
CREATE OR REPLACE FUNCTION hash_encode(BIGINT) RETURNS TEXT
  AS 'pg_hashids', 'id_encode' LANGUAGE C;
CREATE OR REPLACE FUNCTION hash_encode(BIGINT, TEXT) RETURNS TEXT
  AS 'pg_hashids', 'id_encode_salt' LANGUAGE C;
CREATE OR REPLACE FUNCTION hash_encode(BIGINT, TEXT, INT) RETURNS TEXT
  AS 'pg_hashids', 'id_encode_salt' LANGUAGE C;

CREATE OR REPLACE FUNCTION hash_decode(TEXT, TEXT, INT) RETURNS INT
  AS 'pg_hashids', 'id_decode' LANGUAGE C;

-- v1.1
CREATE OR REPLACE FUNCTION id_encode(BIGINT) RETURNS TEXT
  AS 'pg_hashids', 'id_encode' LANGUAGE C;
CREATE OR REPLACE FUNCTION id_encode(BIGINT, TEXT) RETURNS TEXT
  AS 'pg_hashids', 'id_encode_salt' LANGUAGE C;
CREATE OR REPLACE FUNCTION id_encode(BIGINT, TEXT, INT) RETURNS TEXT
  AS 'pg_hashids', 'id_encode_salt' LANGUAGE C;
CREATE OR REPLACE FUNCTION id_encode(BIGINT, TEXT, INT, TEXT) RETURNS TEXT
  AS 'pg_hashids', 'id_encode_salt_alphabet' LANGUAGE C;

CREATE OR REPLACE FUNCTION id_decode(TEXT, TEXT, INT) RETURNS INT
  AS 'pg_hashids', 'id_decode' LANGUAGE C;
CREATE OR REPLACE FUNCTION id_decode(TEXT, TEXT, INT, TEXT) RETURNS INT
  AS 'pg_hashids', 'id_decode' LANGUAGE C;