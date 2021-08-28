#include "compile.h"
#include "parser.h"
#include "dict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void emit(Emitter* em, Element elem) {
    em->elems[em->pos++] = elem;
}

static void compile_if(Emitter* em) {
    emit(em, ename_element("exch"));
    emit(em, number_element(4));
    emit(em, primitive_element(OP_JMP_NOT_IF));
    emit(em, primitive_element(OP_EXEC));
    emit(em, number_element(1));
    emit(em, primitive_element(OP_JMP));
    emit(em, ename_element("pop"));
}

static void compile_ifelse(Emitter* em) {
    emit(em, number_element(3));
    emit(em, number_element(2));
    emit(em, ename_element("roll"));
    emit(em, number_element(5));
    emit(em, primitive_element(OP_JMP_NOT_IF));
    emit(em, ename_element("pop"));
    emit(em, primitive_element(OP_EXEC));
    emit(em, number_element(4));
    emit(em, primitive_element(OP_JMP));
    emit(em, ename_element("exch"));
    emit(em, ename_element("pop"));
    emit(em, primitive_element(OP_EXEC));
}

static void compile_while(Emitter* em) {
    emit(em, primitive_element(OP_STORE));
    emit(em, primitive_element(OP_STORE));
    emit(em, number_element(0));
    emit(em, primitive_element(OP_LOAD));
    emit(em, primitive_element(OP_EXEC));
    emit(em, number_element(6));
    emit(em, primitive_element(OP_JMP_NOT_IF));
    emit(em, number_element(1));
    emit(em, primitive_element(OP_LOAD));
    emit(em, primitive_element(OP_EXEC));
    emit(em, number_element(-9));
    emit(em, primitive_element(OP_JMP));
}

void register_compile_primitives() {
    compile_dict_put("ifelse", &(Element){ ET_COMPILE_FUNC, { .compfunc = compile_ifelse } });
    compile_dict_put("if", &(Element){ ET_COMPILE_FUNC, { .compfunc = compile_if } });
    compile_dict_put("while", &(Element){ ET_COMPILE_FUNC, { .compfunc = compile_while } });
    compile_dict_put("exec", &(Element){ ET_PRIMITIVE, { .op = OP_EXEC } });
    compile_dict_put("jmp", &(Element){ ET_PRIMITIVE, { .op = OP_JMP } });
    compile_dict_put("jmp_not_if", &(Element){ ET_PRIMITIVE, { .op = OP_JMP_NOT_IF } });
}

int compile_exec_array(int prev_ch, Element* out_element) {
    int ch = prev_ch;
    Token token = {
        LT_UNKNOWN,
        {0}
    };

    int i = 0;
    Emitter em;
    em.pos = 0;

    do {
        ch = parse_one(ch, &token);

        if (token.ltype != LT_UNKNOWN) {
            switch (token.ltype) {
            case LT_NUMBER:
                emit(&em, number_element(token.u.number));
                break;
            case LT_EXECUTABLE_NAME: {
                Element exec_name;
                if (compile_dict_get(token.u.name, &exec_name)) {
                    switch (exec_name.type) {
                    case ET_COMPILE_FUNC:
                        exec_name.u.compfunc(&em);
                        break;
                    case ET_PRIMITIVE:
                        emit(&em, exec_name);
                        break;
                    default:
                        printf("expect resolved func %s to be compile func or primitive, but got %d\n", token.u.name, exec_name.type);
                        exit(1);
                    }
                }
                else {
                    emit(&em, ename_element(token.u.name));
                }
                break;
            }
            case LT_LITERAL_NAME:
                emit(&em, lname_element(token.u.name));
                break;
            case LT_OPEN_CURLY: {
                Element e;
                ch = compile_exec_array(ch, &e);
                emit(&em, e);
                break;
            }
            case LT_CLOSE_CURLY:
            case LT_SPACE:
                break;
            default:
                break;
            }
        }
    } while (token.ltype != LT_END_OF_FILE && token.ltype != LT_CLOSE_CURLY);

    ElementArray* array = malloc(sizeof(ElementArray) + sizeof(Element) * em.pos);
    array->len = em.pos;
    memcpy(array->elements, em.elems, sizeof(Element) * em.pos);
    *out_element = earray_element(array);

    return ch;
}