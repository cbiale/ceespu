#ifndef EMULATOR_CEESPU_H
#define EMULATOR_CEESPU_H

#define CeespuMemorySize (1 << 14)

#include <stdint.h>
#include <stdlib.h>
#include <atomic>
#include <cstdio>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __linux__
#include <time.h>
#include <unistd.h>
#endif

class Video;

typedef union _memory {
  uint32_t word;
  uint16_t hword[2];
  uint8_t byte[4];
} Memory;

class Ceespu {
 public:
  Ceespu();
  ~Ceespu();
  void init(Video *screen);
  void emulateCycle();               // Emulate one cycle
  bool load(const char *file_path);  // Load application
  void timerInterrupt();             // Do timer interrupt
  void recieveInterrupt(char c);
  uint32_t getWord(uint16_t location) const;
  uint16_t getHalfword(uint16_t location) const;
  uint8_t getByte(uint16_t location) const;
  friend void crash(Ceespu *cpu, const char *error);
  std::atomic<bool> running;

 private:
  void storeWord(uint16_t location, uint32_t data);
  void storeHalfword(uint16_t location, uint16_t data);
  void storeByte(uint16_t location, uint8_t data);
  uint32_t Regs[32];
  uint16_t pc;
  Memory memory[CeespuMemorySize];
  Video *screen;
  bool carry;
  std::atomic<bool> interrupt;
  std::atomic<uint8_t> int_vector;
  std::atomic<char> reveiced_char;
  bool enable_interrupt;
  bool immidiate_valid;
  uint16_t immidiate_reg;
};

enum {
  ADD = 0,
  ADC = 1,
  SUB = 2,
  SBB = 3,
  OR = 4,
  AND = 5,
  XOR = 6,
  SE = 7,
  SHF = 8,
  MUL = 9,
  ADDI = 16,
  ADCI = 17,
  SUBI = 18,
  SBBI = 19,
  ORI = 20,
  ANDI = 21,
  XORI = 22,
  SHFI = 24,
  MULI = 25,
  LW = 32,
  LH = 33,
  LB = 34,
  LHU = 35,
  LBU = 36,
  SETI = 42,
  EI = 43,
  SW = 52,
  SH = 53,
  SB = 54,
  BEQ = 56,
  BNE = 57,
  BGT = 58,
  BGE = 59,
  BGU = 60,
  BGEU = 61,
  BC = 62,
  B = 63
};

const unsigned char font[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 0
    0x00, 0x00, 0x7E, 0x81, 0xA5, 0x81, 0x81, 0xBD,
    0x99, 0x81, 0x81, 0x7E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 1
    0x00, 0x00, 0x3E, 0x7F, 0x7F, 0x6B, 0x7F, 0x7F,
    0x5D, 0x63, 0x7F, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 2
    0x00, 0x00, 0x00, 0x36, 0x77, 0x7F, 0x7F, 0x7F,
    0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00,  // Code for char num 3
    0x00, 0x00, 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x3E,
    0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 4
    0x00, 0x00, 0x00, 0x08, 0x1C, 0x1C, 0x08, 0x36,
    0x77, 0x36, 0x08, 0x1C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 5
    0x00, 0x00, 0x08, 0x1C, 0x3E, 0x3E, 0x7F, 0x7F,
    0x7F, 0x36, 0x08, 0x1C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x3C,
    0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 7
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0xC3,
    0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Code for char num 8
    0x00, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x66, 0x66,
    0x66, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 9
    0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0x99, 0x99,
    0x99, 0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Code for char num 10
    0x00, 0x00, 0x78, 0x70, 0x78, 0x6C, 0x1E, 0x33,
    0x33, 0x33, 0x33, 0x1E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 11
    0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18,
    0x7E, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,  // Code for char num 12
    0x00, 0x00, 0x78, 0x58, 0x78, 0x18, 0x18, 0x18,
    0x18, 0x1E, 0x1F, 0x0E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 13
    0x00, 0x00, 0x7C, 0x6C, 0x7C, 0x6C, 0x6C, 0x6E,
    0x6F, 0x66, 0x70, 0x78, 0x30, 0x00, 0x00, 0x00,  // Code for char num 14
    0x00, 0x00, 0x18, 0xDB, 0x7E, 0x3C, 0x66, 0x66,
    0x3C, 0x7E, 0xDB, 0x18, 0x00, 0x00, 0x00, 0x00,  // Code for char num 15
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Code for char num 16
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 17
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 18
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 19
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 20
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 21
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 22
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 23
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 24
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 25
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 26
    0x00, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00,  // Code for char num 27
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 28
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 29
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 30
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 31
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 32
    0x00, 0x00, 0x18, 0x3C, 0x3C, 0x3C, 0x3C, 0x18,
    0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,  // Code for char num 33
    0x00, 0x6C, 0x6C, 0x6C, 0x6C, 0x28, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 34
    0x00, 0x00, 0x36, 0x36, 0x36, 0x7F, 0x36, 0x36,
    0x7F, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00,  // Code for char num 35
    0x00, 0x00, 0x18, 0x18, 0x3E, 0x63, 0x03, 0x1E,
    0x3C, 0x60, 0x63, 0x3E, 0x18, 0x18, 0x00, 0x00,  // Code for char num 36
    0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x66, 0x30,
    0x18, 0x0C, 0x66, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 37
    0x00, 0x00, 0x1C, 0x36, 0x1C, 0x0C, 0x6E, 0x7E,
    0x33, 0x33, 0x33, 0x6E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 38
    0x00, 0x30, 0x30, 0x30, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 39
    0x00, 0x00, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00,  // Code for char num 40
    0x00, 0x00, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 41
    0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x1C, 0x7F,
    0x1C, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 42
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7E,
    0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 43
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x30, 0x30, 0x18, 0x00, 0x00, 0x00,  // Code for char num 44
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 45
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,  // Code for char num 46
    0x00, 0x00, 0x00, 0x00, 0x40, 0x60, 0x30, 0x18,
    0x0C, 0x06, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char num 47
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x73, 0x7B, 0x6F,
    0x67, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 48
    0x00, 0x00, 0x18, 0x1E, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 49
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x60, 0x30, 0x18,
    0x0C, 0x06, 0x63, 0x7F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 50
    0x00, 0x00, 0x3E, 0x63, 0x60, 0x60, 0x3C, 0x60,
    0x60, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 51
    0x00, 0x00, 0x30, 0x38, 0x3C, 0x36, 0x33, 0x33,
    0x7F, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0x00,  // Code for char num 52
    0x00, 0x00, 0x7F, 0x03, 0x03, 0x03, 0x3F, 0x60,
    0x60, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 53
    0x00, 0x00, 0x3E, 0x63, 0x03, 0x03, 0x3F, 0x63,
    0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 54
    0x00, 0x00, 0x7F, 0x63, 0x60, 0x30, 0x18, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 55
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x3E, 0x63,
    0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 56
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x7E,
    0x60, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 57
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,
    0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 58
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,
    0x00, 0x30, 0x30, 0x30, 0x18, 0x00, 0x00, 0x00,  // Code for char num 59
    0x00, 0x00, 0x00, 0x30, 0x18, 0x0C, 0x06, 0x03,
    0x06, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00,  // Code for char num 60
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00,
    0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 61
    0x00, 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60,
    0x30, 0x18, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00,  // Code for char num 62
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x30, 0x18, 0x18,
    0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,  // Code for char num 63
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x7B, 0x7B,
    0x7B, 0x3B, 0x03, 0x7E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 64
    0x00, 0x00, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x7F,
    0x63, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 65
    0x00, 0x00, 0x3F, 0x66, 0x66, 0x66, 0x3E, 0x66,
    0x66, 0x66, 0x66, 0x3F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 66
    0x00, 0x00, 0x3C, 0x66, 0x43, 0x03, 0x03, 0x03,
    0x03, 0x43, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 67
    0x00, 0x00, 0x1F, 0x36, 0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x36, 0x1F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 68
    0x00, 0x00, 0x7F, 0x66, 0x06, 0x26, 0x3E, 0x26,
    0x06, 0x06, 0x66, 0x7F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 69
    0x00, 0x00, 0x7F, 0x66, 0x06, 0x26, 0x3E, 0x26,
    0x06, 0x06, 0x06, 0x0F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 70
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x03, 0x03, 0x03,
    0x73, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 71
    0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x7F, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 72
    0x00, 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 73
    0x00, 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x1B, 0x1B, 0x0E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 74
    0x00, 0x00, 0x63, 0x63, 0x33, 0x1B, 0x0F, 0x0F,
    0x1B, 0x33, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 75
    0x00, 0x00, 0x0F, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x46, 0x66, 0x7F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 76
    0x00, 0x00, 0x63, 0x63, 0x77, 0x77, 0x7F, 0x6B,
    0x6B, 0x6B, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 77
    0x00, 0x00, 0x63, 0x63, 0x67, 0x67, 0x6F, 0x7B,
    0x73, 0x73, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 78
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 79
    0x00, 0x00, 0x3F, 0x66, 0x66, 0x66, 0x66, 0x3E,
    0x06, 0x06, 0x06, 0x0F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 80
    0x00, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x6B, 0x6B, 0x3E, 0x60, 0x00, 0x00, 0x00,  // Code for char num 81
    0x00, 0x00, 0x3F, 0x66, 0x66, 0x66, 0x3E, 0x1E,
    0x36, 0x66, 0x66, 0x67, 0x00, 0x00, 0x00, 0x00,  // Code for char num 82
    0x00, 0x00, 0x3E, 0x63, 0x03, 0x03, 0x0E, 0x38,
    0x60, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 83
    0x00, 0x00, 0x7E, 0x5A, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 84
    0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 85
    0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x36, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00,  // Code for char num 86
    0x00, 0x00, 0x63, 0x63, 0x63, 0x6B, 0x6B, 0x6B,
    0x7F, 0x77, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 87
    0x00, 0x00, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x1C,
    0x36, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 88
    0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C,
    0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 89
    0x00, 0x00, 0x7F, 0x63, 0x61, 0x30, 0x18, 0x0C,
    0x06, 0x43, 0x63, 0x7F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 90
    0x00, 0x00, 0x3E, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 91
    0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x0C,
    0x18, 0x30, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,  // Code for char num 92
    0x00, 0x00, 0x3E, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 93
    0x00, 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 94
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,  // Code for char num 95
    0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 96
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x30, 0x3E,
    0x33, 0x33, 0x3B, 0x6E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 97
    0x00, 0x00, 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x3F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 98
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x63, 0x03,
    0x03, 0x03, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 99
    0x00, 0x00, 0x38, 0x30, 0x30, 0x3E, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x7E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 100
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x63, 0x63,
    0x7F, 0x03, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 101
    0x00, 0x00, 0x38, 0x6C, 0x0C, 0x0C, 0x3F, 0x0C,
    0x0C, 0x0C, 0x0C, 0x1E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 102
    0x00, 0x00, 0x00, 0x00, 0x00, 0x6E, 0x73, 0x63,
    0x63, 0x73, 0x6E, 0x60, 0x63, 0x3E, 0x00, 0x00,  // Code for char num 103
    0x00, 0x00, 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x67, 0x00, 0x00, 0x00, 0x00,  // Code for char num 104
    0x00, 0x00, 0x18, 0x18, 0x00, 0x1C, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,  // Code for char num 105
    0x00, 0x00, 0x30, 0x30, 0x00, 0x38, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00, 0x00,  // Code for char num 106
    0x00, 0x00, 0x07, 0x06, 0x06, 0x66, 0x66, 0x36,
    0x1E, 0x36, 0x66, 0x67, 0x00, 0x00, 0x00, 0x00,  // Code for char num 107
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00,  // Code for char num 108
    0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x7F, 0x6B,
    0x6B, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 109
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,  // Code for char num 110
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 111
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x66, 0x66,
    0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00, 0x00,  // Code for char num 112
    0x00, 0x00, 0x00, 0x00, 0x00, 0x6E, 0x33, 0x33,
    0x33, 0x33, 0x3E, 0x30, 0x30, 0x78, 0x00, 0x00,  // Code for char num 113
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x66, 0x06,
    0x06, 0x06, 0x06, 0x0F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 114
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x63, 0x03,
    0x3E, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 115
    0x00, 0x00, 0x0C, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C,
    0x0C, 0x0C, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00,  // Code for char num 116
    0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x6E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 117
    0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x63, 0x63,
    0x63, 0x36, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00,  // Code for char num 118
    0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x63, 0x6B,
    0x6B, 0x6B, 0x7F, 0x36, 0x00, 0x00, 0x00, 0x00,  // Code for char num 119
    0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x63, 0x36,
    0x1C, 0x36, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,  // Code for char num 120
    0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x63, 0x63,
    0x63, 0x73, 0x6E, 0x60, 0x63, 0x3E, 0x00, 0x00,  // Code for char num 121
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x61, 0x30,
    0x18, 0x0C, 0x46, 0x7F, 0x00, 0x00, 0x00, 0x00,  // Code for char num 122
    0x00, 0x00, 0x70, 0x18, 0x18, 0x18, 0x0E, 0x18,
    0x18, 0x18, 0x18, 0x70, 0x00, 0x00, 0x00, 0x00,  // Code for char num 123
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,  // Code for char num 124
    0x00, 0x00, 0x0E, 0x18, 0x18, 0x18, 0x70, 0x18,
    0x18, 0x18, 0x18, 0x0E, 0x00, 0x00, 0x00, 0x00,  // Code for char num 125
    0x00, 0x00, 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char num 126
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x1C,
    0x36, 0x36, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00  // Code for char num 127
};
#endif  // EMULATOR_CEESPU_H
