# ["lambda", ["x"], [+ "x" 1]]
# [fun, exp]
# [+, x, 1]
# 1 | 2 ..

def Closure(x, body, env):
    return [x, body, env]

def closure_x (closure):
    match closure:
       case [x, body, env]:
           return x
    return None

def closure_body (closure):
    match closure:
        case [x, body, env]:
            return body
    return None

def closure_env (closure):
    match closure:
        case[x, body, env]:
            return env
    return None


def extend_env (var, val, env):
    new_env = [[var, val]] + env
    return new_env

def lookup (exp, env):
    if len(env) == 0:
        return None
    if env[0][0] == exp:
        return env[0][1]
    else:
        return lookup (exp, env[1:])
    
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
                case '+':
                    return v1 + v2
                case '-':
                    return v1 - v2
                case '*':
                    return v1 * v2
                case '/':
                    return v1 / v2
        case [fun, e]:
            fun = interpreter(fun, env)
            arg = closure_x (fun)
            body = closure_body (fun)
            env = closure_env (fun)
            new_env = extend_env (arg, interpreter (e, env), env)
            return interpreter (body, new_env)
            
r = interpreter([["lambda", ["x"], ["+", "x", 1]], 2], [])
r1 = interpreter([[["lambda", ["x"], ["lambda", ["y"], ["+", "x", "y"]]], 1], 2], [])
print (r)
                    
