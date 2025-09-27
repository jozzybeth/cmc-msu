def find_modified_max_argmax(L, f):
    L = [f(i) for i in L if type(i) == int]
    return (m := max(L), L.index(m)) if L else ()
