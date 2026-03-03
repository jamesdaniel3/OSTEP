#### Page 65

```
Deducing the type of a binary, octal, or hexadecimal literal is a bit more complicated. These can also be of an unsigned type if the value doesn't fit for a signed type. In the earlier example, the hexadecimal literal 0x7FFF has the value 32,767 and thus is type signed. Other than for the decimal literal, the literal 0x8000 (value 32,768 written in hexadecimal) then is an unsigned, and expresseion -0x8000 again is unsigned.
```

So I don't think this was explained well in the book and my current explanation is from ChatGPT because I haven't dug around for a more concrete source. It is saying that bascially instead of just sizing up to the next biggest signed int, these types mix in unsigned. So:

Integers: int --> long --> long long

Hex, Octal, Binary: int --> unsigned int --> long --> unsigned long --> ...

This is because these numbers are most often used for things like memory addresses and bit masked, so it is not likely that they will need a type.
