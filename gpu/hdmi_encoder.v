/*
   This file was generated automatically by the Mojo IDE version B1.3.6.
   Do not edit this file directly. Instead edit the original Lucid source.
   This is a temporary file and any changes made to it will be destroyed.
*/

/*
   Parameters:
     PCLK_DIV = 2
     Y_RES = Y_RES
     X_RES = X_RES
     Y_FRAME = 521
     X_FRAME = 800
*/
module hdmi_encoder (
         input clk,
         input rst,
         output reg pclk,
         output reg [3:0] tmds,
         output reg [3:0] tmdsb,
         output reg active,
         output reg [10:0] x,
         output reg [9:0] y,
         input [7:0] red,
         input [7:0] green,
         input [7:0] blue
       );

localparam PCLK_DIV = 2'h2;
localparam Y_RES = 9'h190;
localparam X_RES = 10'h280;
localparam Y_FRAME = 10'h209;
localparam X_FRAME = 10'h320;


reg clkfbin;

wire [1-1:0] M_pll_oserdes_CLKOUT0;
wire [1-1:0] M_pll_oserdes_CLKOUT1;
wire [1-1:0] M_pll_oserdes_CLKOUT2;
wire [1-1:0] M_pll_oserdes_CLKOUT3;
wire [1-1:0] M_pll_oserdes_CLKOUT4;
wire [1-1:0] M_pll_oserdes_CLKOUT5;
wire [1-1:0] M_pll_oserdes_CLKFBOUT;
wire [1-1:0] M_pll_oserdes_LOCKED;
PLL_BASE #(.CLKIN_PERIOD(20), .CLKFBOUT_MULT(10), .CLKOUT0_DIVIDE(2), .CLKOUT1_DIVIDE(20), .CLKOUT2_DIVIDE(10), .COMPENSATION("SOURCE_SYNCHRONOUS")) pll_oserdes (
           .CLKFBIN(clkfbin),
           .CLKIN(clk),
           .RST(1'h0),
           .CLKOUT0(M_pll_oserdes_CLKOUT0),
           .CLKOUT1(M_pll_oserdes_CLKOUT1),
           .CLKOUT2(M_pll_oserdes_CLKOUT2),
           .CLKOUT3(M_pll_oserdes_CLKOUT3),
           .CLKOUT4(M_pll_oserdes_CLKOUT4),
           .CLKOUT5(M_pll_oserdes_CLKOUT5),
           .CLKFBOUT(M_pll_oserdes_CLKFBOUT),
           .LOCKED(M_pll_oserdes_LOCKED)
         );

wire [1-1:0] M_clkfb_buf_O;
BUFG clkfb_buf (
       .I(M_pll_oserdes_CLKFBOUT),
       .O(M_clkfb_buf_O)
     );

always @* begin
  clkfbin = M_clkfb_buf_O;
end

wire [1-1:0] M_pclkx2_buf_O;
BUFG pclkx2_buf (
       .I(M_pll_oserdes_CLKOUT2),
       .O(M_pclkx2_buf_O)
     );

wire [1-1:0] M_pclk_buf_O;
BUFG pclk_buf (
       .I(M_pll_oserdes_CLKOUT1),
       .O(M_pclk_buf_O)
     );

wire [1-1:0] M_ioclk_buf_IOCLK;
wire [1-1:0] M_ioclk_buf_SERDESSTROBE;
wire [1-1:0] M_ioclk_buf_LOCK;
BUFPLL #(.DIVIDE(5)) ioclk_buf (
         .PLLIN(M_pll_oserdes_CLKOUT0),
         .GCLK(M_pclkx2_buf_O),
         .LOCKED(M_pll_oserdes_LOCKED),
         .IOCLK(M_ioclk_buf_IOCLK),
         .SERDESSTROBE(M_ioclk_buf_SERDESSTROBE),
         .LOCK(M_ioclk_buf_LOCK)
       );

reg [10:0] M_ctrX_d, M_ctrX_q = 1'h0;
reg [9:0] M_ctrY_d, M_ctrY_q = 1'h0;

reg hSync;
reg vSync;
reg drawArea;

wire [4-1:0] M_dvi_tmds;
wire [4-1:0] M_dvi_tmdsb;
dvi_encoder_13 dvi (
                 .pclk(M_pclk_buf_O),
                 .pclkx2(M_pclkx2_buf_O),
                 .pclkx10(M_ioclk_buf_IOCLK),
                 .strobe(M_ioclk_buf_SERDESSTROBE),
                 .rst(~M_ioclk_buf_LOCK),
                 .blue(blue),
                 .green(green),
                 .red(red),
                 .hsync(hSync),
                 .vsync(vSync),
                 .de(drawArea),
                 .tmds(M_dvi_tmds),
                 .tmdsb(M_dvi_tmdsb)
               );

always @* begin
  M_ctrX_d = M_ctrX_q;
  M_ctrY_d = M_ctrY_q;

  M_ctrX_d = (M_ctrX_q == 11'h31f) ? 1'h0 : M_ctrX_q + 1'h1;
  if (M_ctrX_q == 11'h31f) begin
    M_ctrY_d = (M_ctrY_q == 11'h208) ? 1'h0 : M_ctrY_q + 1'h1;
  end
  pclk = M_pclk_buf_O;
  hSync = (M_ctrX_q >= 11'h28a) && (M_ctrX_q < 11'h294);
  vSync = (M_ctrY_q >= 10'h1ea) && (M_ctrY_q < 10'h1ec);
  drawArea = (M_ctrX_q < 10'h280) && (M_ctrY_q < 9'h1e0);
  active = drawArea;
  x = M_ctrX_q;
  y = M_ctrY_q;
  tmds = M_dvi_tmds;
  tmdsb = M_dvi_tmdsb;
end

always @(posedge M_pclk_buf_O) begin
  if (rst == 1'b1) begin
    M_ctrX_q <= 1'h0;
    M_ctrY_q <= 1'h0;
  end else begin
    M_ctrX_q <= M_ctrX_d;
    M_ctrY_q <= M_ctrY_d;
  end
end

endmodule
