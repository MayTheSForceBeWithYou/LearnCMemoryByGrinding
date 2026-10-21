# M01 e03 notes

| fn | valid? | warns at -O0 | warns at -O2 |
|---|---|---|---|
| f1 `return &x;` | no (automatic) | yes `[-Wreturn-local-addr]` | yes |
| f2 via `int *p = &x` | no (automatic) | **no** | yes `[-Wreturn-local-addr]` |
| f3 via struct field | no (automatic) | no | no |
| f4 literal | yes (static) | no | no |
| f5 static local | yes (static) | no | no |
| f6 parameter | yes (the caller's object) | no | no |

Captured on the course machine (gcc 16.2.1, plain flags plus `-O0` / `-O2`, `-c main.c`), warning lines only:

```
-O0:
main.c:19:12: warning: function returns address of local variable [-Wreturn-local-addr]
-O2:
main.c:19:12: warning: function returns address of local variable [-Wreturn-local-addr]
main.c:26:12: warning: function returns address of local variable [-Wreturn-local-addr]
```

**Why.** At -O0, the warning is a front-end pattern match on `return &local`. At -O2, the optimiser
propagates `p = &x` into the return, so f2 becomes visible. The struct in f3 hides the address from
both. That's the general lesson: the warning is a heuristic, and its absence proves nothing. ASan's
`detect_stack_use_after_return=1` (M11) catches f3 at run time, if a test actually dereferences it.
