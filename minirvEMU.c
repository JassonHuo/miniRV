#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define max_size 40000000
uint32_t PC = 0;
uint32_t R[16] = {0};
uint32_t M[max_size] = {0};
#define ADDI 0
#define JALR 1
#define ADD 2
#define LW 3
#define LBU 4
#define SB 5
#define SW 6
#define LUI 7
#define EBREAK 8

int halt = 1;

int type(uint32_t code)
{
	uint32_t opcode, funct3;
	opcode = code & 0x7F;
	funct3 = (code >> 12) & 0x7;
	if(opcode == 0b0110111) return LUI;
	else if(opcode == 0b1100111 && funct3 == 0) return JALR;
	else if(opcode == 0b0000011 && funct3 == 0b10) return LW;
	else if(opcode == 0b0000011 && funct3 == 0b100) return LBU;
	else if(opcode == 0b0100011 && funct3 == 0) return SB;
	else if(opcode == 0b0100011 && funct3 == 0b10) return SW;
	else if(opcode == 0b0010011 && funct3 == 0) return ADDI;
	else if(opcode == 0b0110011 && funct3 == 0) return ADD;
	else if(opcode == 0b1110011 && funct3 == 0 && (code >> 20)) return EBREAK;
	else{
		printf("opcode or funct3 is wrong, try again\n");
		printf("opcode: %7x\n, funct3: %3x\n",opcode, funct3);
		exit(1);
	}
}

void chan_reg(uint32_t idx, uint32_t elem)
{
	if(idx < 0 || idx > 15){
		printf("PC: %d\n", PC);
		printf("code: %x\n", M[PC >> 2]);
		printf("index out of range[register write]\n");
		exit(1);
	}
	if(idx == 0) return;
	R[idx] = elem;
}

uint32_t get_reg(uint32_t idx)
{
	if(idx < 0 || idx > 15){
		printf("PC: %d\n", PC);
		printf("code: %x\n", M[PC >> 2]);
		printf("index out of range[register read]\n");
		exit(1);
	}
	if(idx == 0) return 0;
	return R[idx];
}

void chan_M(uint32_t idx, uint32_t elem)
{
	if(idx < 0 || idx >= max_size){
		printf("PC: %d\n", PC);
		printf("code: %x\n", M[PC >> 2]);
		printf("index out of range[Memory write]\n");
		exit(1);
	}
	M[idx] = elem;
}

uint32_t get_M(uint32_t idx)
{
	if(idx < 0 || idx >= max_size){
		printf("PC: %d\n", PC);
		printf("code: %x\n", M[PC >> 2]);
		printf("index out of range[Memory read]\n");
		exit(1);
	}
	return M[idx];
}

void addi(uint32_t code)
{
	uint32_t rs1, rd;
	int32_t imm;
	imm = (int32_t)code >> 20;
	rs1 = (code >> 15) & 0x1F;
	rd = (code >> 7) & 0x1F;
	uint32_t reg_num = get_reg(rs1);
	uint32_t tar_num = (uint32_t)imm + reg_num;
	chan_reg(rd, tar_num);
	PC += 4;
}

void jalr(uint32_t code)
{
	int32_t imm;
	uint32_t rs1, rd;
	imm = (int32_t)code >> 20;
	rs1 = (code >> 15) & 0x1F;
	rd = (code >> 7) & 0x1F;
	uint32_t reg_num = get_reg(rs1);
	chan_reg(rd, PC + 4);
	PC = ((uint32_t)imm + reg_num) & ~0x1;	
}

void add(uint32_t code)
{
	uint32_t rs1, rs2, rd;
	uint32_t num1, num2, tar_num;
	rd = (code >> 7) & 0x1F;
	rs1 = (code >> 15) & 0x1F;
	rs2 = (code >> 20) & 0x1F;
	num1 = get_reg(rs1);
	num2 = get_reg(rs2);
	tar_num = num1 + num2;
	chan_reg(rd, tar_num);
	PC += 4;
}

void lui(uint32_t code)
{
	uint32_t imm, rd;
	rd = (code >> 7) & 0x1F;
	imm = code & 0xFFFFF000;
	chan_reg(rd, imm);
	PC += 4;
}

void lw(uint32_t code)
{
	int32_t rs1, rd;
	int32_t imm;
	imm = (int32_t)code >> 20;
	rs1 = (code >> 15) & 0x1F;
	rd = (code >> 7) & 0x1F;
	uint32_t num = get_reg(rs1);
	uint32_t addr = (uint32_t)imm + num;
	uint32_t M_num = get_M(addr >> 2);
	chan_reg(rd, M_num);
	PC += 4;
}

void lbu(uint32_t code)
{
	uint32_t imm, rs1, rd;
	imm = (uint32_t)((int32_t)code >> 20);
	rs1 = (code >> 15) & 0x1F;
	rd = (code >> 7) & 0x1F;
	uint32_t addr = get_reg(rs1) + imm;
	uint32_t w_addr = addr >> 2;
	uint32_t byte_addr = addr & 0x3;
	uint32_t M_num = get_M(w_addr);
	uint32_t tar_num = (M_num >> (8 * byte_addr)) & 0xFF;
	chan_reg(rd, tar_num);
	PC += 4;
}

void sb(uint32_t code)
{
	uint32_t imm_high, imm_low, rs1, rs2, imm;
	imm_low = (code >> 7) & 0x1F;
	imm_high = (code >> 20) & 0x00000FE0;
	imm = imm_low | imm_high;
	rs1 = (code >> 15) & 0x1F;
	rs2 = (code >> 20) & 0x1F;
	int32_t sign_imm = (int32_t)(imm << 20) >> 20;
	uint32_t addr = (uint32_t)sign_imm + get_reg(rs1);
	uint32_t reg_num = get_reg(rs2);
	uint32_t insert_num = reg_num & 0xFF;
	uint32_t M_num = get_M(addr >> 2);
	uint32_t byte_addr = addr & 0x3;
	uint32_t tar_num = (M_num & (~(0xFFU << (byte_addr * 8)))) | insert_num << (byte_addr * 8);
	chan_M(addr >> 2, tar_num);
	PC += 4;
}	

void sw(uint32_t code)
{
	uint32_t imm_high, imm_low, rs1, rs2, imm;
	imm_low = (code >> 7) & 0x1F;
	imm_high = (code >> 20) & 0x00000FE0;
	imm = imm_low | imm_high;
	rs1 = (code >> 15) & 0x1F;
	rs2 = (code >> 20) & 0x1F;
	int32_t sign_imm = (int32_t)(imm << 20) >> 20;
	uint32_t addr = (uint32_t)sign_imm + get_reg(rs1);
	uint32_t reg_num = get_reg(rs2);
	chan_M(addr >> 2, reg_num);
	PC += 4;	
}

void ebreak(uint32_t code)
{
	halt = 0;
	if(get_reg(10))
	{ 
		printf("value of ra is wrong\n\n");
	}
	else
	{
		printf("operating normally\n\n");
	}
}	

void inst_cycle()
{
//	printf("PC: %d(%x)\n", PC, PC);
	if(PC & 0x3){
		printf("unaligned accesses[PC]");
		exit(1);
	}
//	printf("PC: %d", PC);
	uint32_t code = get_M(PC >> 2);
	switch(type(code))
	{
		case ADDI:
			addi(code);
			break;
		case JALR:
			jalr(code);
			break;
		case ADD:
			add(code);
			break;
		case LUI:
			lui(code);
			break;
		case LW:
			lw(code);
			break;
		case LBU:
			lbu(code);
			break;
		case SB:
			sb(code);
			break;
		case SW:
			sw(code);
			break;
		case EBREAK:
			ebreak(code);
			break;
		default:
			printf("do not have the opcode or func, try again\n");
			printf("PC: %d(%x)\n", PC, PC);
			printf("code: %d(%x)\n", code, code);
			exit(1);
			break;
	}
//	PC += 4;
}
int main(int argc, char *argv[])
{
	int i = 0;
	if(argc == 1){
		printf("You must input a file name\n");
		exit(1);
	}
	FILE *fp = fopen(argv[1], "rb");
	while(fread(M + i, sizeof(uint32_t), 1, fp)) i++; 
	M[548 / 4] = 0b100000000000001110011;
	M[549 / 4] = 0b100000000000001110011;
	PC = 0;
	while(halt) inst_cycle();
	for(i = 0; i < 16; i ++) printf("%2d: %-8x(%d)\n", i, R[i], R[i]);
	printf("PC: %d(%x)\n", PC, PC);
	return 0;	
}
