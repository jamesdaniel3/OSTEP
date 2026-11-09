### First just run checksum.py with no arguments. Compute the additive, XOR-based, and Fletcher checksums. Use -c to check your answers.

### Now do the same, but vary the seed (-s) to different values.

### Sometimes the additive and XOR-based checksums produce the same checksum (e.g., if the data value is all zeroes). Can you pass in a 4-byte data value (using the `-D` flag, e.g., `-D a,b,c,d`) that does not contain only zeroes and leads the additive and XOR-based checksum having the same value? In general, when does this occur? Check that you are correct with the `-c` flag.

Yeah if you use `-D 128,128,128,128`, the addative and XOR values will have the same checksum. This happens because the addative method ignores bits that overflow, so $128 + 128 = 0$. This can also be achieved by keeping bits set in different locations so something like `128,64,32,16` works because none of the bits are lost due to XOR'ing.

### Now pass in a 4-byte value that you know will produce a different checksum values for additive and XOR. In general, when does this occur?

This occurs anytime that you are dealing with numbers that have bits set in the same column and do not have the addition lost due to modulo. 

### Use the simulator to compute checksums twice (once each for a different set of numbers). The two number strings should be different (e.g., `-D a1,b1,c1,d1` the first time and `-D a2,b2,c2,d2` the second) but should produce the same additive checksum. In general, when will the additive checksum be the same, even though the data values are different? Check your specific answer with the `-c` flag.

I'm assuming this is treating numbers as sets so I can't just reorder them. The addative checksum will produce the same values any time the sum of the 4 values modulo 256 is the same. So `66,1,157,357` and `32,16,16,5` are the same because $66 + 1 + 157 + 357 = 581 MOD 256 = 69$ and $32 + 16 + 16 + 5 = 69 MOD 256 = 69$

### Now do the same for the XOR checksum.

An easy example is $0,0,0,0$ and $255,255,255,255$. It would be harder for me to pick values for this one without looking at the binary, but this will be true any time the tallies in the column are all both even or odd, assuming 0 is considered even. So in the example that I gave; if you add up the count of 1's in every bit place you would get 0, and if you do the same for the second set of numbers you could get 4. A less obvious example might be something like `128,64,32,16` and `192,8,240,200`

### Now let’s look at a specific set of data values. The first is: `-D 1,2,3,4`. What will the different checksums (additive, XOR, Fletcher) be for this data? Now compare it to computing these checksums over `-D 4,3,2,1`. What do you notice about these three checksums? How does Fletcher compare to the other two? How is Fletcher generally “better” than something like the simple additive checksum?

XOR and basic addition result in the same checksum's in the above examples, but fletcher results in different checksums. the running addition allows it to account for the order in which we saw the bytes rather than just the total. 

### No checksum is perfect. Given a particular input of your choosing, can you find other data values that lead to the same Fletcher checksum? When, in general, does this occur? Start with a simple data string (e.g., `-D 0,1,2,3`) and see if you can replace one of those numbers but end up with the same Fletcher checksum. As always, use `-c` to check your answers.

`0,1,2,3` and `1,0,1,4` result in the same Fletcher checksum. For this to occur in the general case, both sets of numbers first have to have the same total MOD 255. Addiitonally, I think the sum of all the intermediate values when calculating the first bit must be the same. I feel like there should be a better way to put the second condition but I haven't found it. 