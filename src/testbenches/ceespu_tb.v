`timescale 1ns / 1ps

module ceespu_tb();

wire [3:0]O_dmemWe;
wire O_int_ack;
wire [15:0]O_dmemAddress;
wire [31:0]I_dmemData;
wire [15:0]O_imemAddress;
wire I_int;
wire [2:0]I_int_vector;
reg clk, rst;
wire O_dmemE;
wire [31:0]O_dmemWData;
wire I_dmemBusy;
reg [31:0]I_imemData;

reg [31:0] Icache [0:16383];

//assign O_dmemWe=
//assign O_int_ack=
//assign O_dmemAddress=
assign I_dmemData = 2;
//assign O_imemAddress=
assign I_int = 0;
assign I_int_vector = 0;
//assign clk=
//assign I_rst=
//assign O_dmemE=
//assign O_dmemWData=
assign I_dmemBusy = 0;
//assign I_imemData=

ceespu ceespu_test(
			.O_imemAddress(O_imemAddress[15:0]),
			.O_int_ack(O_int_ack),
			.O_dmemAddress(O_dmemAddress[15:0]),
			.O_dmemWData(O_dmemWData[31:0]),
			.O_dmemE(O_dmemE),
			.O_dmemWe(O_dmemWe[3:0]),
			.I_clk(clk),
			.I_rst(rst),
			.I_int(I_int),
			.I_int_vector(I_int_vector[2:0]),
			.I_imemData(I_imemData[31:0]),
			.I_dmemData(I_dmemData[31:0]),
			.I_dmemBusy(I_dmemBusy));
initial begin
	clk = 0;
	rst = 1;
	#12 rst = 0;
end

always begin
	forever #5 clk = !clk;
end

// Do this in your test bench

initial
 begin
    $dumpfile("test.lxt");
    $dumpvars(0,ceespu_test);
 end

always @(negedge clk) begin
	if (rst) begin
	   I_imemData = 0;
	end
	else begin
	    // $display("---------------------------");
	    // $display("reading addr %h", O_imemAddress);
		I_imemData = Icache[O_imemAddress / 4];
		// $display("yielded %h", I_imemData);
		// $display("---------------------------");
	end
end

initial begin
	#150
    $finish;
end
integer file, r;
initial begin
    $display("Loading rom");
    file = $fopenr("output.bin");
    if (file == 0) begin
      $display("Can't open rom, quiting...");
      $finish;
    end
	r = $fread(Icache, file);
	$display("Loaded %0d entries \n", r); 
end
endmodule