# Lambda 演算解释器 —— Python 实现
#
# 使用方式：
#   python z.py

def Closure(x, body, env):
    return [x, body, env]

def closure_x(closure):
    match closure:
        case [x, body, env]:
            return x
    return None

def closure_body(closure):
    match closure:
        case [x, body, env]:
            return body
    return None

def closure_env(closure):
    match closure:
        case [x, body, env]:
            return env
    return None

# extend_env :: var -> val -> env -> env
# 函数式语义：创建新节点，不修改原 env
def extend_env(var, val, env):
    return [[var, val]] + env

# lookup :: str -> env -> value
# 迭代实现（避免递归栈溢出）
def lookup(exp, env):
    for var, val in env:
        if var == exp:
            return val
    raise NameError(f"unknown symbol: {exp}")

# pretty_print :: exp -> None
# 以 S-表达式格式打印 AST
def pretty_print(exp, indent=0):
    prefix = " " * indent
    match exp:
        case int():
            print(f"{prefix}{exp}")
        case str():
            print(f"{prefix}{exp}")
        case ["lambda", [x], body]:
            print(f"{prefix}(lambda ({x})")
            pretty_print(body, indent + 2)
            print(f"{prefix})")
        case [op, e1, e2] if op in ("+", "-", "*", "/"):
            print(f"{prefix}({op}")
            pretty_print(e1, indent + 2)
            pretty_print(e2, indent + 2)
            print(f"{prefix})")
        case [fun, arg]:
            print(f"{prefix}(")
            pretty_print(fun, indent + 2)
            pretty_print(arg, indent + 2)
            print(f"{prefix})")
        case _:
            print(f"{prefix}<unknown: {exp!r}>")

# interpreter :: exp -> env -> value
def interpreter(exp, env):
    match exp:
        case int():
            return exp
        case str():
            return lookup(exp, env)
        case ["lambda", [x], body]:
            return Closure(x, body, env)
        case [op, e1, e2]:
            v1 = interpreter(e1, env)
            v2 = interpreter(e2, env)
            match op:
                case '+': return v1 + v2
                case '-': return v1 - v2
                case '*': return v1 * v2
                case '/': return v1 // v2   # 整数除法，与 C 一致
        case [fun, e]:
            fun = interpreter(fun, env)
            arg = closure_x(fun)
            body = closure_body(fun)
            captured_env = closure_env(fun)
            new_env = extend_env(arg, interpreter(e, env), captured_env)
            return interpreter(body, new_env)
        case _:
            raise ValueError(f"unknown expression: {exp!r}")

# ── 测试 ──
def test(description, actual, expected):
    ok = "✓" if actual == expected else "✗"
    print(f"  {description:40s} {ok}  got {actual!r}, expected {expected!r}")

if __name__ == "__main__":
    print("── 测试 ──\n")

    # 1. 数值
    r = interpreter(42, [])
    test("数值字面量", r, 42)

    # 2. 二元运算
    r = interpreter(['+', 1, 2], [])
    test("加法 (+ 1 2)", r, 3)

    r = interpreter(['-', 5, 3], [])
    test("减法 (- 5 3)", r, 2)

    r = interpreter(['*', 3, 4], [])
    test("乘法 (* 3 4)", r, 12)

    r = interpreter(['/', 10, 3], [])
    test("除法 (/ 10 3) — 整数截断", r, 3)

    # 3. Lambda 应用
    r = interpreter([["lambda", ["x"], ["+", "x", 1]], 2], [])
    test("((λx. + x 1) 2)", r, 3)

    r = interpreter([["lambda", ["x"], ["+", "x", 1]], 3], [])
    test("((λx. + x 1) 3)", r, 4)

    r = interpreter([["lambda", ["x"], ["*", "x", "x"]], 7], [])
    test("((λx. * x x) 7)", r, 49)

    # 4. 闭包捕获
    r = interpreter([[["lambda", ["x"], ["lambda", ["y"], ["+", "x", "y"]]], 1], 2], [])
    test("((λx. λy. + x y) 1 2)", r, 3)

    r = interpreter([[["lambda", ["x"], ["lambda", ["y"], ["+", "x", "y"]]], 5], 6], [])
    test("((λx. λy. + x y) 5 6)", r, 11)

    # 5. 恒等函数
    r = interpreter([["lambda", ["x"], "x"], 99], [])
    test("((λx. x) 99)", r, 99)

    # 6. Closure 类型检查
    r = interpreter(["lambda", ["x"], "x"], [])
    test("lambda 返回 closure", r, ['x', 'x', []])

    # 7. 错误：未定义变量
    try:
        interpreter("x", [])
        test("未定义变量应抛异常", "no error", "NameError")
    except NameError as e:
        test("未定义变量应抛异常", "NameError", "NameError")

    print(f"\n── pretty_print 示例 ──")
    print()
    exp = [["lambda", ["x"], ["lambda", ["y"], ["+", "x", "y"]]], 1]
    print("AST:")
    pretty_print(exp)
    print()
    val = interpreter([exp, 2], [])
    print(f"结果: {val}")
    print()
