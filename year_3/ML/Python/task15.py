from typing import List


def hello(name = None) -> str:
    if name == None:
        return "Hello!"
    elif name == "":
        return "Hello!"
    strok = "Hello, " + name + "!"
    return strok
    
def int_to_roman(num: int) -> str:
    result = ""
    thous = num // 1000
    for i in range (thous):
        result += "M"
    num  -= thous*1000
    hun = num // 100
    if (hun == 4):
        result += "CD"
    elif (hun == 9):
        result += "CM"
    elif (hun >= 1 and hun <=3):
        for i in range (hun):
            result += "C"
    elif (hun >= 5 and hun <=8):
        result += "D"
        hund = hun - 5
        for i in range (hund):
            result += "C"
    else:
        pass
    num -= hun*100
    tens = num // 10
    if (tens == 4):
        result += "XL"
    elif (tens == 9):
        result += "XC"
    elif (tens >= 1 and tens <=3):
        for i in range (tens):
            result += "X"
    elif (tens >= 5 and tens <=8):
        result += "L"
        ten = tens - 5
        for i in range (ten):
            result += "X"
    else:
        pass
    num -= tens*10
    if (num == 4):
        result += "IV"
    elif (num == 9):
        result += "IX"
    elif (num >= 1 and num <=3):
        for i in range (num):
            result += "I"
    elif (num >= 5 and num <=8):
        result += "V"
        num -=5
        for i in range (num):
            result += "I"
    else:
        pass
    return result


def longest_common_prefix(strs_input: List[str]) -> str:
    if len(strs_input) == 0:
        return ""
    strok = strs_input
    for i in range (len(strok)):
        strok[i] = strok[i].strip()
    prefix = strok[0]
    size = len(prefix)
    for i in range (size):
        result = True
        for j in range (len(strok) - 1):
            if (strok[j + 1].find(prefix) != 0):
                result = False
                break
        if result:
            break
        prefix = prefix[:-1]
    return prefix

def primes() -> int:
    yield 2
    yield 3
    result = 3
    while True:
        result += 1
        check = True
        for i in range (2,int(result**0.5) + 1):
            if result % i == 0:
                check = False
                break
        if check:
            yield result

            


class BankCard:
    def __init__(self, total_sum: int, balance_limit = -1):
        self.total_sum = total_sum
        self.balance_limit = balance_limit
        
    def __call__(self, sum_spent: int):
        if sum_spent > self.total_sum:
            print(f"Not enough money to spend {sum_spent} dollars.")
            raise ValueError()
        else:
            self.total_sum -= sum_spent
            print(f"You spent {sum_spent} dollars.")
    
    def __str__(self):
        return "To learn the balance call balance."
    
    @property
    def balance(self):
        if self.balance_limit == -1:
            return self.total_sum
        elif self.balance_limit == 0:
            print("Balance check limits exceeded.")
            raise ValueError()
        else:
            self.balance_limit -= 1
            return self.total_sum
        
    def put(self, sum_put: int):
         self.total_sum += sum_put
         print(f"You put {sum_put} dollars.")

    def __add__(self, other):
        new_balance = self.total_sum + other.total_sum
        new_balance_limit = 0
        if self.balance_limit == -1 or other.balance_limit == -1:
            new_balance_limit = -1
        elif self.balance_limit > other.balance_limit:
            new_balance_limit = self.balance_limit
        else:
            new_balance_limit = other.balance_limit
        new = BankCard(new_balance, new_balance_limit)
        return new




