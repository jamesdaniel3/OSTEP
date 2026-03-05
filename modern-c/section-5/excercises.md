## Excercise 1:

#### Show that if the minimal and maxiumal values for signed long long have similar properities, the smallest integer value for the platform can't be written as a combination of one literal with a minus sign.

The following information was explained before the excercise and is necessary context:

- Numerical literals in C are never negative; if you see something like -1, it is +1 being negated
- To determine the exact type for integer literals, we have a first fit rule, which means that a decimal integer literal has the first of the three signed types that it fits
  - side note: these types are (signed) int, (signed) long, and (signed) long long
- This can lead to some weird effects. Here is the example they gave.

Say that the minimal signed value on a platform is -2^15 = -32,768 and the maxium value is 2^15 - 1 = 32,767. If I write the constant -32,768 it will actual have the type signed long, despite
the number that you think it evaluates to fitting within the signed type constraint. This is because it is being evaluted as 32,768, which doesn't fit, and is then negated.

I think the proof for this question following immediately from what was described above. If all negative decimal literals are evaluated as positive decimal literals which are immediately
negated; than the written form of this would fail during compilation because the decimal would be 1 greater than the limit for the size, as it is evaluated as a positive.

## Excercise 2:

#### Show that if the maximum unsigned is 2^16 -1, then -0x8000 has value 32,768, too.

According to [this guy](https://www.nayuki.io/page/summary-of-c-cpp-integer-rules) (I can't find an actual C standard), the rule for converting to an unsigned type is:

```
newValue = oldValue % 2^(destinationBitWidth)
```

In C and in math (but not in all programming languages), taking the modulo of a negative number just means adding the divisor to the number until it is >=0 and < divisor.

If the maximum unsigned integer is 2^16 - 1, than we can assume the maximum signed integer is 2^15 - 1 (sign bit). So in this question, the range of signed integers must be [-32,768, 32,767]. The value 0x8000, when converted to decimal, is 32,768. Because this doesn't fit in the range of signed integers, it will bump up to be an unsigned int. Next, it will be negated. Because it cannot remain negative, the modulo operator will be used to bring it back within a valid range, now [0, 65,536).

```
-32,768 + 65,536 = 32,768
```

So to be clear, if any other number was put through this, it would not come out as itself negated, that just happened because the number used was the exact halfway point.

## Excercise 3:

#### Show that the expressions -1U, -1UL, and -1ULL have the maxiumum values and type as the three non-promoted unsigned types, respectively.

This can borrow alot from the explanation before.

All three values are 1 and negated, and forced to be unsigned. Note that the three types denoted by the prefixes are `unsigned`, `unsigned long`, and `unsigned long long`. In the previous question I explained how we handle negation for unsigned values. Let's apply the same logic here, taking DBW_U to be the number of bits used for the unsigned type, with DBW_UL, and DBW_ULL following the same pattern:

```
-1 + 2^(DBW_U) = 2^(DBW_U) - 1 = max possible value
```

```
-1 + 2^(DBW_UL) = 2^(DBW_UL) - 1 = max possible value
```

```
-1 + 2^(DBW_ULL) = 2^(DBW_ULL) - 1 = max possible value
```

## Excercise 4:

#### Under the assumption that the maximum value for unsigned int is 0xFFFF'FFFF, prove that -0x8000'0000 == 0x8000'0000

This is essentially a rehashing of question 2.

Maximum value: 4,294,967,295
0x8000'0000: 2,147,483,648

When looking at the value, it can't be signed because if the range of the unsigned ints is [0, 4,294,967,295], than the range of the signed ints is [-2,147,483,648, 2,147,483,647].
We look at the value as an unsigned value which has been negated and then modulo it. Because it is the halfway point it ends up being equal to itself.
