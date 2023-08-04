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


s name Yo
s other 10
thisvec [ .name .other "green" "blue" ]

forin thisvec print

print (concat "Size of thisvec: " (size thisvec))

append .name thisvec
append .other thisvec

forin thisvec print

print (concat "Size of thisvec: " (size thisvec))