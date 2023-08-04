myfunc count name facts {
    p (concat "There are " .count " " .name "s!")
    p (concat "That's " .facts " yo!")

    ofunc name yo {
        p (concat .name " ain't " .yo)
    }

    ofunc "Jack" "Jack"
}

myfunc 12 "dog" "facts"