# RUN: %chocopy-llvm %s -ast-dump | diff %s.ast -

x:int = 0
class x(object): 
    a:int = 0
for x in [1, 2, 3]:
    x = x + 1
