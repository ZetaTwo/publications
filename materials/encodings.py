#!/usr/bin/env python3

# -*- coding: utf-8 -*-

import random
import struct

def server_emulator():
    """
    This function emulates receiving a reponse consisting of the following memory leak:
    [A random sequence of bytes of random (0-512) length][A fixed phrase in Swedish (with UTF-8 encoded Unicode characters)][A little endian encoded 64-bit number]...
    """
    response = b''
    response += bytes([random.randint(0,255) for _ in range(random.randint(0, 512))])
    response += 'Här får du: '.encode('utf-8')
    response += struct.pack('<Q', 0xCAFEBABEFEEDF00D)
    response += bytes([random.randint(0,255) for _ in range(random.randint(0, 512))])

    return response

def decode_leak_verbose(leak):
    """This function decodes the memory leak. Verbose version"""
    phrase_marker = 'Här får du: '.encode('utf-8')
    phrase_start = leak.find(phrase_marker)
    leak_start = phrase_start + len(phrase_marker)
    pointer_bytes = leak[leak_start:leak_start+8]
    cool_pointer = struct.unpack('<Q', pointer_bytes)[0]

    assert(cool_pointer == 0xCAFEBABEFEEDF00D)

    print('Leak length: %d' % len(leak))
    print('Phrase start index: %d' % phrase_start)
    print('Leak start index: %d' % leak_start)
    print('Leaked bytes (len: %d): %s' % (len(pointer_bytes), repr(pointer_bytes)))
    print('Leaked pointer: %016x' % cool_pointer)

def decode_leak(leak):
    """This function decodes the memory leak. More condensed"""
    fixed_phrase = 'Här får du: '.encode('utf-8')
    cool_pointer = struct.unpack("<Q", leak[leak.find(fixed_phrase) + len(fixed_phrase):leak.find(fixed_phrase) + len(fixed_phrase) + 8])[0]
    assert(cool_pointer == 0xCAFEBABEFEEDF00D)
    print('Leaked pointer: %016x' % cool_pointer)

def decode_leak_compact(leak):
    """This function decodes the memory leak. Single expression"""
    cool_pointer = struct.unpack("<Q", leak[leak.find('Här får du: '.encode('utf-8')) + len('Här får du: '.encode('utf-8')):leak.find('Här får du: '.encode('utf-8')) + len('Här får du: '.encode('utf-8')) + 8])[0]
    assert(cool_pointer == 0xCAFEBABEFEEDF00D)
    print('Leaked pointer: %016x' % cool_pointer)

def decode_leak_wrong(leak):
    """This function tried to decode the memory leak but fails."""
    leak_string = leak.decode('utf-8')
    phrase_marker = 'Här: '
    phrase_start = leak_string.find(phrase_marker)
    leak_start = phrase_start + len(phrase_marker)
    pointer_characters = leak_string[leak_start:leak_start+8]
    cool_pointer = struct.unpack('<Q', pointer_bytes)[0]
    assert(cool_pointer == 0xCAFEBABEFEEDF00D)
    print('Leaked pointer: %016x' % cool_pointer)

def decode_leak_python2(leak):
    """This function tried to decode the memory leak but fails."""
    phrase_marker = 'Här: '
    phrase_start = leak.find(phrase_marker)
    leak_start = phrase_start + len(phrase_marker)
    pointer_characters = leak[leak_start:leak_start+8]
    cool_pointer = struct.unpack('<Q', pointer_characters)[0]
    print(repr(pointer_characters), repr(cool_pointer))
    assert(cool_pointer == 0xCAFEBABEFEEDF00D)
    print('Leaked pointer: %016x' % cool_pointer)

leaked_data = server_emulator()
decode_leak_verbose(leaked_data)
decode_leak(leaked_data)
decode_leak_compact(leaked_data)
#decode_leak_wrong(leaked_data)
decode_leak_python2(leaked_data)

