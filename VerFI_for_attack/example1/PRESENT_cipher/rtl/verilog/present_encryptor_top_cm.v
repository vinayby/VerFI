module present_encryptor_top_cm(data_o, data_o_valid, data_i, key_i, data_load,key_load,clk_i, rst, cointoss);
output wire[63:0] data_o; // ciphertext will appear here
output wire       data_o_valid;
input wire cointoss;
input  wire[79:0] data_i; // plaintext and key must be fed here
input  wire[79:0] key_i; // plaintext and key must be fed here
input  wire clk_i; // clock signal
input wire rst;
input  wire key_load; // when '1', data_i will loaded into key register
input  wire data_load; // when '1', first 64 bits of data_i will be loaded into state register

wire[63:0] data_o_A; // ciphertext will appear here
wire[63:0] data_o_R; // ciphertext will appear here
wire data_o_valid_A, data_o_valid_R;

present_encryptor_top UUTA (.data_o(data_o_A), .data_o_valid (data_o_valid_A), .data_i(data_i),.key_i(key_i),.data_load(data_load),.key_load(key_load),.clk_i(clk_i), .rst(rst), .cointoss(cointoss));

present_encryptor_top UUTR (.data_o(data_o_R), .data_o_valid (data_o_valid_R), .data_i(data_i),.key_i(key_i),.data_load(data_load),.key_load(key_load),.clk_i(clk_i), .rst(rst), .cointoss(cointoss));

assign data_o = data_o_A ^ data_o_R  ? 0 : data_o_A;
assign data_o_valid = !((data_o_valid_A == 0) && (data_o_valid_R == 0)); // neg level done

endmodule 
