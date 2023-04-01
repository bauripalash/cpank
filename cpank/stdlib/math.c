#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"
#include "../include/vm.h"

#define _USE_MATH_DEFINES

Value _math_pow(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm, L"math pow function only takes 2 arguments!");
    }

    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm, L"math pow function only works on numbers");
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = pow(a, b);
    return make_num(raw_result);
}

Value _math_add(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm, L"math add function only takes 2 arguments!");
    }

    //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm, L"math add function only works on numbers");
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = a + b;
    return make_num(raw_result);
}

double _get_gcd(double a, double b) {
    double x = (a > 0) ? a : -a;
    double y = (b > 0) ? b : -b;

    while (x != y) {
        if (x > y) {
            x -= y;
        } else {
            y -= x;
        }
    }

    return x;
}

Value _math_gcd(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        // return make_nil();

        return make_error(vm, L"math gcd function only takes two arguments!");
    }

    //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm, L"math add function only works on numbers");
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(_get_gcd(a, b));
}

Value _math_lcm(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm, L"math lcm function only takes 2 arguments!");
    }
    if (!is_num(args[0]) && !is_num(args[1])) {
        //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_error(vm, L"math lcm function only works on numbers");
    }
    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(a * b / _get_gcd(a, b));
}

Value _math_sqrt(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math sqrt function only takes 1 arguments!");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math sqrt function only works on numbers");
    }

    double a = get_as_number(args[0]);
    return make_num(sqrt(a));
}

Value _math_log10(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math log 10 function only takes 1 arguments!");
    }

    // if (args[0].type != V_NUM) {

    if (!is_num(args[0])) {
        return make_error(vm, L"math log10 function only works on numbers");
    }
    double a = get_as_number(args[0]);

    return make_num(log10(a));
}
// Log base e
Value _math_log_e(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math log e function only takes 1 arguments!");
    }

    // if (args[0].type != V_NUM) {
    //
    if (!is_num(args[0])) {
        return make_error(vm, L"math log e function only works on numbers");
    }
    double a = get_as_number(args[0]);

    return make_num(log(a));
}

// Log base X
Value _math_logx(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm, L"math logx function only takes 2 arguments!");
    }

    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm, L"math logx function only works on numbers");
    }

    double base = get_as_number(args[0]);
    double num = get_as_number(args[1]);

    return make_num(log(num) / log(base));
}

Value _math_sine(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math sin function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math sin function only works on numbers");
    }

    double n = get_as_number(args[0]);

    return make_num(sin(n));
}

Value _math_cosine(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math cos function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math cos function only works on numbers");
    }

    double n = get_as_number(args[0]);

    return make_num(cos(n));
}

Value _math_tangent(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math tan function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math tan function only works on numbers");
    }

    double n = get_as_number(args[0]);

    return make_num(tan(n));
}

Value _math_to_degree(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math degree function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math degree function only works on numbers");
    }

    double n = get_as_number(args[0]);

    return make_num(n * (180 / CONST_PI));
}

Value _math_to_radians(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math rad function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math rad function only works on numbers");
    }

    double n = get_as_number(args[0]);

    return make_num(n * (CONST_PI / 180));
}

Value _math_get_pi(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, L"math pi() function takes no arguments");
    }

    return make_num(CONST_PI);
}

Value _math_get_e(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, L"math e() function takes no arguments");
    }

    return make_num(CONST_E);
}

Value _math_str_to_num(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(
            vm, L"math strtonum(...) function takes only 1 arguments");
    }

    if (!is_str_obj(args[0])) {
        return make_error(vm,
                          L"math strtonum(...) function only works on strings");
    }

    ObjString* ostr = get_as_string(args[0]);
    double result = 0;
    swscanf(ostr->chars, L"%lf", &result);
    return make_num(result);
}

Value _math_get_random(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, L"math randnum() function takes no arguments");
    }
    srand((unsigned int)time(0));
    double result = ((double)rand() / (double)(RAND_MAX));

    return make_num(result);
}

Value _math_get_random_from_range(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm, L"math random() function takes 2 arguments");
    }
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm,
                          L"math random(...) function only works on numbers");
    }
    srand((unsigned int)time(0));
    double min = get_as_number(args[0]);
    double max = get_as_number(args[1]);
    double range = (max - min);
    double div = RAND_MAX / range;
    double result = min + (rand() / div);

    return make_num(result);
}

Value _math_abs_num(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"math abs(...) function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math abs(...) function only works on numbers");
    }

    double v = get_as_number(args[0]);

    return make_num(fabs(v));
}

Value _math_round(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm,
                          L"math round(...) function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm,
                          L"math round(...) function only works on numbers");
    }

    double v = get_as_number(args[0]);

    return make_num(round(v));
}

Value _math_get_infinity(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, L"math infinity() function takes no arguments");
    }

    return make_num(INFINITY);
}

Value _math_ceil(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm,
                          L"math ceil(...) function takes only 1 arguments");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"math ceil(...) function only works on numbers");
    }

    double v = get_as_number(args[0]);
    return make_num(ceil(v));
}

void push_stdlib_math(PankVm* vm) {
    SL sls[] = {
        msl(L"pow", _math_pow),
        msl(L"add", _math_add),
        msl(L"gcd", _math_gcd),
        msl(L"lcm", _math_lcm),
        msl(L"sqrt", _math_sqrt),
        msl(L"log10", _math_log10),
        msl(L"loge", _math_log_e),
        msl(L"logx", _math_logx),
        msl(L"sin", _math_sine),
        msl(L"cos", _math_cosine),
        msl(L"tan", _math_tangent),
        msl(L"degree", _math_to_degree),
        msl(L"rad", _math_to_radians),
        msl(L"pi", _math_get_pi),
        msl(L"e", _math_get_e),
        msl(L"strtonum", _math_str_to_num),
        msl(L"randnum", _math_get_random),
        msl(L"random", _math_get_random_from_range),
        msl(L"abs", _math_abs_num),
        msl(L"round", _math_round),
        msl(L"infinity", _math_get_infinity),
        msl(L"ceil", _math_ceil),
    };

    _push_stdlib(vm, L"math", sls, 22);
}
