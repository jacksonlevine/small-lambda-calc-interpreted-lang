newadd vecname item {
    append .item .vecname
    print (concat "Appended " .item " to " .vecname)
}

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