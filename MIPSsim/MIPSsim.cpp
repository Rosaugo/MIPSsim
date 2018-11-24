// MIPSsim.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <sstream>
#include <math.h>

using namespace std;

const int kOriginalInstructionAddress = 64; //初始的指令地址为64

//记录内存的起始地址
int original_address;

//定义Instruction结构体，用于存储解码后的指令中的参数
struct Instruction
{
	int address; //指令的地址
	string operation; //指令名
	int rs; //rs
	int rt; //rt
	int rd; //rd
	int sa; //sa
	int immediate; //立即数
	int offset; //偏移量
	int base; //基址
	string type; //指令类型，第一类指令（寄存器），第二类指令（立即数）

	//构造函数
	Instruction()
	{}

	//析构函数
	~Instruction()
	{}
};

//31位二进制数转十进制数
int ConvertToDecimal(string str)
{
	int decimal_value; //转换后的十进制数
	string str_to_be_convert = str; //二进制串
	string sign = str_to_be_convert.substr(0, 1); //二进制数的符号

	if (sign == "0") //正数情况
	{
		string unsigned_str_to_be_convert = str_to_be_convert.substr(1, 31); //去掉一位符号位后的二进制串

		bitset<32> bit_type_value = bitset<32>(string(unsigned_str_to_be_convert)); //string类型转换位bitset32类型
		decimal_value = bit_type_value.to_ulong(); //用二进制数据类to_ulong()方法转换成十进制

	}
	else if (sign == "1") //负数情况，补码取反加一
	{
		string unsigned_str_to_be_convert = str_to_be_convert.substr(1, 31); //去掉一位符号位后的二进制串

		bitset<32> bit_type_value = bitset<32>(string(unsigned_str_to_be_convert)); //string类型转换为bitset32类型

		//二进制取反
		for (int i = 0; i < 31; i++)
		{
			bit_type_value[i] = 1 - bit_type_value[i];
		}

		decimal_value = 0 - (bit_type_value.to_ulong() + 1); //加一后取负，表示一个负数
	}

	return decimal_value;
}

//将16位偏移量左移两位
int LeftShiftTwoBits(string str)
{
	int offset_value; //转换后的偏移量
	string str_to_be_left_shift = str.substr(0, 16); //二进制串
	bitset<16> bit_type_offset = bitset<16>(string(str_to_be_left_shift)); //string类型转换为bitset16类型
	offset_value = bit_type_offset.to_ulong() * 4; //转换成十进制，并左移两位(*4)

	return offset_value;
}

//16位偏移量不左移
int TwoBitsNoShift(string str)
{
	int offset_value; //转换后的偏移量
	string str_to_be_left_shift = str.substr(0, 16); //二进制串
	bitset<16> bit_type_offset = bitset<16>(string(str_to_be_left_shift)); //string类型转换为bitset16类型
	offset_value = bit_type_offset.to_ulong(); //转换成十进制

	return offset_value;
}

//将26位偏移量左移两位
int LeftShiftTwoBitsFullVersion(string str)
{
	int offset_value; //转换后的偏移量
	string str_to_be_left_shift = str.substr(0, 26);
	bitset<26> bit_type_offset = bitset<26>(string(str_to_be_left_shift));
	offset_value = bit_type_offset.to_ulong() * 4;

	return offset_value;
}

//将5位二进制数转换为寄存器的号码
int BinaryToRegister(string str)
{
	//TBD
	int register_number;

	bitset<5> bit_type_register_number = bitset<5>(string(str)); //string类型转换为bitset5类型
	register_number = bit_type_register_number.to_ulong();

	return register_number;
}

//读取二进制文件并解码
void ReadAndDecode(vector<string> input_instructions, vector<string> &output_instructions, vector<int> &data, vector<Instruction> &disassembled_instructions)
{	
	//定义字符串：解码后的输出、前1-5位操作码、操作指令、最后6位操作码
	string disassembly_output, opcode, operation, function_code; 

	int instruction_address; //定义指令地址

	bool break_flag = false; //判断二进制串表示的是指令还是寄存器中的值

	for (int i = 0; i < input_instructions.size(); i++)
	{
		instruction_address = kOriginalInstructionAddress + i * 4;

		string instruction_type = input_instructions.at(i).substr(0, 1);

		opcode = input_instructions.at(i).substr(1, 5);
		function_code = input_instructions.at(i).substr(26, 6);

		if (instruction_type == "0" && break_flag == false)
		{
			//实例化一个指令结构
			Instruction current_instruction;

			if (opcode == "00000")
			{
				if (function_code == "100000")
				{
					//ADD寄存器操作指令
					operation = "ADD";

					int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
					int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
					int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));

					stringstream str_rs, str_rt, str_rd, str_instruction_address;
					str_rs << rs;
					str_rt << rt;
					str_rd << rd;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.rs = rs;
					current_instruction.rt = rt;
					current_instruction.rd = rd;
					current_instruction.type = "register";

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "001101")
				{
					//BREAK指令
					operation = "BREAK";

					original_address = instruction_address + 4;

					stringstream str_instruction_address;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + "\n";

					output_instructions.push_back(output);

					break_flag = true;

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "001000")
				{
					//JR寄存器指令
					operation = "JR";

					int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));

					stringstream str_rs, str_instruction_address;
					str_rs << rs;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rs.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.rs = rs;

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "000000")
				{
					//用二进制串的的11到25位判断是指令为NOP还是SLL
					string nop_sll_code = input_instructions.at(i).substr(11, 15);

					if (nop_sll_code == "000000000000000")
					{
						//NOP指令
						operation = "NOP";

						stringstream str_instruction_address;
						str_instruction_address << instruction_address;

						string code_zero_to_five = input_instructions.at(i).substr(0, 6);
						string code_six_to_ten = input_instructions.at(i).substr(6, 5);
						string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
						string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
						string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
						string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

						string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

						//output为一行的实际输出
						string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + "\n";

						output_instructions.push_back(output);

						//向实例化的指令结构中注入灵魂
						current_instruction.address = instruction_address;
						current_instruction.operation = operation;

						//将获得灵魂的指令结构放入动态数组中
						disassembled_instructions.push_back(current_instruction);
					}
					else
					{
						//SLL指令
						operation = "SLL";

						int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
						int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));
						int sa = BinaryToRegister(input_instructions.at(i).substr(21, 5));

						stringstream str_rt, str_rd, str_sa, str_instruction_address;
						str_rt << rt;
						str_rd << rd;
						str_sa << sa;
						str_instruction_address << instruction_address;

						string code_zero_to_five = input_instructions.at(i).substr(0, 6);
						string code_six_to_ten = input_instructions.at(i).substr(6, 5);
						string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
						string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
						string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
						string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

						string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

						//output为一行的实际输出
						string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_sa.str() + "\n";

						output_instructions.push_back(output);

						//向实例化的指令结构中注入灵魂
						current_instruction.address = instruction_address;
						current_instruction.operation = operation;
						current_instruction.sa = sa;
						current_instruction.rt = rt;
						current_instruction.rd = rd;

						//将获得灵魂的指令结构放入动态数组中
						disassembled_instructions.push_back(current_instruction);
					}
				}
				else if (function_code == "100010")
				{
					//SUB寄存器操作指令
					operation = "SUB";

					int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
					int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
					int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));

					stringstream str_rs, str_rt, str_rd, str_instruction_address;
					str_rs << rs;
					str_rt << rt;
					str_rd << rd;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.rs = rs;
					current_instruction.rt = rt;
					current_instruction.rd = rd;
					current_instruction.type = "register";

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "000011")
				{
					//SRA指令，算术右移
					operation = "SRA";

					int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
					int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));
					int sa = BinaryToRegister(input_instructions.at(i).substr(21, 5));

					stringstream str_rt, str_rd, str_sa, str_instruction_address;
					str_rt << rt;
					str_rd << rd;
					str_sa << sa;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_sa.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.sa = sa;
					current_instruction.rt = rt;
					current_instruction.rd = rd;

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "000010")
				{
					//SRL指令，逻辑右移
					operation = "SRL";

					int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
					int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));
					int sa = BinaryToRegister(input_instructions.at(i).substr(21, 5));

					stringstream str_rt, str_rd, str_sa, str_instruction_address;
					str_rt << rt;
					str_rd << rd;
					str_sa << sa;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_sa.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.sa = sa;
					current_instruction.rt = rt;
					current_instruction.rd = rd;

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "101010")
				{
					//SLT指令
					operation = "SLT";

					int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
					int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
					int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));

					stringstream str_rs, str_rt, str_rd, str_instruction_address;
					str_rs << rs;
					str_rt << rt;
					str_rd << rd;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.rs = rs;
					current_instruction.rt = rt;
					current_instruction.rd = rd;
					current_instruction.type = "register";

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "100100")
				{
					//AND指令
					operation = "AND";

					int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
					int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
					int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));

					stringstream str_rs, str_rt, str_rd, str_instruction_address;
					str_rs << rs;
					str_rt << rt;
					str_rd << rd;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.rs = rs;
					current_instruction.rt = rt;
					current_instruction.rd = rd;
					current_instruction.type = "register";

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
				else if (function_code == "100111")
				{
					//NOR指令
					operation = "NOR";

					int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
					int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
					int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));

					stringstream str_rs, str_rt, str_rd, str_instruction_address;
					str_rs << rs;
					str_rt << rt;
					str_rd << rd;
					str_instruction_address << instruction_address;

					string code_zero_to_five = input_instructions.at(i).substr(0, 6);
					string code_six_to_ten = input_instructions.at(i).substr(6, 5);
					string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
					string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
					string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
					string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

					string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

					//output为一行的实际输出
					string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n";

					output_instructions.push_back(output);

					//向实例化的指令结构中注入灵魂
					current_instruction.address = instruction_address;
					current_instruction.operation = operation;
					current_instruction.rs = rs;
					current_instruction.rt = rt;
					current_instruction.rd = rd;
					current_instruction.type = "register";

					//将获得灵魂的指令结构放入动态数组中
					disassembled_instructions.push_back(current_instruction);
				}
			}
			else if (opcode == "00100")
			{
				//BEQ指令
				operation = "BEQ";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int offset = LeftShiftTwoBits(input_instructions.at(i).substr(16, 16)); //偏移量左移两位

				stringstream str_rs, str_rt, str_instruction_address, str_offset;
				str_rs << rs;
				str_rt << rt;
				str_instruction_address << instruction_address;
				str_offset << offset;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_offset.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.offset = offset;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "00111")
			{
				//BGTZ指令
				operation = "BGTZ";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int offset = LeftShiftTwoBits(input_instructions.at(i).substr(16, 16)); //偏移量左移两位

				stringstream str_rs, str_instruction_address, str_offset;
				str_rs << rs;
				str_instruction_address << instruction_address;
				str_offset << offset;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rs.str() + ", " + "#" + str_offset.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.offset = offset;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "00001")
			{
				//BLTZ指令
				operation = "BLTZ";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int offset = LeftShiftTwoBits(input_instructions.at(i).substr(16, 16)); //偏移量左移两位

				stringstream str_rs, str_instruction_address, str_offset;
				str_rs << rs;
				str_instruction_address << instruction_address;
				str_offset << offset;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rs.str() + ", " + "#" + str_offset.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.offset = offset;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "00010")
			{
				//J指令
				operation = "J";

				int offset = LeftShiftTwoBitsFullVersion(input_instructions.at(i).substr(6, 26)); //偏移量左移两位

				stringstream str_instruction_address, str_offset;
				str_instruction_address << instruction_address;
				str_offset << offset;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "#" + str_offset.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.offset = offset;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "11100")
			{
				//MUL指令
				operation = "MUL";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int rd = BinaryToRegister(input_instructions.at(i).substr(16, 5));

				stringstream str_rs, str_rt, str_rd, str_instruction_address;
				str_rs << rs;
				str_rt << rt;
				str_rd << rd;
				str_instruction_address << instruction_address;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.rd = rd;
				current_instruction.type = "register";

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
		}
		else if (instruction_type == "1" && break_flag == false)
		{
			//实例化一个指令结构
			Instruction current_instruction;

			if (opcode == "10000")
			{
				//ADD立即数指令
				operation = "ADD";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int immediate = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //ADD立即数不左移

				stringstream str_rs, str_rt, str_instruction_address, str_immediate;
				str_rs << rs;
				str_rt << rt;
				str_instruction_address << instruction_address;
				str_immediate << immediate;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.type = "immediate";
				current_instruction.immediate = immediate;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "10001")
			{
				//SUB立即数指令
				operation = "SUB";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int immediate = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //SUB立即数不左移

				stringstream str_rs, str_rt, str_instruction_address, str_immediate;
				str_rs << rs;
				str_rt << rt;
				str_instruction_address << instruction_address;
				str_immediate << immediate;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.type = "immediate";
				current_instruction.immediate = immediate;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "00001")
			{
				//MUL指令
				operation = "MUL";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int immediate = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //MUL立即数不左移

				stringstream str_rs, str_rt, str_instruction_address, str_immediate;
				str_rs << rs;
				str_rt << rt;
				str_instruction_address << instruction_address;
				str_immediate << immediate;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.type = "immediate";
				current_instruction.immediate = immediate;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "10010")
			{
				//AND指令
				operation = "AND";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int immediate = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //AND立即数不左移

				stringstream str_rs, str_rt, str_instruction_address, str_immediate;
				str_rs << rs;
				str_rt << rt;
				str_instruction_address << instruction_address;
				str_immediate << immediate;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.type = "immediate";
				current_instruction.immediate = immediate;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "10011")
			{
				//NOR指令
				operation = "NOR";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int immediate = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //NOR立即数不左移

				stringstream str_rs, str_rt, str_instruction_address, str_immediate;
				str_rs << rs;
				str_rt << rt;
				str_instruction_address << instruction_address;
				str_immediate << immediate;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.type = "immediate";
				current_instruction.immediate = immediate;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "10101")
			{
				//SLT指令
				operation = "SLT";

				int rs = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int immediate = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //SLT立即数不左移

				stringstream str_rs, str_rt, str_instruction_address, str_immediate;
				str_rs << rs;
				str_rt << rt;
				str_instruction_address << instruction_address;
				str_immediate << immediate;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rs = rs;
				current_instruction.rt = rt;
				current_instruction.type = "immediate";
				current_instruction.immediate = immediate;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "01011")
			{
				//SW指令
				operation = "SW";

				int base = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int offset = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //SW的offset不左移

				stringstream str_base, str_rt, str_instruction_address, str_offset;
				str_rt << rt;
				str_base << base;
				str_instruction_address << instruction_address;
				str_offset << offset;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + str_offset.str() + "(" + "R" + str_base.str() + ")" + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rt = rt;
				current_instruction.base = base;
				current_instruction.offset = offset;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
			else if (opcode == "00011")
			{
				//LW指令
				operation = "LW";

				int base = BinaryToRegister(input_instructions.at(i).substr(6, 5));
				int rt = BinaryToRegister(input_instructions.at(i).substr(11, 5));
				int offset = TwoBitsNoShift(input_instructions.at(i).substr(16, 16)); //LW的offset不左移

				stringstream str_base, str_rt, str_instruction_address, str_offset;
				str_rt << rt;
				str_base << base;
				str_instruction_address << instruction_address;
				str_offset << offset;

				string code_zero_to_five = input_instructions.at(i).substr(0, 6);
				string code_six_to_ten = input_instructions.at(i).substr(6, 5);
				string code_eleven_to_fifteen = input_instructions.at(i).substr(11, 5);
				string code_sixteen_to_twenty = input_instructions.at(i).substr(16, 5);
				string code_twentyone_to_twentyfive = input_instructions.at(i).substr(21, 5);
				string code_twentysix_to_thirtyone = input_instructions.at(i).substr(26, 6);

				string instructions_with_blank = code_zero_to_five + " " + code_six_to_ten + " " + code_eleven_to_fifteen + " " + code_sixteen_to_twenty + " " + code_twentyone_to_twentyfive + " " + code_twentysix_to_thirtyone;

				//output为一行的实际输出
				string output = instructions_with_blank + "\t" + str_instruction_address.str() + "\t" + operation + " " + "R" + str_rt.str() + ", " + str_offset.str() + "(" + "R" + str_base.str() + ")" + "\n";

				output_instructions.push_back(output);

				//向实例化的指令结构中注入灵魂
				current_instruction.address = instruction_address;
				current_instruction.operation = operation;
				current_instruction.rt = rt;
				current_instruction.base = base;
				current_instruction.offset = offset;

				//将获得灵魂的指令结构放入动态数组中
				disassembled_instructions.push_back(current_instruction);
			}
		}
		else if (break_flag == true)
		{
			int decimal_value;

			string str_to_be_push_back;

			//BREAK指令之后的二进制串形式表示寄存器中的值
			decimal_value = ConvertToDecimal(input_instructions.at(i));

			//整型转换为string类型，与其他参数一起输出
			stringstream str_instruction_address, str_decimal_value;
			str_instruction_address << instruction_address;
			str_decimal_value << decimal_value;
			str_to_be_push_back = input_instructions.at(i) + "\t" + str_instruction_address.str() + "\t" + str_decimal_value.str() + "\n";

			//存放入output_instructions中
			output_instructions.push_back(str_to_be_push_back);

			//解码出的十进制数值存入data动态数组中
			data.push_back(decimal_value);
		}
	}
}

//模拟指令执行
void Simulation(vector<Instruction> disassembled_instructions, vector<string> &output_disassembled_instructions, int(&registers)[32], vector<int> &data)
{
	bool end_flag = false;

	int cycle_number = 1;

	//用于输出的分割线
	const string cut_off = "--------------------";

	//从地址为64的指令开始执行
	int address = 64;

	//记录共有多少内存地址被使用
	int number_of_address_used = data.size();

	//函数内记录当前地址
	int current_original_address;

	//计算内存地址共需要显示几行
	int row_number = (number_of_address_used + 7) / 8;

	//计算最后一行有几个地址需要输出
	int residual = number_of_address_used - (8 * (row_number - 1));

	string operation;

	while (end_flag != true)
	{
		int i;

		for (i = 0; i < disassembled_instructions.size(); i++)
		{
			if (disassembled_instructions.at(i).address == address)
			{
				operation = disassembled_instructions.at(i).operation;
				break;
			}
		}

		Instruction current_instruction = disassembled_instructions.at(i);

		if (operation == "ADD")
		{
			//ADD指令
			if (current_instruction.type == "register")
			{
				//ADD寄存器指令

				//加法操作，rd = rs + rt
				registers[current_instruction.rd] = registers[current_instruction.rs] + registers[current_instruction.rt];

				stringstream str_cycle_number, str_address, str_rs, str_rd, str_rt;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rd << current_instruction.rd;
				str_rt << current_instruction.rt;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number -1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
			else if (current_instruction.type == "immediate")
			{
				//ADD立即数指令

				//加法操作，rt = rs + immediate
				registers[current_instruction.rt] = registers[current_instruction.rs] + current_instruction.immediate;

				stringstream str_cycle_number, str_address, str_rs, str_rt, str_immediate;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rt << current_instruction.rt;
				str_immediate << current_instruction.immediate;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
		}
		else if (operation == "BEQ")
		{
			//BEQ指令

			int next_address;

			//两数相等则跳转 rs = rt, then branch
			if(registers[current_instruction.rs] == registers[current_instruction.rt])
			{
				next_address = address + current_instruction.offset + 4;
			}
			else if (registers[current_instruction.rs] != registers[current_instruction.rt])
			{
				next_address = address + 4;
			}

			stringstream str_cycle_number, str_rs, str_rt, str_address, str_offset;
			str_rs << current_instruction.rs;
			str_rt << current_instruction.rt;
			str_address << address;
			str_offset << current_instruction.offset;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rs.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_offset.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address = next_address;
		}
		else if (operation == "BGTZ")
		{
			//BGTZ指令

			int next_address;

			//大于零则跳转 rs > 0, then branch
			if (registers[current_instruction.rs] > 0)
			{
				next_address = address + current_instruction.offset + 4;
			}
			else
			{
				next_address = address + 4;
			}

			stringstream str_cycle_number, str_rs, str_address, str_offset;
			str_rs << current_instruction.rs;
			str_address << address;
			str_offset << current_instruction.offset;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rs.str() + ", " + "#" + str_offset.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address = next_address;
		}
		else if (operation == "BLTZ")
		{
			//BLTZ指令

			int next_address;

			//小于零则跳转 rs < 0, then branch
			if (registers[current_instruction.rs] < 0)
			{
				next_address = address + current_instruction.offset + 4;
			}
			else
			{
				next_address = address + 4;
			}

			stringstream str_cycle_number, str_rs, str_address, str_offset;
			str_rs << current_instruction.rs;
			str_address << address;
			str_offset << current_instruction.offset;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rs.str() + ", " + "#" + str_offset.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address = next_address;
		}
		else if (operation == "BREAK")
		{
			//BREAK指令

			end_flag = true;

			stringstream str_cycle_number, str_address, str_offset;
			str_address << address;
			str_offset << current_instruction.offset;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation
				+ "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);
		}
		else if (operation == "J")
		{
			//J指令

			int next_address;

			//无条件跳转 branch
			next_address = current_instruction.offset;

			stringstream str_cycle_number, str_address, str_offset;
			str_address << address;
			str_offset << current_instruction.offset;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "#" + str_offset.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address = next_address;
		}
		else if (operation == "JR")
		{
			//JR指令

			int next_address;

			//无条件跳转到寄存器中的地址

			next_address = current_instruction.rs;

			stringstream str_cycle_number, str_rs, str_address;
			str_address << address;
			str_rs << current_instruction.rs;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rs.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address = next_address;
		}
		else if (operation == "NOP")
		{
			//NOP指令

			//无操作

			stringstream str_cycle_number, str_address;
			str_address << address;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation
				+ "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address += 4;
		}
		else if (operation == "SLL")
		{
			//SLL指令

			//位数左移操作 rd = rt << sa
			registers[current_instruction.rd] = registers[current_instruction.rt] * (pow(2, current_instruction.sa));

			stringstream str_cycle_number, str_sa, str_rd, str_rt, str_address;
			str_address << address;
			str_sa << current_instruction.sa;
			str_rd << current_instruction.rd;
			str_rt << current_instruction.rt;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rd.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_sa.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address += 4;
		}
		else if (operation == "SUB")
		{
			//SUB指令
			if (current_instruction.type == "register")
			{
				//SUB寄存器指令

				//减法操作，rd = rs - rt
				registers[current_instruction.rd] = registers[current_instruction.rs] - registers[current_instruction.rt];

				stringstream str_cycle_number, str_address, str_rs, str_rd, str_rt;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rd << current_instruction.rd;
				str_rt << current_instruction.rt;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
			else if (current_instruction.type == "immediate")
			{
				//SUB立即数指令

				//减法操作，rt = rs - immediate
				registers[current_instruction.rt] = registers[current_instruction.rs] - current_instruction.immediate;

				stringstream str_cycle_number, str_address, str_rs, str_rt, str_immediate;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rt << current_instruction.rt;
				str_immediate << current_instruction.immediate;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
		}
		else if (operation == "SRA")
		{
			//SRA指令

			//rd = rt算术右移sa位的结果
			registers[current_instruction.rd] = registers[current_instruction.rt] >> registers[current_instruction.sa];

			stringstream str_cycle_number, str_address, str_rd, str_rt, str_sa;
			str_cycle_number << cycle_number;
			str_address << address;
			str_rd << current_instruction.rd;
			str_rt << current_instruction.rt;
			str_sa << current_instruction.sa;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rd.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_sa.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address += 4;
		}
		else if (operation == "SRL")
		{
			//SRL指令

			//rd = rt逻辑右移sa位的结果
			unsigned int current_registers_value;
			current_registers_value = (unsigned)registers[current_instruction.rt] >> registers[current_instruction.sa];
			registers[current_instruction.rd] = (int)current_registers_value;

			stringstream str_cycle_number, str_address, str_rd, str_rt, str_sa;
			str_cycle_number << cycle_number;
			str_address << address;
			str_rd << current_instruction.rd;
			str_rt << current_instruction.rt;
			str_sa << current_instruction.sa;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rd.str() + ", " + "R" + str_rt.str() + ", " + "#" + str_sa.str() + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address += 4;
		}
		else if (operation == "SW")
		{
			//SW指令

			//存储指令 memory[base + offset] = rt;
			data.at(((current_instruction.offset + registers[current_instruction.base]) - original_address) / 4) = registers[current_instruction.rt];

			stringstream str_cycle_number, str_offset, str_base, str_rt, str_address;
			str_address << address;
			str_base << current_instruction.base;
			str_rt << current_instruction.rt;
			str_offset << current_instruction.offset;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rt.str() + ", " + str_offset.str() + "(" + "R" + str_base.str() + ")" + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address += 4;
		}
		else if (operation == "LW")
		{
			//LW指令

			//读取指令 rt = memory[base + offset]
			registers[current_instruction.rt] = data.at(((current_instruction.offset + registers[current_instruction.base]) - original_address) / 4);

			stringstream str_cycle_number, str_offset, str_base, str_rt, str_address;
			str_address << address;
			str_base << current_instruction.base;
			str_rt << current_instruction.rt;
			str_offset << current_instruction.offset;
			str_cycle_number << cycle_number;

			//output为实际输出
			string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
				+ "R" + str_rt.str() + ", " + str_offset.str() + "(" + "R" + str_base.str() + ")" + "\n" + "\n" + "Registers" + "\n"
				+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
				+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
				+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
				+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
				+ to_string(registers[15]) + "\n"
				+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
				+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
				+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
				+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
				+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

			for (int i = 0; i < row_number; i++)
			{
				current_original_address = original_address + i * 32;

				if (i != row_number - 1)
				{
					output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
						+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
						+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
						+ "\n";
				}
				else if (i == row_number - 1)
				{
					output = output + to_string(current_original_address) + ":";
					for (int j = 0; j < residual; j++)
					{
						output = output + "\t" + to_string(data.at(i * 8 + j));
					}
					output = output + "\n";
				}
			}

			output = output + "\n";

			output_disassembled_instructions.push_back(output);

			//下一个地址
			address += 4;
		}
		else if (operation == "MUL")
		{
			//MUL指令
			if (current_instruction.type == "register")
			{
				//MUL寄存器指令

				//乘法操作，rd = rs * rt
				registers[current_instruction.rd] = registers[current_instruction.rs] * registers[current_instruction.rt];

				stringstream str_cycle_number, str_address, str_rs, str_rd, str_rt;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rd << current_instruction.rd;
				str_rt << current_instruction.rt;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
			else if (current_instruction.type == "immediate")
			{
				//MUL立即数指令

				//乘法操作，rt = rs * immediate
				registers[current_instruction.rt] = registers[current_instruction.rs] * current_instruction.immediate;

				stringstream str_cycle_number, str_address, str_rs, str_rt, str_immediate;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rt << current_instruction.rt;
				str_immediate << current_instruction.immediate;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;

			}
		}
		else if (operation == "AND")
		{
			//AND指令
			if (current_instruction.type == "register")
			{
				//AND寄存器指令

				//与操作，rd = rs && rt
				registers[current_instruction.rd] = (registers[current_instruction.rs] && registers[current_instruction.rt]);

				stringstream str_cycle_number, str_address, str_rs, str_rd, str_rt;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rd << current_instruction.rd;
				str_rt << current_instruction.rt;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
			else if (current_instruction.type == "immediate")
			{
				//AND立即数指令

				//与操作，rt = rs && immediate
				registers[current_instruction.rt] = (registers[current_instruction.rs] && current_instruction.immediate);

				stringstream str_cycle_number, str_address, str_rs, str_rt, str_immediate;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rt << current_instruction.rt;
				str_immediate << current_instruction.immediate;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
		}
		else if (operation == "NOR")
		{
			//NOR指令
			if (current_instruction.type == "register")
			{
				//NOR寄存器指令

				//或非操作，rd = ~(rs || rt)
				registers[current_instruction.rd] = ~(registers[current_instruction.rs] || registers[current_instruction.rt]);

				stringstream str_cycle_number, str_address, str_rs, str_rd, str_rt;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rd << current_instruction.rd;
				str_rt << current_instruction.rt;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
			else if (current_instruction.type == "immediate")
			{
				//NOR立即数指令

				//或非操作，rt = ~(rs || immediate)
				registers[current_instruction.rt] = ~(registers[current_instruction.rs] || current_instruction.immediate);

				stringstream str_cycle_number, str_address, str_rs, str_rt, str_immediate;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rt << current_instruction.rt;
				str_immediate << current_instruction.immediate;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
		}
		else if (operation == "SLT")
		{
			//SLT指令
			if (current_instruction.type == "register")
			{
				//SLT寄存器指令

				//逻辑小于操作，rd = (rs < rt)
				registers[current_instruction.rd] = (registers[current_instruction.rs] < registers[current_instruction.rt]);

				stringstream str_cycle_number, str_address, str_rs, str_rd, str_rt;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rd << current_instruction.rd;
				str_rt << current_instruction.rt;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rd.str() + ", " + "R" + str_rs.str() + ", " + "R" + str_rt.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
			else if (current_instruction.type == "immediate")
			{
				//SLT立即数指令

				//逻辑小于操作，rt = (rs < immediate)
				registers[current_instruction.rt] = (registers[current_instruction.rs] < current_instruction.immediate);

				stringstream str_cycle_number, str_address, str_rs, str_rt, str_immediate;
				str_cycle_number << cycle_number;
				str_address << address;
				str_rs << current_instruction.rs;
				str_rt << current_instruction.rt;
				str_immediate << current_instruction.immediate;

				//output为实际输出
				string output = cut_off + "\n" + "Cycle:" + str_cycle_number.str() + "\t" + str_address.str() + "\t" + operation + "\t"
					+ "R" + str_rt.str() + ", " + "R" + str_rs.str() + ", " + "#" + str_immediate.str() + "\n" + "\n" + "Registers" + "\n"
					+ "R00:" + "\t" + to_string(registers[0]) + "\t" + to_string(registers[1]) + "\t" + to_string(registers[2]) + "\t"
					+ to_string(registers[3]) + "\t" + to_string(registers[4]) + "\t" + to_string(registers[5]) + "\t" + to_string(registers[6]) + "\t"
					+ to_string(registers[7]) + "\t" + to_string(registers[8]) + "\t" + to_string(registers[9]) + "\t" + to_string(registers[10]) + "\t"
					+ to_string(registers[11]) + "\t" + to_string(registers[12]) + "\t" + to_string(registers[13]) + "\t" + to_string(registers[14]) + "\t"
					+ to_string(registers[15]) + "\n"
					+ "R16:" + "\t" + to_string(registers[16]) + "\t" + to_string(registers[17]) + "\t" + to_string(registers[18]) + "\t"
					+ to_string(registers[19]) + "\t" + to_string(registers[20]) + "\t" + to_string(registers[21]) + "\t"
					+ to_string(registers[22]) + "\t" + to_string(registers[23]) + "\t" + to_string(registers[24]) + "\t" + to_string(registers[25]) + "\t"
					+ to_string(registers[26]) + "\t" + to_string(registers[27]) + "\t" + to_string(registers[28]) + "\t" + to_string(registers[29]) + "\t"
					+ to_string(registers[30]) + "\t" + to_string(registers[31]) + "\n" + "\n" + "Data" + "\n";

				for (int i = 0; i < row_number; i++)
				{
					current_original_address = original_address + i * 32;

					if (i != row_number - 1)
					{
						output = output + to_string(current_original_address) + ":" + "\t" + to_string(data.at(i * 8)) + "\t" + to_string(data.at(i * 8 + 1))
							+ "\t" + to_string(data.at(i * 8 + 2)) + "\t" + to_string(data.at(i * 8 + 3)) + "\t" + to_string(data.at(i * 8 + 4))
							+ "\t" + to_string(data.at(i * 8 + 5)) + "\t" + to_string(data.at(i * 8 + 6)) + "\t" + to_string(data.at(i * 8 + 7))
							+ "\n";
					}
					else if (i == row_number - 1)
					{
						output = output + to_string(current_original_address) + ":";
						for (int j = 0; j < residual; j++)
						{
							output = output + "\t" + to_string(data.at(i * 8 + j));
						}
						output = output + "\n";
					}
				}

				output = output + "\n";

				output_disassembled_instructions.push_back(output);

				//下一个地址
				address += 4;
			}
		}
		cycle_number++;
	}
}

int main()
{
	//定义字符串：输入二进制文件名、模拟运行后的输出
	string filename, simulation_output;

	vector<string> input_instructions; //创建一个string动态数组，用于存放从文件中读取的二进制字符串
	vector<string> output_instrucitons;

	vector<Instruction> disassembled_instructions; //创建一个Instruction动态数组，用于存放反汇编得到的汇编指令

	vector<string> output_disassembled_instructions;

	int registers[32]; //创建一个大小为32的数组，用于存放MIPS32个寄存器中的值
	
	//registers数组初始化
	for (int i = 0; i < 32; i++)
	{
		registers[i] = 0;
	}

	vector<int> data; //创建一个data动态数组，用于存放内存中的值

	cin >> filename;
	ifstream inputfile(filename);

	//从文件中读取二进制指令，并存放到vector容器中
	string current_input_line;
	while (getline(inputfile, current_input_line))
	{
		input_instructions.push_back(current_input_line);
	}

	//测试从文件中读取的二进制指令是否被存放到了vector容器中
	for (int i = 0; i < input_instructions.size(); i++)
	{
		cout << input_instructions.at(i) << endl;
	}

	ofstream disassembly_file;
	disassembly_file.open("disassembly_int.txt");

	ReadAndDecode(input_instructions, output_instrucitons, data, disassembled_instructions); //将二进制指令解码输出

	for (int i = 0; i < output_instrucitons.size(); i++)
	{
		cout << output_instrucitons.at(i) << endl;
		disassembly_file << output_instrucitons.at(i);
	}

	disassembly_file.close();

	ofstream simulation_file;
	simulation_file.open("simulation_int.txt");

	Simulation(disassembled_instructions, output_disassembled_instructions, registers, data); //模拟指令执行

	for (int i = 0; i < output_disassembled_instructions.size(); i++)
	{
		cout << output_disassembled_instructions.at(i) << endl;
		simulation_file << output_disassembled_instructions.at(i);
	}

	simulation_file.close();

	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
