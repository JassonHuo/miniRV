import re
def to_two(num: int, length: int = 5) -> str:
    is_nega = 0
    li = []
    if num < 0:
        is_nega = 1
        num = -num
    while num != 0:
        li.append(num % 2)
        num //= 2
    if is_nega:
        for i in range(len(li)):
            li[i] = 1 - li[i]
        cin = 0
        li[0] += 1
        if li[0] == 2:
            cin = 1
            li[0] = 0
        for i in range(1, len(li)):
            li[i] += cin
            if li[i] == 2:
                cin = 1
                li[i] = 0
            else:
                cin = 0
    s = ''
    while li:
        s += str(li.pop())
    s = (length - len(s)) * str(is_nega) + s
    return s

def to_o0(num: str) -> str:
    enco = {10:'a', 11:'b', 12:'c', 13:'d', 14:'e', 15:'f'}
    ans = ""
    for i in range(0, len(num) - 3, 4):
        s = num[i:i + 4]
        by = 8 * int(s[0]) + 4 * int(s[1]) + 2 * int(s[2]) + 1 * int(s[3])
        if by in enco:
            by = enco[by]
        else:
            by = str(by)
        ans += by
    return ans

opcodes = {
    'lui': '0110111',
    'jalr': ['1100111', '000'],
    'lw': ['0000011', '010'],
    'lbu': ['0000011', '100'],
    'sb': ['0100011', '000'],
    'sw': ['0100011', '010'],
    'addi': ['0010011', '000'],
    'add': ['0110011', '000']
}

# def ext_regi(rs: str, length:int = 5, nega = 0) -> str:
#     return (length - len(rs)) * str(nega) + rs

while True:
    code = input().split()
    out = ''
    if code[0] == 'q':
        break
    op = code[0]
    #is_nega = 0
    if op == 'lui' or op == 'LUI':
        rd, imm = map(int, re.split("[,，]", code[1]))
        if imm > 524287 or rd > 31:
            print("warning：超过最大上限")
            continue
        elif imm < -524288 or rd < 0:
            print("warning：超过最低下限")
            continue
        #if imm < 0: is_nega = 1
        rd, imm = map(str, (to_two(rd), to_two(imm, 20)))
        # imm = ext_regi(imm, 20)
        # rd = ext_regi(rd)
        out = imm + rd + opcodes['lui']
        print(out)
    elif op == 'addi' or op == 'ADDI':
        rd, rs1, imm = map(int, re.split("[,，]", code[1]))
        if imm > 2047 or rd > 31 or rs1 > 31:
            print("warning：超过最大上限")
            continue
        elif imm < -2048 or rd < 0 or rs1 < 0:
            print("warning：超过最低下限")
            continue
        rd, rs1, imm = map(str, (to_two(rd), to_two(rs1), to_two(imm, 12)))
        #if imm < 0: is_nega = 1
        # imm = ext_regi(imm, 12)
        # rs1 = ext_regi(rs1)
        # rd = ext_regi(rd)
        out = imm + rs1 + opcodes['addi'][1] + rd + opcodes['addi'][0]
        print(out)
    elif op == 'add' or op == 'ADD':
        rd, rs1, rs2 = map(int, re.split("[,，]", code[1]))
        if rs1 > 31 or rs2 > 31 or rd > 31:
            print("warning：超过最大上限")
            continue
        elif rs1 < 0 or rs2 < 0 or rd < 0:
            print("warning：超过最低下限")
            continue
        rd, rs1, rs2 = map(str, (to_two(rd), to_two(rs1), to_two(rs2)))
        # rd = ext_regi(rd)
        # rs2 = ext_regi(rs2)
        # rs1 = ext_regi(rs1)
        out = "0"*7 + rs2 + rs1 + opcodes['add'][1] + rd + opcodes['add'][0]
        print(out)
    elif op in ['sw', 'SW', 'sb', 'SB']:
        rs2, imm, rs1 = map(int, re.split("[,，()（）]", code[1])[0:-1])
        if imm > 2047 or rs2 > 31 or rs1 > 31:
            print("warning：超过最大上限")
            continue
        elif imm < -2048 or rs2 < 0 or rs1 < 0:
            print("warning：超过最低下限")
            continue
        rs2, imm, rs1 = to_two(rs2), to_two(imm, 12), to_two(rs1)
        # rs1 = ext_regi(rs1)
        # rs2 = ext_regi(rs2)
        # imm = ext_regi(imm, 12)
        out = imm[0:7] + rs2 + rs1 + opcodes[op][1] + imm[7:] + opcodes[op][0]
        print(out)
    elif op in ['jalr', "JALR", 'lw', 'LW', 'lbu', "LBU"]:
        rd, imm, rs1 = map(int, re.split("[,，()（）]", code[1])[0:-1])
        if imm > 2047 or rd > 31 or rs1 > 31:
            print("warning：超过最大上限")
            continue
        elif imm < -2048 or rd < 0 or rs1 < 0:
            print("warning：超过最低下限")
            continue
        rd, imm, rs1 = to_two(rd), to_two(imm, 12), to_two(rs1)
        out = imm + rs1 + opcodes[op][1] + rd + opcodes[op][0]
        print(out)

    print(to_o0(out))
