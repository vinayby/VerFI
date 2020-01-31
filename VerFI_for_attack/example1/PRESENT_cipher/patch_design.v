module Cipher (data_o, data_o_valid, data_i, key_i, data_load,key_load, clk_i, rst, cointoss);
    input [79:0] data_i;
    input [79:0] key_i;
    input data_load, key_load, clk_i, rst, cointoss;
    output [63:0] data_o;
    output        data_o_valid;
