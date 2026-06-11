#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <errno.h>
#include <stdbool.h>

typedef const char* PACKAGENAME4patch;

static int mem_handle = -1;

#define MAX_FOUND_MODULES 16


typedef struct BackupNode {
    long address;
    unsigned char *original_bytes;
    size_t size;
    struct BackupNode *next;
} BackupNode;

static BackupNode *backup_list = NULL;

static int charToHexValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static char *clearHex(const char *hexStr) {
    if (!hexStr) return NULL;
    size_t len = strlen(hexStr);
    char *stripped = (char *)malloc(len + 1);
    if (!stripped) return NULL; 

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        char c = hexStr[i];
        if (c != ' ' && c != '\t' && c != 'h' && c != 'H') {
            stripped[j++] = c;
        }
    }
    stripped[j] = '\0';
    char *shrunk = (char *)realloc(stripped, j + 1);
    return shrunk ? shrunk : stripped; 
}

static int hexToBytes(const char *hexStr, unsigned char *output) {
    size_t len = strlen(hexStr);
    if (len % 2 != 0) {
        return -1;
    }

    size_t byteLen = len / 2;
    for (size_t i = 0; i < byteLen; i++) {
        int high = charToHexValue(hexStr[i * 2]);
        int low = charToHexValue(hexStr[i * 2 + 1]);

        if (high == -1 || low == -1) {
            return -1;
        }

        output[i] = (unsigned char)((high << 4) | low);
    }

    return (int)byteLen;
}

int getPID(PACKAGENAME4patch PackageName) {
    DIR *dir = opendir("/proc");
    if (dir == NULL) return 0;

    struct dirent *ptr = NULL;
    int found_pid = 0;

    while ((ptr = readdir(dir)) != NULL) {
        if (ptr->d_type == DT_DIR && strspn(ptr->d_name, "0123456789") == strlen(ptr->d_name)) {
            char filepath[256];
            char filetext[256] = {0};

            snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", ptr->d_name);

            FILE *fp = fopen(filepath, "r");
            if (fp != NULL) {
                if (fgets(filetext, sizeof(filetext) - 1, fp) != NULL) {
                    if (strcmp(filetext, PackageName) == 0) {
                        found_pid = atoi(ptr->d_name);
                        fclose(fp);
                        break;
                    }
                }
                fclose(fp);
            }
        }
    }

    closedir(dir);
    return found_pid;
}

int getSharedLibMulti(int pid, const char *module_name, long *addrs_out, char *module_path_out, size_t path_out_size) {
    FILE *fp;
    char filepath[64];
    char buff[512];
    int count = 0;

    snprintf(filepath, sizeof(filepath), "/proc/%d/maps", pid);
    fp = fopen(filepath, "r");
    if (fp) {
        while (!feof(fp) && fgets(buff, sizeof(buff), fp)) {
            if (strstr(buff, module_name) != NULL) {
                
                long addr = 0;
                long file_offset = 0;
               
                if (sscanf(buff, "%lx-%*lx %*s %lx", &addr, &file_offset) == 2) {                
                    if (file_offset == 0) {                     
                        bool already_exists = false;
                        for (int i = 0; i < count; i++) {
                            if (addrs_out[i] == addr) {
                                already_exists = true;
                                break;
                            }
                        }

                        if (!already_exists && count < MAX_FOUND_MODULES) {
                            addrs_out[count] = addr;
                            
                            if (count == 0) {
                                char *path_start = strchr(buff, '/');
                                if (path_start) {
                                    char *path_end = path_start + strlen(path_start) - 1;
                                    while(path_end > path_start && isspace((unsigned char)*path_end)) {
                                        *path_end-- = '\0';
                                    }                  
                                    strncpy(module_path_out, path_start, path_out_size - 1);
                                    module_path_out[path_out_size - 1] = '\0';
                                }
                            }
                            count++;
                        }
                    }
                }
            }
        }
        fclose(fp);
    }
    return count;
}


int WriteBaseHex(long int addr, const unsigned char *bytes, size_t size) {
    if (mem_handle == -1) return -1;
    if (pwrite64(mem_handle, bytes, size, addr) != size) return -1;
    return 0;
}


static void memoryBackup(int fd, long target_address, size_t patch_size) {
    if (fd == -1) return;
    BackupNode *curr = backup_list;
    while (curr) {
        if (curr->address == target_address) {
            return; 
        }
        curr = curr->next;
    }

    size_t backup_size = (patch_size > 12) ? patch_size : 12;

    unsigned char *buf = (unsigned char *)malloc(backup_size);
    if (!buf) return;

    if (pread64(fd, buf, backup_size, target_address) == backup_size) {
        BackupNode *new_node = (BackupNode *)malloc(sizeof(BackupNode));
        if (new_node) {
            new_node->address = target_address;
            new_node->original_bytes = buf;
            new_node->size = backup_size;
            new_node->next = backup_list;
            backup_list = new_node;
        } else {
            free(buf);
        }
    } else {
        free(buf);
    }
}

int readBytesFromLib(const char *filepath, long offset, unsigned char *output, size_t size) {
    int fd = open(filepath, O_RDONLY); 
    if (fd == -1) return -1;
    if (pread64(fd, output, size, offset) != size) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static unsigned int parse_mov_w_imm(const char *instruction) {
    int reg, imm;
    if (sscanf(instruction, "MOV W%d, #%i", &reg, &imm) == 2 ||
        sscanf(instruction, "MOV W%d, #0x%x", &reg, &imm) == 2) {
        if (reg >= 0 && reg <= 31 && imm >= 0 && imm <= 0xFFFF) {
            return 0x52800000 | (imm << 5) | reg;
        }
    }
    return 0;
}

static unsigned int parse_mov_x_x(const char *instruction) {
    int rd, rs;
    if (sscanf(instruction, "MOV X%d, X%d", &rd, &rs) == 2) {
        if (rd >= 0 && rd <= 31 && rs >= 0 && rs <= 31) {
            return 0xAA0003E0 | (rs << 16) | rd;
        }
    }
    return 0;
}

static unsigned int parse_ret(const char *instruction) {
    if (strcmp(instruction, "RET") == 0) {
        return 0xD65F03C0;
    }
    return 0;
}

static unsigned int parse_nop(const char *instruction) {
    if (strcmp(instruction, "NOP") == 0) {
        return 0xD503201F;
    }
    return 0;
}

static unsigned int parse_add_x(const char *instruction) {
    int rd, rn, rm;
    if (sscanf(instruction, "ADD X%d, X%d, X%d", &rd, &rn, &rm) == 3) {
        if (rd >= 0 && rd <= 31 && rn >= 0 && rn <= 31 && rm >= 0 && rm <= 31) {
            return 0x8B000000 | (rm << 16) | (rn << 5) | rd;
        }
    }
    return 0;
}

static unsigned int parse_b_pc(const char *instruction) {
    int offset;
    if (sscanf(instruction, "B #%i", &offset) == 1 ||
        sscanf(instruction, "B #0x%x", &offset) == 1) {
        if (offset % 4 == 0 && offset >= -0x4000000 && offset <= 0x3FFFFFF) {
            unsigned int imm26 = (offset >> 2) & 0x3FFFFFF;
            return 0x14000000 | imm26;
        }
    }
    return 0;
}

static void trim_and_clean_asm_instruction(char *str) {
    int len = strlen(str);
    if (len == 0) return;
    int end = len - 1;
    while (end >= 0 && (isspace((unsigned char)str[end]) || str[end] == '\n')) {
        str[end--] = '\0';
    }
    int start = 0;
    while (str[start] != '\0' && isspace((unsigned char)str[start])) {
        start++;
    }

    if (start > 0) {
        memmove(str, str + start, end - start + 2);
    }
}

static char* assembleToHex(const char* asm_input) {
    char *input_copy = strdup(asm_input);
    if (!input_copy) return NULL;
  
    char *output_hex_str = (char*)malloc(512); 
    if (!output_hex_str) { free(input_copy); return NULL; }
    output_hex_str[0] = '\0';

    char *token = strtok(input_copy, ";");
    while (token != NULL) {
        trim_and_clean_asm_instruction(token);
        unsigned int hex = 0;
        
        if (strncmp(token, "MOV W", 5) == 0) {
            hex = parse_mov_w_imm(token);
        } else if (strncmp(token, "MOV X", 5) == 0) {
            hex = parse_mov_x_x(token);
        } else if (strcmp(token, "RET") == 0) {
            hex = parse_ret(token);
        } else if (strcmp(token, "NOP") == 0) {
            hex = parse_nop(token);
        } else if (strncmp(token, "ADD X", 5) == 0) {
            hex = parse_add_x(token);
        } else if (strncmp(token, "B #", 3) == 0) {
            hex = parse_b_pc(token);
        }
        
        if (hex != 0) {
            char hex_str[9];
            snprintf(hex_str, sizeof(hex_str), "%02X%02X%02X%02X", 
                     (hex >> 0) & 0xFF, (hex >> 8) & 0xFF, 
                     (hex >> 16) & 0xFF, (hex >> 24) & 0xFF);
            strcat(output_hex_str, hex_str);
        } else if (token[0] != '\0') {
            free(input_copy);
            free(output_hex_str);
            return strdup("ERROR");
        }
        token = strtok(NULL, ";");
    }
    free(input_copy);
    return output_hex_str;
}

static char* getCleanPatchHex(const char *inputStr, bool isParameterPatch) {
    bool isHex = true;
    size_t len = strlen(inputStr);
    for (size_t i = 0; i < len; i++) {
        char c = inputStr[i];
        if (c != ' ' && c != '\t' && c != 'h' && c != 'H') {
            if (!isxdigit((unsigned char)c)) {
                isHex = false;
                break;
            }
        }
    }

    if (isHex) {
        char *stripped_hex_str = clearHex(inputStr);
        if (stripped_hex_str && isParameterPatch && strlen(stripped_hex_str) != 8) {
            free(stripped_hex_str);
            return NULL;
        }
        return stripped_hex_str;
    } else {
        char *asm_hex_str = assembleToHex(inputStr);
        if (asm_hex_str == NULL || strcmp(asm_hex_str, "ERROR") == 0) {
            if (asm_hex_str) free(asm_hex_str);
            return NULL;
        }
        if (isParameterPatch && strlen(asm_hex_str) != 8) {
            free(asm_hex_str);
            return NULL;
        }
        
        return asm_hex_str;
    }
}

int hexPatch(PACKAGENAME4patch packageName, const char *offsetHex, const char *hexPatchStr, const char *offsetModifierStr, const char *moduleName) {
    int pid = getPID(packageName);
    if (pid == 0) return -1; 
    
    char *stripped_hex_str = getCleanPatchHex(hexPatchStr, false);
    if (!stripped_hex_str) return -1;
    
    size_t max_bytes = strlen(stripped_hex_str) / 2;
    unsigned char *patch_bytes = (unsigned char *)malloc(max_bytes);
    if (!patch_bytes) { 
        free(stripped_hex_str);
        return -1;
    }

    int patch_size = hexToBytes(stripped_hex_str, patch_bytes);
    free(stripped_hex_str);
    
    if (patch_size <= 0) { 
        free(patch_bytes); 
        return -1;
    }
    
    char mem_path[64];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    mem_handle = open(mem_path, O_RDWR);
    if (mem_handle == -1) { 
        free(patch_bytes);
        return -1;
    }
    
    char temp_path[256];
    long module_bases[MAX_FOUND_MODULES];
    int modules_count = getSharedLibMulti(pid, moduleName, module_bases, temp_path, sizeof(temp_path));
    
    if (modules_count == 0) {
        close(mem_handle);
        mem_handle = -1;
        free(patch_bytes);
        return -1;
    }

    int result = -1;
    long offset = strtol(offsetHex, NULL, 0); 
    
    if (offsetModifierStr != NULL) {
        long modifier = strtol(offsetModifierStr, NULL, 0);
        offset += modifier;
    }

    for (int i = 0; i < modules_count; i++) {
        long target_address = module_bases[i] + offset;
        memoryBackup(mem_handle, target_address, patch_size);
        
        if (WriteBaseHex(target_address, patch_bytes, patch_size) == 0) {
            result = 0; 
        }
    }

    free(patch_bytes);
    close(mem_handle);
    mem_handle = -1;
    return result;
}


int resPatch(PACKAGENAME4patch packageName, const char *offsetHex, const char *offsetModifierStr, const char *moduleName) {
    int pid = getPID(packageName);
    if (pid == 0) return -1;

    char module_filepath[256];
    long module_bases[MAX_FOUND_MODULES];
    int modules_count = getSharedLibMulti(pid, moduleName, module_bases, module_filepath, sizeof(module_filepath));
    
    if (modules_count == 0) return -1;
    long offset = strtol(offsetHex, NULL, 0); 
    if (offsetModifierStr != NULL) {
        long modifier = strtol(offsetModifierStr, NULL, 0);
        offset += modifier;
    }

    char mem_path[64];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    mem_handle = open(mem_path, O_RDWR);
    if (mem_handle == -1) return -1;
    
    int result = -1;
    bool restored_from_backup = false;
    
    for (int i = 0; i < modules_count; i++) {
        long target_address_ram = module_bases[i] + offset;
        
        BackupNode *curr = backup_list;
        while (curr) {
            if (curr->address == target_address_ram) {
                if (WriteBaseHex(target_address_ram, curr->original_bytes, curr->size) == 0) {
                    result = 0;
                    restored_from_backup = true;
                }
                break;
            }
            curr = curr->next;
        }
    }
    
    if (!restored_from_backup) {
        size_t rollback_size = 12; 
        unsigned char *file_bytes = (unsigned char *)malloc(rollback_size);
        
        if (file_bytes) {
            if (readBytesFromLib(module_filepath, offset, file_bytes, rollback_size) == 0) {
                for (int i = 0; i < modules_count; i++) {
                    long target_address_ram = module_bases[i] + offset;
                    if (WriteBaseHex(target_address_ram, file_bytes, rollback_size) == 0) {
                        result = 0;
                    }
                }
            }
            free(file_bytes);
        }
    }
    
    close(mem_handle);
    mem_handle = -1;
    return result;
}



int hookMethod(PACKAGENAME4patch packageName, const char *offset1Hex, const char *offset2Hex, const char *moduleName) {
    int pid = getPID(packageName);
    if (pid == 0) return -1;

    char temp_path[256]; 
    long module_bases[MAX_FOUND_MODULES];
    int modules_count = getSharedLibMulti(pid, moduleName, module_bases, temp_path, sizeof(temp_path));
    if (modules_count == 0) return -1;

    long offset1 = strtol(offset1Hex, NULL, 0); 
    long offset2 = strtol(offset2Hex, NULL, 0);
    
    long distance_bytes = offset2 - offset1;
    long distance_instructions = distance_bytes / 4; 

    if (distance_bytes % 4 != 0) return -1;
    if (distance_instructions > 0x1FFFFFFF || distance_instructions < -0x20000000) return -1;
    
    unsigned int instruction = (unsigned int)(distance_instructions & 0x3FFFFFF) | 0x14000000;
    unsigned char patch_bytes[4]; 
    patch_bytes[0] = (unsigned char)(instruction & 0xFF);
    patch_bytes[1] = (unsigned char)((instruction >> 8) & 0xFF);
    patch_bytes[2] = (unsigned char)((instruction >> 16) & 0xFF);
    patch_bytes[3] = (unsigned char)((instruction >> 24) & 0xFF);
    
    char mem_path[64];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    mem_handle = open(mem_path, O_RDWR);
    if (mem_handle == -1) return -1; 

    int result = -1;
    for (int i = 0; i < modules_count; i++) {
        long target_address = module_bases[i] + offset1; 
        memoryBackup(mem_handle, target_address, 4);

        if (WriteBaseHex(target_address, patch_bytes, 4) == 0) {
            result = 0;
        }
    }
    
    close(mem_handle);
    mem_handle = -1;
    return result;
}



int hookField(PACKAGENAME4patch packageName, const char *offsetHex, const char *valStr, const char *offset2Hex, const char *moduleName) {
    int pid = getPID(packageName);
    if (pid == 0) return -1;

    char temp_path[256]; 
    long module_bases[MAX_FOUND_MODULES];
    int modules_count = getSharedLibMulti(pid, moduleName, module_bases, temp_path, sizeof(temp_path));
    if (modules_count == 0) return -1;

    long offset = strtol(offsetHex, NULL, 0); 
    long offset2 = strtol(offset2Hex, NULL, 0);
    if (offset2 < 0 || offset2 > 0x3FFC || offset2 % 4 != 0) return -1; 

    unsigned int final_value_raw = 0;

    if (strchr(valStr, '.') != NULL) {
        float f_val = strtof(valStr, NULL);
        memcpy(&final_value_raw, &f_val, sizeof(float));
    } else {
        long i_val = strtol(valStr, NULL, 0);
        final_value_raw = (unsigned int)i_val;
    }
    
    unsigned char patch_bytes[16]; 
    unsigned int ldr_instruction = 0x18000061; 
    unsigned int imm12 = (unsigned int)offset2 / 4;
    unsigned int str_instruction = 0xB9000001 | (imm12 << 10);
    unsigned int ret_instruction = 0xD65F03C0; 

    patch_bytes[0] = (unsigned char)(ldr_instruction & 0xFF);
    patch_bytes[1] = (unsigned char)((ldr_instruction >> 8) & 0xFF);
    patch_bytes[2] = (unsigned char)((ldr_instruction >> 16) & 0xFF);
    patch_bytes[3] = (unsigned char)((ldr_instruction >> 24) & 0xFF);
  
    patch_bytes[4] = (unsigned char)(str_instruction & 0xFF);
    patch_bytes[5] = (unsigned char)((str_instruction >> 8) & 0xFF);
    patch_bytes[6] = (unsigned char)((str_instruction >> 16) & 0xFF);
    patch_bytes[7] = (unsigned char)((str_instruction >> 24) & 0xFF);
    
    patch_bytes[8] = (unsigned char)(ret_instruction & 0xFF);
    patch_bytes[9] = (unsigned char)((ret_instruction >> 8) & 0xFF);
    patch_bytes[10] = (unsigned char)((ret_instruction >> 16) & 0xFF);
    patch_bytes[11] = (unsigned char)((ret_instruction >> 24) & 0xFF);

    patch_bytes[12] = (unsigned char)(final_value_raw & 0xFF);
    patch_bytes[13] = (unsigned char)((final_value_raw >> 8) & 0xFF);
    patch_bytes[14] = (unsigned char)((final_value_raw >> 16) & 0xFF);
    patch_bytes[15] = (unsigned char)((final_value_raw >> 24) & 0xFF);

    char mem_path[64];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    mem_handle = open(mem_path, O_RDWR);
    if (mem_handle == -1) return -1; 

    int result = -1;
    for (int i = 0; i < modules_count; i++) {
        long target_address = module_bases[i] + offset; 
        
        memoryBackup(mem_handle, target_address, 16);

        if (WriteBaseHex(target_address, patch_bytes, 16) == 0) {
            result = 0;
        }
    }
    
    close(mem_handle);
    return result;
}

