## Simple Lambda-Calculus-like Interpreted Language

I wanted to write dynamically-loaded scripts in Haskell that my C++ program could run, but there was no such possibility, which sparked the idea of trying to write a simple functional language, going back more or less to the roots of Lambda Calculus as a dead-simple poly-morphic answer to computing, which is just functions, functions, functions.

This is barely the start of the things I'd like to add, will be updating with new features soon. I am not a professional or expert so this is merely for my own fun.

This has also probably been made very similarly by many people, I am not claiming ownership of this idea.

### An Explanation:

Everything in this language is a function call structured as:

`func arg1 arg2 arg3` (there can be infinitely many args. The func name is just what is at offset 0 in the vector.)

Parentheses are used to nest other functions that will be recursively evaluated from deepest to shallowest.

These functions often have side effects and state, which, among other things, makes this less like a lambda-calculus.

All arguments are strings. All functions evaluate to strings. The interpreter internally casts strings to floats or ints manually when it is expected that a function will be receiving numeric arguments.

### Examples: 

### print function (any # of args)

```swift
print .var1 .var2 (+ .var1 .var3)
```

For every arg it's given, `print` prints it on a new line.

To print multiple on the same line, use the:

### concat function (any # of args)

```swift
s fruit_type orange

s fruit_amt 4

print (concat "The number of " .fruit_type "s you have is" .fruit_amt)
```

Combines strings.

### let/set function (2 args)

Creates a variable named arg1 with the value arg2, like this: 
```swift
let x 8
```

This also overwrites the variable if it already exists, which is why the `set` function is just an alias of `let`. Other aliases include `l`, `s`, `le`, `s`, and `se`.

I will be using the `s` alias in most examples.

### Variable references

To use a variable, use its name with a . before it, and the interpreter will swap it out for the current value of the variable before executing.

Prints 8: 

```swift
s x 4
print (+ .x 4)
```

This can be used in a function call to change what function is being called:

```swift
s x print

.x 2 2 

s x +

.x 2 2
``` 

### + function (any # of args)

Evaluates to 4: 

```swift
+ 2 1 1
```

Evaluates to 6: 

```swift
+ (+ 2 3) (+ 1 0)
```

Evaluates to 6: 

```swift
+ (+ 2 3) (+ 1 0)
```

Ignore the .swift file extension, it just happens to cause my IDE to highlight the syntax quite nicely.