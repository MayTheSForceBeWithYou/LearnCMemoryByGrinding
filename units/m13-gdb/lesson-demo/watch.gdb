set debuginfod enabled off
set pagination off
break main
run watch
watch game.round
continue
bt
print i
print v
continue
