newadd vecname item {
    append .item .vecname
    print (concat "Appended " .item " to " .vecname)}

heyyo []

newadd heyyo 10
newadd heyyo blue
newadd heyyo hey
newadd heyyo test!

forin heyyo print

print (size heyyo)

funco x {
    print (concat .x " is a very important thing.")
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
