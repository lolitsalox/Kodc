# Syntax
- `[]` is optional 

## Explicit assignment
### `<id>: <dtype> = <expr>`
```rust
x: i32 = 123
y: #i32 = #x
```

## Implicit assignment
### `<id> = <expr>`
### `<id> -> <id>`
```rust
x = 123
y -> x  // y = #x
```

## Assignment between expressions
### `<expr> = <expr>`
```rust
@x = 123
```

## Function
### `[<id>] (<id>: <dtype>, ...) { <compound> }`
```rust
f (a: i32) {
    return a 
}
```

## Call
### `<id>(<expr>, ...)`
```rust
main(1, 2, 3)
```

