#include <math.h>
#include <wchar.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"
#include "../include/vm.h"

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

    // if (args[0].type != args[1].type && args[0].type != V_NUM) {

    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm, L"math logx function only works on numbers");
    }

    double base = get_as_number(args[0]);
    double num = get_as_number(args[1]);

    return make_num(log(num) / log(base));
}

void push_stdlib_math(PankVm* vm) {
    SL sls[] = {
        msl(L"pow", _math_pow),    msl(L"add", _math_add),
        msl(L"gcd", _math_gcd),    msl(L"lcm", _math_lcm),
        msl(L"sqrt", _math_sqrt),  msl(L"log10", _math_log10),
        msl(L"loge", _math_log_e), msl(L"logx", _math_logx),
    };

    _push_stdlib(vm, L"math", sls, 8);
}
