myfunc count name facts {
    p (concat "There are " .count " " .name "s!")
    p (concat "That's " .facts " yo!")

    ofunc name yo {
        p (concat .name " ain't " .yo)
    }

    ofunc "Jack" "Jack"
}

myfunc 12 "dog" "facts"

heyyo num1 num2 {
    + .num1 .num2
}

print (concat "2 + 3 is " (heyyo 2 3))

thisvec [ yoZero yoOne yoTwo yoThree yoFour ]

print (concat "The index 2 value of " thisvec " is: " (thisvec 2))
print (thisvec 1)
print (thisvec 0)
