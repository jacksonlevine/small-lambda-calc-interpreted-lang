## Boris: Simple Interpreted Language

Made as simply a fun challenge for myself, not intended for wide usage.

This code prints every number 0-5000 that is evenly divisible by 3:

```
array []

let x 10

_ { 
    append .x array
    set x (+ .x 1)
}
repeat 5000 _

print_filtered x {
    if (! (% .x 3)) {
        print (concat .x " is evenly divisible by 3")
    }
}

forin array print_filtered

```

The syntax is function definitions and function calls. For example, let/set is a function that creates or reassigns a global variable. 

All values are strings, and can be treated as all the same thing, except in functions like + where the number is parsed as a float or int. (In all of these cases, if one argument is a float they are all treated as floats.)

Function definitions can have parameter variables, scoped only to that function body (Parameters can have the same name as global variables, local version will be used first if it exists).

Any series of words followed by { } is a function definition, UNLESS it begins with `if (a function call that evals to truthy or falsey)`, then it is an if-statement.

Arrays are c++ vectors under the hood and are declared as such: `name_you_want []` for an empty array, or `name_you_want [ 3, .a_variable, 5, 1, "A string" ]`

The available built-in functions will have documentation soon, but for now are viewable in "function_definitions" function in main.cpp. They include: "+", "%", "/", "concat", "print", "repeat", and "forin".

Thank you!

### MIT License:

Copyright 2023 Jackson Levine

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.