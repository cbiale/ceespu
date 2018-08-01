// Copyright 2018 <Cees Wolfs>
#include "ceespu_assembler.h"

bool inline is16bit(int x) { return ((int16_t)x == x); }

bool ispowerof2(unsigned int x) { return x && !(x & (x - 1)); }

inline int roundUp(int n, int a) {
  // align value n to alignment a, works only for powers of 2
  // fails miserably otherwise
  return ((n & (a - 1)) == 0) ? n : n + a & ~(a - 1);
}

uint8_t Tokenize(const std::string& str, uint8_t& curtoken, uint8_t start) {
  // check if we reached the end of our string
  if (curtoken == str.length()) {
    return 0;
  }

  // Get the size of the next token in the string skipping all delimeters
  curtoken = str.find_first_not_of(" ,()\t", start);
  std::string::size_type pos = str.find_first_of(" ,()\t", curtoken);

  if (curtoken == (uint8_t)-1) {
    return 0;
  }

  if (pos == std::string::npos) {
    pos = str.length();
  }

  // check if the token is ; meaning the rest of the line is comment
  if (str[curtoken] == ';') {
    return 0;
  }

  return pos - curtoken;
}

int64_t getImmidiate(const char* immidiate, uint8_t size) {
  // All 32-bit immidiates fit in 10 charachters
  if (size > 10 || size < 1) {
    return invalid_immidiate;
  }
  char buf[10];
  char* endptr = buf;
  errno = 0;
  memcpy(buf, immidiate, size);
  buf[size] = 0;
  // do the actual conversion
  int64_t imm = (strtol(buf, &endptr, 0) & 0xffffffff);
  if (errno != 0 || (endptr == buf)) {
    // Error occured thus immidiate is invalid
    return invalid_immidiate;
  }
  return imm;
}

std::string getLabel(char* label, uint8_t size) {
  // if the token ends with ':' it is a label
  if (label[size - 1] != ':') {
    // not a label return empty string
    return std::string();
  }
  // return the label without the ':' character
  return std::string(label, size - 1);
}

uint8_t getDirective(char* directive, uint8_t size) {
  // all directives start with '.'
  if (!(directive[0] == '.')) {
    return INVALID;
  }
  // skip the '.' character
  directive++;
  // perform binary search for directive in table
  uint8_t left = 0;
  uint8_t right = nDirect - 1;
  while (left <= right) {
    uint8_t middle = (left + right) / 2;
    if (strncmp(directive, directives[middle], size - 1) == 0) {
      // directive found
      return middle;
    }
    if (strncmp(directive, directives[middle], size - 1) > 0) {
      left = middle + 1;
    } else {
      right = middle - 1;
    }
  }
  // directive not in table, return invalid
  return INVALID;
}

InstructionInfo getInstruction(char* mnemonic, uint8_t size) {
  // strcmp requires null termination
  mnemonic[size] = '\0';
  if (size > 5) {
    return invalid_instruction;
  }
  // perform binary search on instruction table
  uint8_t left = 0;
  uint8_t right = nInstrs - 1;
  while (left <= right) {
    uint8_t middle = (left + right) / 2;
    if (strcmp(mnemonic, instr[middle].Mnemonic) == 0) {
      // found instruction
      return instr[middle];
    }
    if (strcmp(mnemonic, instr[middle].Mnemonic) > 0) {
      left = middle + 1;
    } else {
      right = middle - 1;
    }
  }
  // instruction not found return invalid
  return invalid_instruction;
}

uint8_t getRegister(const char* reg, uint8_t size) {
  if (size > 3 || size < 2) {
    return -1;
  }
  if (reg[0] != 'c') {
    return -1;
  }
  if (size == 2) {
    if (isdigit(reg[1]) != 0) {
      return reg[1] - '0';
    }
    return -1;
  }
  // bit of a hack, but it does work
  int regnum = ((reg[1] - '0') * 10 + (reg[2] - '0'));
  if ((regnum < 32) && (regnum >= 0)) {
    return regnum;
  }
  if (reg[1] == 's' && reg[2] == 'p') {
    return 18;  // Ceespu stack pointer
  }
  if (reg[1] == 'l' && reg[2] == 'r') {
    return 19;  // Ceespu link register
  }
  if (reg[1] == 'i' && reg[2] == 'r') {
    return 17;  // Ceespu link register
  }
  return -1;  // invalid register
}

void parseLabel(std::string& label_name, int line_num, Label* function_label,
                std::unordered_map<std::string, Label>& symbol_table,
                uint16_t offset) {
  Label labelValue = {offset, LABEL_LOCAL, function_label};
  // Check if the label is already defined
  auto ret = symbol_table.insert(std::make_pair(label_name, labelValue));
  if (ret.second == false) {
    if (ret.first->second.type &
        LABEL_UNDEFINED) {  // label was in symbol table but still undefined
      ret.first->second.type &= ~LABEL_UNDEFINED;  // set label type to defined
      ret.first->second.offset = offset;
    } else {
      // Error label already defined
      fprintf(stderr, "Error duplicate label %s, found on line %d",
              label_name.c_str(), line_num);
      exit(1);
    }
  }
}

uint32_t parseInstruction(const std::string& line, uint8_t& curtoken,
                          bool& immset, uint16_t offset,
                          InstructionInfo instruction,
                          std::vector<Relocation>& relocations) {
  // get the next token
  uint8_t token_len = Tokenize(line, curtoken, curtoken);
  uint8_t rd = 0, ra = 0, rb = 0;
  uint16_t imm = 0;
  uint32_t instr = 0;

  switch (instruction.Type) {
    case A0: {
      rd = getRegister(&line[curtoken], token_len);
      if (rd == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      ra = getRegister(&line[curtoken], token_len);
      if (ra == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      rb = getRegister(&line[curtoken], token_len);
      if (rb == 0xff) {
        return -1;
      }
      break;
    }
    case A2: {
      rd = getRegister(&line[curtoken], token_len);
      if (rd == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      ra = getRegister(&line[curtoken], token_len);
      if (ra == 0xff) {
        return -1;
      }
      break;
    }
    case A3: {
      ra = getRegister(&line[curtoken], token_len);
      if (ra == 0xff) {
        return -1;
      }
      break;
    }
    case B0: {
      rd = getRegister(&line[curtoken], token_len);
      if (rd == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      ra = getRegister(&line[curtoken], token_len);
      if (ra == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      uint64_t immidiate = getImmidiate(&line[curtoken], token_len);
      if (!is16bit(immidiate)) {
        // Not an immidiate push relocation
        Relocation reloc = {std::string(line, curtoken, token_len), offset,
                            REL_LO16};
        relocations.push_back(reloc);
      } else {
        imm = immidiate;
      }
      break;
    }
    case B1: {
      ra = getRegister(&line[curtoken], token_len);
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      if (ra == 0xff) {
        return -1;
      }
      rb = getRegister(&line[curtoken], token_len);
      if (rb == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      uint64_t immidiate = getImmidiate(&line[curtoken], token_len);
      if (immidiate == invalid_immidiate) {
        // Not an immidiate push relocation
        Relocation reloc = {std::string(line, curtoken, token_len), offset,
                            REL_LO12};
        relocations.push_back(reloc);
      } else {
        imm = immidiate & 0x7FF;
        rd = (immidiate >> 11) & 0x1F;
      }
      break;
    }
    case B2: {
      uint64_t immidiate = getImmidiate(&line[curtoken], token_len);
      if (immidiate == invalid_immidiate) {
        // Not an immidiate push relocation
        Relocation reloc = {std::string(line, curtoken, token_len), offset,
                            REL_LO22};
        relocations.push_back(reloc);
        immidiate = 0;
      } else {
        instr |= immidiate & 0x1fffffc;  // set bits 24-2 with the branchtarget
      }
      break;
    }
    case B3: {
      rd = getRegister(&line[curtoken], token_len);
      if (rd == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      uint64_t immidiate = getImmidiate(&line[curtoken], token_len);
      if (immidiate == invalid_immidiate) {
        // Not an immidiate push relocation
        Relocation reloc = {std::string(line, curtoken, token_len), offset,
                            REL_LO16};
        relocations.push_back(reloc);
        immidiate = 0;
      } else {
        imm = immidiate;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      ra = getRegister(&line[curtoken], token_len);
      if (ra == 0xff) {
        return -1;
      }
      break;
    }
    case B4: {
      // store so rb goes first
      rb = getRegister(&line[curtoken], token_len);
      if (rb == 0xff) {
        return -1;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      uint64_t immidiate = getImmidiate(&line[curtoken], token_len);
      if (immidiate == invalid_immidiate) {
        // Not an immidiate push relocation
        Relocation reloc = {std::string(line, curtoken, token_len), offset,
                            REL_LO12};
        relocations.push_back(reloc);
        immidiate = 0;
      } else {
        imm = immidiate & 0x7FF;
        rd = (immidiate >> 11) & 0x1F;
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      ra = getRegister(&line[curtoken], token_len);
      if (ra == 0xff) {
        return -1;
      }
      break;
    }
    case B5: {
      uint64_t immidiate = getImmidiate(&line[curtoken], token_len);
      if (immidiate == invalid_immidiate) {
        Relocation reloc = {std::string(line, curtoken, token_len), offset,
                            REL_HI16};
        relocations.push_back(reloc);
      }
      imm = immidiate;
      immset = true;
      break;
    }
  }
  instr |= instruction.Opcode << 26;
  instr |= rd << 21;
  instr |= ra << 16;
  instr |= rb << 11;
  instr |= imm;
  instr |= instruction.FuncCode;
  if (Tokenize(line, curtoken, curtoken + token_len) != 0) {
    fprintf(stderr, "Error too many tokens\n");
    return -1;
  }
  return instr;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Error no input file specified\nUsage: %s <inputfile>\n",
            argv[0]);
    return 1;
  }
  std::string outputfile;
  if (argc == 3) {
    outputfile = argv[2];
  } else {
    outputfile = "output.bin";
  }
  std::ifstream input_file;
  std::string line;

  /*    elfio elffile;
      elffile.create(ELFCLASS32, ELFDATA2MSB);

      section* str_sec = elffile.sections.add(".strtab");
      str_sec->set_type(SHT_STRTAB);
      str_sec->set_addr_align(0x1);
      string_section_accessor str_writer(str_sec);

      section* sym_sec = elffile.sections.add(".symtab");
      sym_sec->set_type(SHT_SYMTAB);
      sym_sec->set_info(2);
      sym_sec->set_link(str_sec->get_index());
      sym_sec->set_addr_align(2);
      sym_sec->set_entry_size(2);

      std::vector<Label> function_labels;

      section* text_sec = nullptr;
      section* data_sec = nullptr;

      symbol_section_accessor symbol_writer(elffile, sym_sec);

      // Another way to add symbol
      // symbol_writer.add_symbol(str_writer, "_start", 0x00000000, 0, STB_WEAK,
      // STT_FUNC, 0, text_sec->get_index());
  */
  std::vector<uint8_t> data;
  std::unordered_map<std::string, Label> symbol_table;
  Label* function_label = nullptr;
  std::vector<Relocation> relocation_table;
  uint32_t offset = 0;
  int line_num = 0;
  bool immset = false;
  input_file.open(argv[1]);
  if (!input_file) {
    fprintf(stderr, "Error input file could not be opened\n");
    return 1;
  }
  uint8_t curtoken = 0;
  while (std::getline(input_file, line)) {
    line_num++;
    if (line.length() > 120) {
      fprintf(stderr, "Error max line length execeeded at line %d\n", line_num);
      return 1;
    }
    int token_len = Tokenize(line, curtoken, 0);
    if (token_len == 0) {
      continue;
    }
    std::string label = getLabel(&line[curtoken], token_len);
    if (!label.empty()) {
      // printf("line %d : label %s found\n", line_num, label.c_str());
      parseLabel(label, line_num, function_label, symbol_table, offset);
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      if (token_len == 0) {
        continue;
      }
    }
    uint8_t directive = getDirective(&line[curtoken], token_len);
    if (directive != INVALID) {
      // printf("line %d : directive %d found\n", line_num, directive);
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      if (token_len == 0) {
        fprintf(stderr, "Error expected token after directive .%s\n",
                directives[directive]);
        exit(1);
      }
      switch (directive) {
        case ALIGN: {
          uint64_t align = getImmidiate(&line[curtoken], token_len);
          if (!ispowerof2(align)) {
            fprintf(stderr, "Error invalid alignement requested at line%d\n",
                    line_num);
          }
          offset = roundUp(offset, align);
          break;
        }
        case DATA: {
          // Create data section*
          /*section* data_sec = elffile.sections.add(".data");
          data_sec->set_type(SHT_PROGBITS);
          data_sec->set_flags(SHF_ALLOC | SHF_WRITE);
          data_sec->set_addr_align(0x4);
           */
        }
        case TEXT: {
          // Create code section
          // text_sec = elffile.sections.add(".text");
          // text_sec->set_type(SHT_PROGBITS);
          // text_sec->set_flags(SHF_ALLOC | SHF_EXECINSTR);
          // text_sec->set_addr_align(0x4);
          break;
        }
        case GLOBL: {
          // symbol_writer.add_symbol(str_writer, &line[curtoken], offset, 0,
          // STB_GLOBAL, STT_FUNC, 0,
          //                         text_sec->get_index());
          // function_labels.clear();
          Label labelValue = {offset, LABEL_GLOBL | LABEL_UNDEFINED, nullptr};
          std::string label_name = std::string(line, curtoken, token_len);
          // Check if the label is already defined
          auto ret =
              symbol_table.insert(std::make_pair(label_name, labelValue));
          if (ret.second == false) {
            if (ret.first->second.type & LABEL_UNDEFINED) {
              symbol_table[label_name] = labelValue;
            }
            fprintf(stderr, "Error duplicate label definition at line %d\n",
                    line_num);
            exit(1);
          }
          break;
        }
        case SPACE: {
          uint64_t space = getImmidiate(&line[curtoken], token_len);
          if (space == invalid_immidiate) {
            printf("Error invalid immidiate after align directive\n");
          }
          data.insert(data.end(), space, 0);
          offset += space;
          break;
        }
        case WORD: {
          uint64_t value = getImmidiate(&line[curtoken], token_len);
          if (value == invalid_immidiate) {
            printf("Error invalid immidiate after word directive\n");
          }
          for (int i = 3; i >= 0; --i) {
            data.push_back((value >> (i * 8)) & 0xff);
          }
          offset += 4;
          break;
        }
        case HWORD: {
          uint64_t value = getImmidiate(&line[curtoken], token_len);
          if (value == invalid_immidiate) {
            printf("Error invalid immidiate after hword directive\n");
          }
          for (int i = 2; i >= 0; --i) {
            data.push_back((value >> (i * 8)) & 0xff);
          }
          offset += 2;
          break;
        }
        case BYTE: {
          uint64_t value = getImmidiate(&line[curtoken], token_len);
          if (value == invalid_immidiate) {
            printf("Error invalid immidiate after byte directive\n");
          }
          data.push_back(value & 0xff);
          offset++;
          break;
        }
      }
      token_len = Tokenize(line, curtoken, curtoken + token_len);
      if (token_len == 0) {
        continue;
      }
    }
    InstructionInfo instruction = getInstruction(&line[curtoken], token_len);
    if (instruction.Type != invalid_instruction.Type) {
      // printf("line %d : instruction %s found of type : %d\n", line_num,
      // instruction.Mnemonic, instruction.Type);
      curtoken += token_len + 1;
      uint32_t instr = parseInstruction(line, curtoken, immset, offset,
                                        instruction, relocation_table);
      if (instruction.Opcode != 0x2A) {
        immset = false;
      }
      offset += 4;
      if (instr == (uint32_t)-1) {
        fprintf(stderr, "Error invalid instruction %s at line %d\n",
                instruction.Mnemonic, line_num);
        exit(1);
      }
      for (int i = 3; i >= 0; --i) {
        data.push_back((instr >> (i * 8)) & 0xff);
      }
      continue;
    }
    fprintf(stderr, "Error invalid token on line %d\n", line_num);
    exit(1);
  }
  // here comes the linking portion, replace all label relocations with their
  // value. if the value is not found an error is returned
  for (auto it = relocation_table.begin(); it != relocation_table.end(); ++it) {
    auto label = symbol_table.find(it->label);
    if (label == symbol_table.end()) {
      fprintf(stderr, "Error label %s was never resolved\n", it->label.c_str());
      exit(1);
    }
    // relative jump
    if (it->type == REL_RJMP) {
      int jumpval = label->second.offset - it->offset;
      if (!is16bit(jumpval)) {
        fprintf(stderr, "Error relative jump to target %s is too far\n",
                label->first.c_str());
        exit(1);
      }
      data[it->offset + 1] |= ((jumpval >> 11) & 0xE0);
      data[it->offset + 2] |= ((jumpval >> 8) & 0x5);
      data[it->offset + 3] = jumpval & 0xff;
    } else if (it->type == REL_LO12) {
      data[it->offset + 1] |= ((label->second.offset >> 11) & 0xE0);
      data[it->offset + 2] |= ((label->second.offset >> 8) & 0x5);
      data[it->offset + 3] = label->second.offset & 0xff;
    } else if (it->type == REL_LO16) {
      /* code */
      data[it->offset + 2] = label->second.offset >> 8;
      data[it->offset + 3] = label->second.offset & 0xff;
    } else if (it->type == REL_LO22) {
      data[it->offset] |= (label->second.offset >> 24) & 0x1;
      data[it->offset + 1] = label->second.offset >> 16;
      data[it->offset + 2] = label->second.offset >> 8;
      data[it->offset + 3] |= label->second.offset & 0x3C;
    } else if (it->type == REL_HI16) {
      /* code */
      data[it->offset + 2] = label->second.offset >> 16;
      data[it->offset + 3] = label->second.offset >> 24;
    }
  }
  for (auto i = data.begin(); i != data.end(); ++i) {
    printf("%02X", *i);
  }
  std::ofstream out(outputfile, std::ios_base::binary);
  out.write(reinterpret_cast<char*>(&data[0]), data.size());
  out.close();
  return 0;
}