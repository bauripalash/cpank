#include <math.h>
#include <wchar.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"
#include "../include/vm.h"

Value _math_pow(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = pow(a, b);
    return make_num(raw_result);
}

Value _math_add(int argc, Value* args) {
    if (argc != 2) {
        // ObjErr * err = new_err_obj(L"math obj error");
        // print_obj()
        // Value verr = make_obj_val(err);
        // print_val(verr);
        // return verr;
        return make_error(L"math add function only takes two arguments!");
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
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

Value _math_gcd(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(_get_gcd(a, b));
}

Value _math_lcm(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }
    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(a * b / _get_gcd(a, b));
}

Value _math_sqrt(int argc, Value* args) {
    if (argc != 1) {
        return make_nil();
    }

    if (args[0].type != V_NUM) {
        return make_nil();
    }

    double a = get_as_number(args[0]);
    return make_num(sqrt(a));
}

Value _math_log10(int argc, Value* args) {
    if (argc != 1) {
        return make_nil();
    }

    if (args[0].type != V_NUM) {
        return make_nil();
    }
    double a = get_as_number(args[0]);

    return make_num(log10(a));
}
// Log base e
Value _math_log_e(int argc, Value* args) {
    if (argc != 1) {
        return make_nil();
    }

    if (args[0].type != V_NUM) {
        return make_nil();
    }
    double a = get_as_number(args[0]);

    return make_num(log(a));
}

// Log base X
Value _math_logx(int argc, Value* args) {
    if (argc != 2) {
        return make_nil();
    }

    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_nil();
    }

    double base = get_as_number(args[0]);
    double num = get_as_number(args[1]);

    return make_num(log(num) / log(base));
}

void push_stdlib_math() {
    SL sls[] = {
        msl(L"pow", _math_pow),    msl(L"add", _math_add),
        msl(L"gcd", _math_gcd),    msl(L"lcm", _math_lcm),
        msl(L"sqrt", _math_sqrt),  msl(L"log10", _math_log10),
        msl(L"loge", _math_log_e), msl(L"logx", _math_logx),
    };

    _push_stdlib(L"math", sls, 8);
}
