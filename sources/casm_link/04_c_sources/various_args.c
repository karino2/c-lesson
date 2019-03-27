#include <stdio.h>

double double_add(double a, int b) {
    return a+b;
}

struct ManyField {
    int a1;
    int a2;
    int a3;
    int a4;
    int a5;
    int a6;
    int a7;
};

int struct_arg(struct ManyField arg) {
    return arg.a1+arg.a2+arg.a3+arg.a4+arg.a5+arg.a6+arg.a7;
}

int struct_pointer_arg(struct ManyField* arg) {
    return arg->a1+arg->a2+arg->a3+arg->a4+arg->a5+arg->a6+arg->a7;
}

/*
For easier assembly reading, make separate function for main side.
*/

void main_double_add() {
    double res = double_add(0.5, 3);
    printf("result is %f\n", res);
}

void main_struct_body() {
    struct ManyField st;

    st.a1 = 7;
    st.a2 = 6;
    st.a3 = 5;
    st.a4 = 4;
    st.a5 = 3;
    st.a6 = 2;
    st.a7 = 1;

    int res = struct_arg(st);
    printf("starg res is %d\n", res);

}

void main_struct_pointer() {
    struct ManyField st;

    st.a1 = 7;
    st.a2 = 6;
    st.a3 = 5;
    st.a4 = 4;
    st.a5 = 3;
    st.a6 = 2;
    st.a7 = 1;

    int res = struct_pointer_arg(&st);
    printf("starg res is %d\n", res);
}


int main() {
    main_double_add();
    main_struct_body();
    main_struct_pointer();
    return 0;
}

