#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"
#include "../include/vm.h"

#define _USE_MATH_DEFINES

Value _bn_math_pow(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm,
                          L"গণিতের ঘাত(ক , খ) কাজটি মাত্র দুটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm, L"গণিতের ঘাত(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = pow(a, b);
    return make_num(raw_result);
}

Value _bn_math_add(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm,
                          L"গণিতের যোগ(ক , খ) কাজটি মাত্র দুটি মান গ্রহণ করে!");
    }

    //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm, L"গণিতের ঘাত(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);
    double raw_result = a + b;
    return make_num(raw_result);
}

static double _get_gcd(double a, double b) {
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

Value _bn_math_gcd(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        // return make_nil();

        return make_error(vm,
                          L"গণিতের গসাগু(ক , খ) কাজটি মাত্র দুটি মান গ্রহণ করে!");
    }

    //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm,
                          L"গণিতের গসাগু(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(_get_gcd(a, b));
}

Value _bn_math_lcm(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm,
                          L"গণিতের লসাগু(ক , খ) কাজটি মাত্র দুটি মান গ্রহণ করে!");
    }
    if (!is_num(args[0]) && !is_num(args[1])) {
        //    if (args[0].type != args[1].type && args[0].type != V_NUM) {
        return make_error(vm,
                          L"গণিতের লসাগু(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }
    double a = get_as_number(args[0]);
    double b = get_as_number(args[1]);

    return make_num(a * b / _get_gcd(a, b));
}

Value _bn_math_sqrt(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের বর্গমূল(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের বর্গমূল(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double a = get_as_number(args[0]);
    return make_num(sqrt(a));
}

Value _bn_math_log10(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের লগদশ(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    // if (args[0].type != V_NUM) {

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের লগদশ(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }
    double a = get_as_number(args[0]);

    return make_num(log10(a));
}
// Log base e
Value _bn_math_log_e(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের লগ(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    // if (args[0].type != V_NUM) {
    //
    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের লগ(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }
    double a = get_as_number(args[0]);

    return make_num(log(a));
}

// Log base X
Value _bn_math_logx(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm,
                          L"গণিতের লগবেস(ক , খ) কাজটি মাত্র দুটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(vm,
                          L"গণিতের লগবেস(ক , খ) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double base = get_as_number(args[0]);
    double num = get_as_number(args[1]);

    return make_num(log(num) / log(base));
}

Value _bn_math_sine(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের সাইন(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের সাইন(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double n = get_as_number(args[0]);

    return make_num(sin(n));
}

Value _bn_math_cosine(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের কস(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের কস(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double n = get_as_number(args[0]);

    return make_num(cos(n));
}

Value _bn_math_tangent(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের ট্যান(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের ট্যান(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double n = get_as_number(args[0]);

    return make_num(tan(n));
}

Value _bn_math_to_degree(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের ডিগ্রি(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের ডিগ্রি(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double n = get_as_number(args[0]);

    return make_num(n * (180 / CONST_PI));
}

Value _bn_math_to_radians(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm,
                          L"গণিতের রেডিয়ান(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের রেডিয়ান(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double n = get_as_number(args[0]);

    return make_num(n * (CONST_PI / 180));
}

Value _bn_math_get_pi(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, L"গণিতের পাই() কাজটি কোনো মান গ্রহণ করেনা!");
    }

    return make_num(CONST_PI);
}

Value _bn_math_get_e(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, L"গণিতের ই()) কাজটি কোনো মান গ্রহণ করেনা!");
    }

    return make_num(CONST_E);
}

Value _bn_math_str_to_num(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের সংখ্যা(ক) কাজটি মাত্র একটি মান গ্রহণ করে!");
    }

    if (!is_str_obj(args[0])) {
        return make_error(
            vm, L"গণিতের সংখ্যা(ক) কাজটি শুধুমাত্র স্ট্রিং/নাম মান গ্রহণ করে");
    }

    ObjString* ostr = get_as_string(args[0]);
    double result = 0;
    swscanf(ostr->chars, L"%lf", &result);
    return make_num(result);
}

Value _bn_math_get_random(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(
            vm, L"গণিতের এলোমেলো_সংখ্যা(ক) কাজটি একটি মাত্র মান গ্রহণ করে ");
    }
    if (!is_num(args[0])) {
        return make_error(
            vm, L"গণিতের এলোমেলো_সংখ্যা(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }
    srand((unsigned int)time(0));
    double result = ((double)rand() / (double)(RAND_MAX));

    return make_num(result);
}

Value _bn_math_get_random_from_range(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(
            vm, L"গণিতের এলোমেলো(নিম্ন, উচ্চ) কাজটি মাত্র দুটি মান গ্রহণ করে ");
    }
    if (!is_num(args[0]) && !is_num(args[1])) {
        return make_error(
            vm, L"গণিতের এলোমেলো(নিম্ন, উচ্চ) কাজটি শুধুমাত্র সংখ্যা মান গ্রহণ করে");
    }
    srand((unsigned int)time(0));
    double min = get_as_number(args[0]);
    double max = get_as_number(args[1]);
    double range = (max - min);
    double div = RAND_MAX / range;
    double result = min + (rand() / div);

    return make_num(result);
}

Value _bn_math_abs_num(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের পরম(ক) কাজটি একটি মাত্র মান গ্রহণ করে");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের পরম(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double v = get_as_number(args[0]);

    return make_num(fabs(v));
}

Value _bn_math_round(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের রাউন্ড(ক) কাজটি একটি মাত্র মান গ্রহণ করে");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের রাউন্ড(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double v = get_as_number(args[0]);

    return make_num(round(v));
}

Value _bn_math_get_infinity(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, L"গণিতের অসীম() কাজটি কোনো মান গ্রহণ করে না");
    }

    return make_num(INFINITY);
}

Value _bn_math_ceil(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, L"গণিতের সিল(ক) কাজটি একটি মাত্র মান গ্রহণ করে");
    }

    if (!is_num(args[0])) {
        return make_error(vm, L"গণিতের সিল(ক) কাজটি শুধু সংখ্যা মান গ্রহণ করে");
    }

    double v = get_as_number(args[0]);
    return make_num(ceil(v));
}

void push_stdlib_math_bn(PankVm* vm) {
    SL sls[] = {
        msl(L"ঘাত", _bn_math_pow),
        msl(L"যোগ", _bn_math_add),
        msl(L"গসাগু", _bn_math_gcd),
        msl(L"লসাগু", _bn_math_lcm),
        msl(L"বর্গমূল", _bn_math_sqrt),
        msl(L"লগদশ", _bn_math_log10),
        msl(L"লগ", _bn_math_log_e),
        msl(L"লগবেস", _bn_math_logx),
        msl(L"সাইন", _bn_math_sine),
        msl(L"কস", _bn_math_cosine),
        msl(L"ট্যান", _bn_math_tangent),
        msl(L"ডিগ্রি", _bn_math_to_degree),
        msl(L"রেডিয়ান", _bn_math_to_radians),
        msl(L"পাই", _bn_math_get_pi),
        msl(L"ই", _bn_math_get_e),
        msl(L"সংখ্যা", _bn_math_str_to_num),
        msl(L"এলোমেলো_সংখ্যা", _bn_math_get_random),
        msl(L"এলোমেলো", _bn_math_get_random_from_range),
        msl(L"পরম", _bn_math_abs_num),
        msl(L"রাউন্ড", _bn_math_round),
        msl(L"অসীম", _bn_math_get_infinity),
        msl(L"সিল", _bn_math_ceil),
    };

    _push_stdlib(vm, L"গণিত", sls, 22);
}
