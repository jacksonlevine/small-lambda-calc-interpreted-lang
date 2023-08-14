newadd vecname item {
    append .item .vecname
    print (concat "Appended " .item " to " .vecname)}

heyyo []

newadd heyyo 10
newadd heyyo 1
newadd heyyo 8
newadd heyyo 4
newadd heyyo 2
newadd heyyo 23
newadd heyyo 33
newadd heyyo 0

forin heyyo print

funco x {
    if (> .x 3) { print (concat .x " is a very important thing.") }
}

forin heyyo funco

print (
    concat 
    3
    4
    2
)

print ( funco a { 
    return (concat "hey" .a)
 } 8)


print (
    bingo x y z {
        return .x ", " .y ", " .z
    } 8 5 1
)