`define CLOCK_SPEED = 104999;
`define opcode	   I_instruction[31:26]
`define regd 	   I_instruction[25:21]
`define rega_sel   I_instruction[20:16]
`define regb_sel   I_instruction[15:11]
`define imm_value  I_instruction[15:0]
`define C_bit	   I_instruction[26]
`define IMM_bit    I_instruction[30] 
`define SE_bit     I_instruction[0]
`define SHF_bits   I_instruction[7:6]
`define LINK_bit   I_instruction[0]
`define branch_condition I_instruction[28:26]

`define ADD       6'b0?000?
`define SUB       6'b0?001?
`define LOGIC_OR  6'b0?0100
`define LOGIC_AND 6'b0?0101
`define LOGIC_XOR 6'b0?0110
`define SEXT      6'b000111
`define SHF       6'b0?1000
`define MUL       6'b0?1001
// `define DIV       6'b0?1010
`define LOAD      6'b100???
`define IMM       6'b101010
`define EINT      6'b101011
`define STORE     6'b1101??
`define C_BRANCH  6'b111??0
`define BRANCH    6'b111111

`define ALU_ADD    4'd0
`define ALU_OR     4'd1
`define ALU_AND    4'd2
`define ALU_XOR    4'd3
`define ALU_SEXT8  4'd4
`define ALU_SEXT16 4'd5
`define ALU_SHL    4'd6
`define ALU_SHR    4'd7
`define ALU_SAR    4'd8
`define ALU_MUL    4'd9
// `define ALU_DIV    4'd10