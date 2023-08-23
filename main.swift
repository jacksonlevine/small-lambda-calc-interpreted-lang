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