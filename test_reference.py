class A:
   def __init__(self):
       self.x = 4
   def p(self):
       self.x = 3

a = A()
b = a
print(b.x)
a.p()
print(b.x)
