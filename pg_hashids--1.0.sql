-- use CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_hashids" to load this file. \quit

CREATE OR REPLACE FUNCTION hash_encode(BIGINT) RETURNS TEXT
  AS 'pg_hashids' LANGUAGE C;
CREATE OR REPLACE FUNCTION hash_encode(BIGINT, TEXT) RETURNS TEXT
  AS 'pg_hashids', 'hash_encode_salt' LANGUAGE C;
CREATE OR REPLACE FUNCTION hash_encode(BIGINT, TEXT, INT) RETURNS TEXT
  AS 'pg_hashids', 'hash_encode_salt' LANGUAGE C;
CREATE OR REPLACE FUNCTION hash_decode(TEXT, TEXT, INT) RETURNS INT
  AS 'pg_hashids', 'hash_decode' LANGUAGE C;