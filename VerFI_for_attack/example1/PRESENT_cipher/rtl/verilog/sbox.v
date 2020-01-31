//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// Design Name: Present Cipher S-BOX                              //
// Module Name: sbox                                              //
// Language:    Verilog                                           //
// Date Created: 1/16/2011                                        //
// Author: Reza Ameli                                             //
//         Digital Systems Lab                                    //
//         Ferdowsi University of Mashhad, Iran                   //
//         http://commeng.um.ac.ir/dslab                          //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//                                                                //
// This source file may be used and distributed without           //
// restriction provided that this copyright statement is not      //
// removed from the file and that any derivative work contains    //
// the original copyright notice and the associated disclaimer.   //
//                                                                //
// This source file is free software; you can redistribute it     //
// and/or modify it under the terms of the GNU Lesser General     //
// Public License as published by the Free Software Foundation;   //
// either version 2.1 of the License, or (at your option) any     //
// later version.                                                 //
//                                                                //
// This source is distributed in the hope that it will be         //
// useful, but WITHOUT ANY WARRANTY; without even the implied     //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR        //
// PURPOSE. See the GNU Lesser General Public License for more    //
// details.                                                       //
//                                                                //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

module sbox (data_o,data_i);

//- Module IOs ----------------------------------------------------------------

output reg [3:0] data_o;
input  wire [4:0] data_i;

//- Continuous Assigments------------------------------------------------------

always @(data_i)
    case (data_i)
        5'h0 : data_o = 4'hC;
        5'h1 : data_o = 4'h5;
        5'h2 : data_o = 4'h6;
        5'h3 : data_o = 4'hB;
        5'h4 : data_o = 4'h9;
        5'h5 : data_o = 4'h0;
        5'h6 : data_o = 4'hA;
        5'h7 : data_o = 4'hD;
        5'h8 : data_o = 4'h3;
        5'h9 : data_o = 4'hE;
        5'hA : data_o = 4'hF;
        5'hB : data_o = 4'h8;
        5'hC : data_o = 4'h4;
        5'hD : data_o = 4'h7;
        5'hE : data_o = 4'h1;
        5'hF : data_o = 4'h2;
        5'h10 : data_o = 4'hD;
        5'h11 : data_o = 4'hE;
        5'h12 : data_o = 4'h8;
        5'h13 : data_o = 4'hB;
        5'h14 : data_o = 4'h7;
        5'h15 : data_o = 4'h0;
        5'h16 : data_o = 4'h1;
        5'h17 : data_o = 4'hC;
        5'h18 : data_o = 4'h2;
        5'h19 : data_o = 4'h5;
        5'h1A : data_o = 4'hF;
        5'h1B : data_o = 4'h6;
        5'h1C : data_o = 4'h4;
        5'h1D : data_o = 4'h9;
        5'h1E : data_o = 4'ha;
        5'h1F : data_o = 4'h3;
    endcase 

//-----------------------------------------------------------------------------
endmodule
