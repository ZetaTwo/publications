#!/usr/bin/env python3

import math

def sqrt_newton(x, iter=10):
    xn = x/2 # initial guess
    for _ in range(iter):
        xn = 1/2*xn + x/(2*xn)
    return xn

def cbrt_newton(x, iter=10):
    xn = x/3 # initial guess
    for _ in range(iter):
        xn = 2/3*xn + x/(3*xn*xn)
    return xn

def generate_primes(limit=400):
    is_prime=[True]*(limit-2)
    primes=[]
    for prime in range(limit-2):
        if not is_prime[prime]:
            continue
        primes.append(prime+2)
        for k in range(2, limit-2):
            if (prime+2)*k-1 > limit-2:
                break
            print(k, prime+2)
            is_prime[(prime+2)*k-2] = False
    
    return primes

print(generate_primes(313))

print(hex(int((pow(3, 1/2)*(1<<32)))))
print(sqrt_newton(123, 10))

print(pow(3, 1/3))
print(cbrt_newton(123, 10))
