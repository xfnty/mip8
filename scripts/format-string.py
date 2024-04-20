data = 'Hello, World!\n'
out = []

for c in data:
    s = hex(ord(c))[2:].rjust(2, '0').upper()
    out.append(s)

for e in reversed(out):
    print(f'psp ${e} ; \'{chr(int(e, 16))}\'')

for e in out:
    print('pop')