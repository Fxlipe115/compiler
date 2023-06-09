#include "assembly_generator.h"

#include "tac.h"

void generate_printf_strings(FILE* stream);

void generate_assembly_for_tac(FILE* stream, tac_t* tac);

void generate_init(FILE* stream, tac_t* tac);

void generate_temp(FILE* stream, tac_t* tac);

void generate_literal(FILE* stream, tac_t* tac);

void generate_vector_uninit(FILE* stream, tac_t* tac);

void generate_vector_init(FILE* stream, tac_t* tac);

void generate_vector_init_value(FILE* stream, tac_t* tac);

void generate_begin_function(FILE* stream, tac_t* tac);

void generate_end_function(FILE* stream, tac_t* tac);

void generate_vector_index(FILE* stream, tac_t* tac);

void generate_symbol(FILE* stream, tac_t* tac);

void generate_basic_arithmetic(FILE* stream, tac_t* tac);

void generate_sub(FILE* stream, tac_t* tac);

void generate_mul(FILE* stream, tac_t* tac);

void generate_div(FILE* stream, tac_t* tac);

void generate_comparison(FILE* stream, tac_t* tac);

void generate_dif(FILE* stream, tac_t* tac);

void generate_gt(FILE* stream, tac_t* tac);

void generate_ge(FILE* stream, tac_t* tac);

void generate_lt(FILE* stream, tac_t* tac);

void generate_le(FILE* stream, tac_t* tac);

void generate_move(FILE* stream, tac_t* tac);

void generate_vector_move(FILE* stream, tac_t* tac);

void generate_jump_false(FILE* stream, tac_t* tac);

void generate_jump(FILE* stream, tac_t* tac);

void generate_call(FILE* stream, tac_t* tac);

void generate_argument(FILE* stream, tac_t* tac);

void generate_parameter(FILE* stream, tac_t* tac);

void generate_return(FILE* stream, tac_t* tac);

void generate_print(FILE* stream, tac_t* tac);

void generate_read(FILE* stream, tac_t* tac);

void generate_label(FILE* stream, tac_t* tac);

size_t data_type_size(data_type_t type);

void convert_byte_to_int(FILE* stream, char* variable);

void convert_byte_to_float(FILE* stream, char* variable);

void convert_int_to_float(FILE* stream, char* variable);

void convert_data_type(FILE* stream, char* variable, data_type_t from, data_type_t to);

char* operator_to_mnemonic(tac_type_t operator, data_type_t data_type);




void generate_assembly(FILE* stream, list_t* tacs) {
    generate_printf_strings(stream);
    for(list_iterator_t it = list_begin(tacs); list_current(it) != NULL; list_next(&it)) {
        generate_assembly_for_tac(stream, list_current(it));
    }
}


void generate_printf_strings(FILE* stream) {
    fprintf(stream,
            ".intfmt:\n"
            "\t.string \"%%d\"\n"
            ".charfmt:\n"
            "\t.string \"%%c\"\n"
            ".floatfmt:\n"
            "\t.string \"%%f\"\n");
}


void generate_assembly_for_tac(FILE* stream, tac_t* tac) {
    switch (tac->type) {
        case tac_init:
            generate_init(stream, tac);
            break;
        case tac_temp:
            generate_temp(stream, tac);
            break;
        case tac_literal:
            generate_literal(stream, tac);
            break;
        case tac_vector_uninit:
            generate_vector_uninit(stream, tac);
            break;
        case tac_vector_init:
            generate_vector_init(stream, tac);
            break;
        case tac_vector_init_value:
            generate_vector_init_value(stream, tac);
            break;
        case tac_begin_function:
            generate_begin_function(stream, tac);
            break;
        case tac_end_function:
            generate_end_function(stream, tac);
            break;
        case tac_argument:
            generate_argument(stream, tac);
            break;
        case tac_call:
            generate_call(stream, tac);
            break;
        case tac_parameter:
            generate_parameter(stream, tac);
            break;
        case tac_return:
            generate_return(stream, tac);
            break;
        case tac_sum:
        case tac_sub:
        case tac_mul:
            generate_basic_arithmetic(stream, tac);
            break;
        case tac_div:
            generate_div(stream, tac);
            break;
        case tac_eq:
        case tac_dif:
        case tac_gt:
        case tac_ge:
        case tac_lt:
        case tac_le:
            generate_comparison(stream, tac);
            break;
        case tac_move:
            generate_move(stream, tac);
            break;
        case tac_label:
            generate_label(stream, tac);
            break;
        case tac_jump_false:
            generate_jump_false(stream, tac);
            break;
        case tac_jump:
            generate_jump(stream, tac);
            break;
        case tac_vector_index:
            generate_vector_index(stream, tac);
            break;
        case tac_vector_move:
            generate_vector_move(stream, tac);
            break;
        case tac_print:
            generate_print(stream, tac);
            break;
        case tac_read:
            generate_read(stream, tac);
            break;
        case tac_symbol:

            break;
    }
}


void generate_init(FILE* stream, tac_t* tac) {
    size_t size = data_type_size(tac->res->data_type);
    fprintf(stream, 
            "\t.globl %s\n"
            "\t.data\n"
            "\t.align %ld\n"
            "\t.type\t%s, @object\n"
            "\t.size\t%s, %ld\n"
            "%s:\n",
            tac->res->value, 
            size, 
            tac->res->value, 
            tac->res->value, 
            size, 
            tac->res->value);
        
    switch(tac->res->data_type) {
        case data_type_int:
            fprintf(stream, 
                    "\t.long %s\n",
                    tac->op1->value);
            break;
        case data_type_bool:
        case data_type_char:
            fprintf(stream, 
                    "\t.byte %s\n",
                    tac->op1->value);
            break;
        case data_type_float:
            fprintf(stream, 
                    "\t.float %f\n",
                    atof(tac->op1->value)/atof(tac->op2->value));
            break;
        case data_type_string:
            fprintf(stream, 
                    "\t.string %s\n",
                    tac->op1->value);
            break;
        default:
            break;
    }
}


void generate_temp(FILE* stream, tac_t* tac) {
    size_t size = data_type_size(tac->res->data_type);
    
    fprintf(stream,
            "\t.comm\t%s,%ld,%ld\n",
            tac->res->value, size, size);
}


void generate_literal(FILE* stream, tac_t* tac) {

}


size_t data_type_size(data_type_t type) {
    size_t size = 0;
    switch(type) {
        case data_type_int:
        case data_type_float:
            size = 4;
            break;
        case data_type_bool:
        case data_type_char:
            size = 1;
            break;
        default:
            break;
    }
    return size;
}


void generate_vector_uninit(FILE* stream, tac_t* tac) {
    size_t size = data_type_size(tac->res->data_type);
    
    fprintf(stream,
            "\t.globl %s\n"
            "\t.comm %s %ld\n",
            tac->res->value, 
            tac->res->value, 
            atoi(tac->op1->value)*size);
}


void generate_vector_init(FILE* stream, tac_t* tac) {
    fprintf(stream, 
            "\t.globl %s\n"
            "%s:\n",
            tac->res->value, tac->res->value);
}


void generate_vector_init_value(FILE* stream, tac_t* tac) {
    switch(tac->res->data_type) {
        case data_type_int:
            fprintf(stream, 
                    "\t.long %s\n",
                    tac->op1->value);
            break;
        case data_type_char:
            fprintf(stream, 
                    "\t.byte %d\n",
                    tac->op1->value[1]);
            break;
        default:
            break;
    }
}


void generate_begin_function(FILE* stream, tac_t* tac) {
    fprintf(stream, 
            "\t.text\n"
            "\t.globl %s\n"
            "%s:\n"
            "\t.cfi_startproc\n"
            "\tpushq\t%%rbp\n"
            "\t.cfi_def_cfa_offset\t16\n"
	        "\t.cfi_offset\t6, -16\n"
            "\tmovq\t%%rsp, %%rbp\n"
            "\t.cfi_def_cfa_register\t6\n",
            tac->res->value, tac->res->value);
    int stack_parameter_initial_displacement = 16;
    int parameter_size = 8;
    for(int i = list_size(tac->res->parameters) - 1; i >= 0 ; i--) {
        fprintf(stream,
                "\tmovl\t%d(%%rbp), %%eax\n"
                "\tpushq\t%%rax\n",
                i * parameter_size + stack_parameter_initial_displacement);
    }
}


void generate_end_function(FILE* stream, tac_t* tac) {
    fprintf(stream,
            ".%s_end:\n"
            "\tmovq\t%%rbp, %%rsp\n"
            "\tpopq\t%%rbp\n"
            "\t.cfi_def_cfa\t7, 8\n"
            "\tret\n"
            "\t.cfi_endproc\n",
            tac->res->value);
}


void generate_print(FILE* stream, tac_t* tac) {
    switch(tac->res->data_type) {
        case data_type_bool:
            convert_byte_to_int(stream, tac->res->value);
            fprintf(stream,
                    "\tmovl\t%%eax, %%esi\n"
                    "\tleaq\t.intfmt(%%rip), %%rdi\n"
                    "\tmovl\t$0, %%eax\n"
                    "\tcall\tprintf@PLT\n");
            break;
        case data_type_int:
            fprintf(stream,
                    "\tmovl\t%s(%%rip), %%eax\n"
                    "\tmovl\t%%eax, %%esi\n"
                    "\tleaq\t.intfmt(%%rip), %%rdi\n"
                    "\tmovl\t$0, %%eax\n"
                    "\tcall\tprintf@PLT\n",
                    tac->res->value);
            break;
        case data_type_char:
            fprintf(stream,
                    "\tmovzbl\t%s(%%rip), %%eax\n"
                    "\tmovsbl\t%%al, %%eax\n"
                    "\tmovl\t%%eax, %%edi\n"
                    "\tcall\tputchar@PLT\n",
                    tac->res->value);
            break;
        case data_type_float:
            fprintf(stream,
                    "\tmovss\t%s(%%rip), %%xmm0\n"
                    "\tcvtss2sd\t%%xmm0, %%xmm0\n"
                    "\tleaq\t.floatfmt(%%rip), %%rdi\n"
                    "\tmovl\t$1, %%eax\n"
                    "\tcall\tprintf@PLT\n",
                    tac->res->value);
            break;
        case data_type_string:
            fprintf(stream,
                    "\tleaq\t%s(%%rip), %%rdi\n"
                    "\tmovl\t$0, %%eax\n"
                    "\tcall\tprintf@PLT\n",
                    tac->res->value);
            break;
        default:
            break;
    }
}


void generate_argument(FILE* stream, tac_t* tac) {
    switch(tac->res->data_type) {
        case data_type_bool:
        case data_type_int:
            fprintf(stream,
                    "\tmovl\t%s(%%rip), %%eax\n"
                    "\tpushq\t%%rax\n",
                    tac->res->value);
            break;
        case data_type_char:
            fprintf(stream,
                    "\tmovzbl\t%s(%%rip), %%eax\n"
                    "\tmovsbl\t%%al, %%eax\n"
                    "\tpushq\t%%rax\n",
                    tac->res->value);
            break;
        case data_type_float:
            fprintf(stream,
                    "\tmovss\t%s(%%rip), %%xmm0\n"
                    "\tleaq\t-8(%%rsp), %%rsp\n"
                    "\tmovss\t%%xmm0, (%%rsp)\n",
                    tac->res->value);
            break;
        default:
            break;
    }
}


void generate_call(FILE* stream, tac_t* tac) {
    fprintf(stream,
            "\tcall\t%s\n"
            "\tmovl\t%%eax, %s(%%rip)\n",
            tac->op1->value,
            tac->res->value);
}


void generate_parameter(FILE* stream, tac_t* tac) {
    switch(tac->res->data_type) {
        case data_type_bool:
        case data_type_int:
            fprintf(stream,
                    "\tpopq\t%%rax\n"
                    "\tmovl\t%%eax, %s(%%rip)\n",
                    tac->res->value);
            break;
        case data_type_char:
            fprintf(stream,
                    "\tpopq\t%%rax\n"
                    "\tmovb\t%%al, %s(%%rip)\n",
                tac->res->value);
            break;
        case data_type_float:
            fprintf(stream,
                    "\tpopq\t%%rax\n"
                    "\tmovl\t%%eax, %s(%%rip)\n",
                    tac->res->value);
            break;
        default:
            break;
    }
}


void generate_return(FILE* stream, tac_t* tac) {
    fprintf(stream,
            "\tmovl\t%s(%%rip), %%eax\n"
            "\tjmp\t.%s_end\n",
            tac->res->value,
            tac->op1->value);
}


void convert_byte_to_int(FILE* stream, char* variable) {
    fprintf(stream,
            "\tmovzbl\t%s(%%rip), %%eax\n"
            "\tmovsbl\t%%al, %%eax\n",
            variable);
}


void convert_byte_to_float(FILE* stream, char* variable) {
    convert_byte_to_int(stream, variable);
    fprintf(stream,
            "\tcvtsi2ss\t%%eax, %%xmm0\n");
}


void convert_int_to_float(FILE* stream, char* variable) {
    fprintf(stream,
            "\tmovl\t%s(%%rip), %%eax\n"
	        "\tcvtsi2ss\t%%eax, %%xmm0\n",
            variable);
}


void convert_data_type(FILE* stream, char* variable, data_type_t from, data_type_t to) {
    if(from == to) {
        if(from == data_type_char ) {
            fprintf(stream,
                "\tmovzbl\t%s(%%rip), %%eax\n",
                variable);
        } else if(from == data_type_int ) {
            fprintf(stream,
                "\tmovl\t%s(%%rip), %%eax\n",
                variable);
    }
    } else if(from == data_type_int && to == data_type_float) {
        convert_int_to_float(stream, variable);
    } else if(from == data_type_char) {
        if(to == data_type_int) {
            convert_byte_to_int(stream, variable);
        } else if(to == data_type_float) {
            convert_byte_to_float(stream, variable);
        }
    }
}


char* operator_to_mnemonic(tac_type_t operator, data_type_t data_type) {
    switch(data_type) {
        case data_type_char:
        case data_type_int:
            switch(operator) {
                case tac_sum:
                    return "addl";
                    break;
                case tac_sub:
                    return "subl";
                    break;
                case tac_mul:
                    return "imull";
                    break;
                case tac_div:
                    return "idivl";
                    break;
                case tac_eq:
                    return "sete";
                    break;
                case tac_dif:
                    return "setne";
                    break;
                case tac_gt:
                    return "setg";
                    break;
                case tac_ge:
                    return "setge";
                    break;
                case tac_lt:
                    return "setl";
                    break;
                case tac_le:
                    return "setle";
                    break;
                default:
                    return "";
                    break;
            }
            break;
        case data_type_float:
            switch(operator) {
                case tac_sum:
                    return "addss";
                    break;
                case tac_sub:
                    return "subss";
                    break;
                case tac_mul:
                    return "mulss";
                    break;
                case tac_div:
                    return "divss";
                    break;
                case tac_eq:
                    return "cmovne";
                    break;
                case tac_dif:
                    return "cmovne";
                    break;
                case tac_gt:
                    return "seta";
                    break;
                case tac_ge:
                    return "setnb";
                    break;
                case tac_lt:
                    return "seta";
                    break;
                case tac_le:
                    return "setnb";
                    break;
                default:
                    return "";
                    break;
            }
            break;
        default:
            return "";
            break;
    }
}


void generate_basic_arithmetic(FILE* stream, tac_t* tac) {
    convert_data_type(stream, tac->op2->value, tac->op2->data_type, tac->res->data_type);
    fprintf(stream,
            "\tmovl\t%%eax, %%edx\n");
    convert_data_type(stream, tac->op1->value, tac->op1->data_type, tac->res->data_type);
    fprintf(stream,
            "\t%s\t%%edx, %%eax\n"
            "\tmovl\t%%eax, %s(%%rip)\n",
            operator_to_mnemonic(tac->type, data_type_int),
            tac->res->value);
}


void generate_div(FILE* stream, tac_t* tac) {
    convert_data_type(stream, tac->op1->value, tac->op1->data_type, tac->res->data_type);
    convert_data_type(stream, tac->op2->value, tac->op2->data_type, tac->res->data_type);
    fprintf(stream,
            "\tmovl\t%s(%%rip), %%eax\n"
            "\tmovl\t%s(%%rip), %%ecx\n"
            "\tcltd\n"
            "\t%s\t%%ecx\n"
            "\tmovl\t%%eax, %s(%%rip)\n",
            tac->op1->value,
            tac->op2->value,
            operator_to_mnemonic(tac->type, data_type_int),
            tac->res->value);
}


void generate_comparison(FILE* stream, tac_t* tac) {
    convert_data_type(stream, tac->op1->value, tac->op1->data_type, tac->res->data_type);
    convert_data_type(stream, tac->op2->value, tac->op2->data_type, tac->res->data_type);
    
    fprintf(stream,
        "\tmovl\t%s(%%rip), %%edx\n"
        "\tmovl\t%s(%%rip), %%eax\n"
        "\tcmpl\t%%eax, %%edx\n"
        "\t%s\t%%al\n"
        "\tmovb\t%%al, %s(%%rip)\n",
        tac->op1->value,
        tac->op2->value,
        operator_to_mnemonic(tac->type, data_type_int),
        tac->res->value);
}


void generate_move(FILE* stream, tac_t* tac) {
    convert_data_type(stream, tac->op1->value, tac->op1->data_type, tac->res->data_type);
    switch(tac->res->data_type) {
        case data_type_int:
                fprintf(stream,
                        "\tmovl\t%%eax, %s(%%rip)\n",
                        tac->res->value);
                break;
        case data_type_char:
                fprintf(stream,
	                    "\tmovb\t%%al, %s(%%rip)\n",
                        tac->res->value);
            break;
        case data_type_float:
            
            break;
        default:
            break;
    }
    
}


void generate_label(FILE* stream, tac_t* tac) {
    fprintf(stream,
            "%s:\n",
            tac->res->value);
}


void generate_jump_false(FILE* stream, tac_t* tac) {
    fprintf(stream,
            "\tmovzbl\t%s(%%rip), %%eax\n"
            "\ttestb\t%%al, %%al\n"
            "\tje\t%s\n",
            tac->op1->value,
            tac->res->value);
}


void generate_jump(FILE* stream, tac_t* tac) {
    fprintf(stream,
            "\tjmp\t%s\n",
            tac->res->value);
}


void generate_read(FILE* stream, tac_t* tac) {
    fprintf(stream,
	        "\tcall\tgetchar@PLT\n"
	        "\tmovb\t%%al, %s(%%rip)\n",
            tac->res->value);
}


void generate_vector_index(FILE* stream, tac_t* tac) {
    switch(tac->res->data_type) {
        case data_type_int:
                fprintf(stream,
                        "\tmovl\t%s(%%rip), %%eax\n"
                        "\tcltq\n"
                        "\tleaq\t0(,%%rax,4), %%rdx\n"
                        "\tleaq\t%s(%%rip), %%rax\n"
                        "\tmovl\t(%%rdx,%%rax), %%eax\n"
                        "\tmovl\t%%eax, %s(%%rip)\n",
                        tac->op2->value,
                        tac->op1->value,
                        tac->res->value);
                break;
        case data_type_char:
                fprintf(stream,
	                    "\tmovl\t%s(%%rip), %%eax\n"
                        "\tmovslq\t%%eax, %%rdx\n"
                        "\tleaq\t%s(%%rip), %%rax\n"
                        "\tmovzbl\t(%%rdx,%%rax), %%eax\n"
                        "\tmovb\t%%al, %s(%%rip)\n",
                        tac->op2->value,
                        tac->op1->value,
                        tac->res->value);
            break;
        case data_type_float:
            
            break;
        default:
            break;
    }
}


void generate_vector_move(FILE* stream, tac_t* tac) {
    switch(tac->res->data_type) {
        case data_type_int:
                fprintf(stream,
                        "\tmovl\t%s(%%rip), %%eax\n"
                        "\tmovl\t%s(%%rip), %%edx\n"
                        "\tcltq\n"
                        "\tleaq\t0(,%%rax,4), %%rcx\n"
                        "\tleaq\t%s(%%rip), %%rax\n"
                        "\tmovl\t%%edx, (%%rcx,%%rax)\n",
                        tac->op1->value,
                        tac->op2->value,
                        tac->res->value);
                break;
        case data_type_char:
                fprintf(stream,
                        "\tmovl\t%s(%%rip), %%eax\n"
                        "\tmovzbl\t%s(%%rip), %%ecx\n"
                        "\tmovslq\t%%eax, %%rdx\n"
                        "\tleaq\t%s(%%rip), %%rax\n"
                        "\tmovb\t%%cl, (%%rdx,%%rax)\n",
                        tac->op1->value,
                        tac->op2->value,
                        tac->res->value);
            break;
        case data_type_float:
            
            break;
        default:
            break;
    }
}