set debuginfod enabled off
set pagination off
set print address on
break realloc
commands
  silent
  printf "realloc(%p, %lu)\n", $rdi, $rsi
  bt 2
  continue
end
break free
commands
  silent
  printf "free(%p)\n", $rdi
  bt 2
  continue
end
break main.c:36
run
delete 1 2
print v
print *v.data@v.len
print v.data[v.len - 1]@2
continue
