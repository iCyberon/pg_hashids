\set VERBOSITY terse
CREATE EXTENSION pg_hashids;

-- Encoding tests
SELECT id_encode(1001); -- Result: jNl
SELECT id_encode(1234567, 'This is my salt'); -- Result: Pdzxp
SELECT id_encode(1234567, 'This is my salt', 10); -- Result: PlRPdzxpR7
SELECT id_encode(1234567, 'This is my salt', 10, 'abcdefghijABCDxFGHIJ1234567890'); -- Result: 3GJ956J9B9

-- Decoding tests
SELECT id_decode('jNl'); -- Result: {1001}
SELECT id_decode('Pdzxp', 'This is my salt'); -- Result: {1234567}
SELECT id_decode('PlRPdzxpR7', 'This is my salt', 10); -- Result: {1234567}
SELECT id_decode('3GJ956J9B9', 'This is my salt', 10, 'abcdefghijABCDxFGHIJ1234567890'); -- Result: {1234567}

SELECT id_decode_once('jNl'); -- Result: 1001
SELECT id_decode_once('Pdzxp', 'This is my salt'); -- Result: 1234567
SELECT id_decode_once('PlRPdzxpR7', 'This is my salt', 10); -- Result: 1234567
SELECT id_decode_once('3GJ956J9B9', 'This is my salt', 10, 'abcdefghijABCDxFGHIJ1234567890'); -- Result: 1234567

