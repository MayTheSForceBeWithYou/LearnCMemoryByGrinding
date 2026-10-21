set debuginfod enabled off
set pagination off
break buf_add if b->len == b->cap
run grow
print *b
print b->data
finish
continue
print *b
print *b->data@b->len
print/c b->data[0]@4
x/8cb b->data
delete
continue
