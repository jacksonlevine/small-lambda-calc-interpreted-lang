array []

let x 10
_ { 
    append .x array
    set x (+ .x 10)
}
repeat 500 _

print_filtered x {
    if (! (% .x 3)) {
        print (concat .x " is evenly divisible by 3")
    }
}

forin array print_filtered