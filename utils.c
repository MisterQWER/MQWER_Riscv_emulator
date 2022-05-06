#include "utils.h"
#include <stdio.h>
#include <stdlib.h>




/* Sign extends the given field to a 32-bit integer where field is
 * interpreted an n-bit integer. */
int sign_extend_number(unsigned int field, unsigned int n) {
  /* YOUR CODE HERE */
  int result = (int) field;
  int offset = 32 - n;
  result <<= offset;
  result >>= offset;
  return result;
}




/* Unpacks the 32-bit machine code instruction given into the correct
 * type within the instruction struct */
Instruction parse_instruction(uint32_t instruction_bits) {
  /* YOUR CODE HERE */
  Instruction instruction;
  // add x8, x0, x0     hex : 00000433  binary = 0000 0000 0000 0000 0000 01000
  // Opcode: 0110011 (0x33) Get the Opcode by &ing 0x1111111, bottom 7 bits
  instruction.opcode = instruction_bits & ((1U << 7) - 1);

  // Shift right to move to pointer to interpret next fields in instruction.
  instruction_bits >>= 7;

  switch (instruction.opcode) {
  // R-Type
  case 0x0b:
  case 0x33:
    // instruction: 0000 0000 0000 0000 0000 destination : 01000
    instruction.rtype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 0000 0000 0000 0 func3 : 000
    instruction.rtype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // instruction: 0000 0000 0000  src1: 00000
    instruction.rtype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 000        src2: 00000
    instruction.rtype.rs2 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // funct7: 0000 000
    instruction.rtype.funct7 = instruction_bits & ((1U << 7) - 1);
    break;
  // case for I-type 
  case 0x13:
  case 0x3:
  case 0x73:
  case 0x67:
    instruction.itype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    instruction.itype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    instruction.itype.rs1 = instruction_bits & ((1U << 5) -1);
    instruction_bits >>= 5;

    instruction.itype.imm = instruction_bits & ((1U << 12) - 1);
    instruction_bits >>= 12;
    break;

  // case S type
  case 0x23:
    instruction.stype.imm5 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    instruction.stype.funct3 = instruction_bits & ((1U << 3) -1);
    instruction_bits >>= 3;

    instruction.stype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    instruction.stype.rs2 = instruction_bits & ((1U << 5) - 1);    
    instruction_bits >>= 5;

    instruction.stype.imm7 = instruction_bits & ((1U << 7) - 1);
    instruction_bits >>= 7;
    break;

  //case B type
  case 0x63:
    instruction.sbtype.imm5 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    instruction.sbtype.funct3 = instruction_bits & ((1U << 3) -1);
    instruction_bits >>= 3;

    instruction.sbtype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    instruction.sbtype.rs2 = instruction_bits & ((1U << 5) - 1);    
    instruction_bits >>= 5;

    instruction.sbtype.imm7 = instruction_bits & ((1U << 7) - 1);
    instruction_bits >>= 7;
    break;

  // case U type
  case 0x37:
  case 0x17:
    instruction.utype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    instruction.utype.imm = instruction_bits & ((1U << 20) - 1);
    instruction_bits >>= 20;
    break;

  //case J type
  case 0x6f:
    instruction.ujtype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    instruction.ujtype.imm = instruction_bits & ((1U << 20) - 1);
    instruction_bits >>= 20;
    break;


  default:
    exit(EXIT_FAILURE);
  }
  return instruction;
}




/* Return the number of bytes (from the current PC) to the branch label using
 * the given branch instruction */
int get_branch_offset(Instruction instruction) {
  /* YOUR CODE HERE */
  int firstFour = (instruction.sbtype.imm5 & 0b11110);
  int eleventhBit = instruction.sbtype.imm5 & 0b00001;
  eleventhBit <<= 10;
  int secondSeven = instruction.sbtype.imm7 & 0b1011111;
  secondSeven <<= 5;
  int result = secondSeven | firstFour;
  result |= eleventhBit;
  return sign_extend_number(result, 12);
}




/* Returns the number of bytes (from the current PC) to the jump label using the
 * given jump instruction */
int get_jump_offset(Instruction instruction) {
  /* YOUR CODE HERE */
  int first_ten_bits = instruction.ujtype.imm & 0b01111111111000000000;
  first_ten_bits >>= 9;
  int last_bit = instruction.ujtype.imm & 0b10000000000000000000;
  int bit_eleven = instruction.ujtype.imm & 0b00000000000100000000;
  bit_eleven <<= 2;
  int twelve_to_nineteen = instruction.ujtype.imm & 0b00000000000011111111;
  twelve_to_nineteen <<= 11;
  int result = first_ten_bits | last_bit | bit_eleven | twelve_to_nineteen;
  result <<= 1;
  return sign_extend_number(result, 20);
}




int get_store_offset(Instruction instruction) {
  /* YOUR CODE HERE */
  int fiveBits = instruction.stype.imm5;
  int sevenBits = instruction.stype.imm7;
  int result = fiveBits | (sevenBits << 5);
  return sign_extend_number(result, 12);
}

void handle_invalid_instruction(Instruction instruction) {
  printf("Invalid Instruction: 0x%08x\n", instruction.bits);
}




void handle_invalid_read(Address address) {
  printf("Bad Read. Address: 0x%08x\n", address);
  exit(-1);
}




void handle_invalid_write(Address address) {
  printf("Bad Write. Address: 0x%08x\n", address);
  exit(-1);
}
