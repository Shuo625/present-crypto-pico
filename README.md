

# present-crypto-pico

Bitslice and non-bitslice implement of present crypto running on raspberrypi pico.

## Usage

### Compile

In the directory of one of two implements:

```bash
mkdir build
cd build
cmake ..
make
```

### Test

Under Ubuntu the com port is ttyACM0. Under other operating system it may be different.

```bash
sudo python test_against_testvectors.py /dev/ttyACM0
```

## Present\_ref

Test result is following:

![](/home/shuo/Projects/present-crypto-pico/assets/2022-04-08-00-58-34-image.png)

## Present\_bs

I implement three optimizations which are *unfold\_loop*, *simplify\_sbox\_anf*, and *multicore* and use three macros which are **OPTIMIZATION_SBOX**, **OPTIMIZATION_MULTICORE**, and **OPTIMIZATION_UNFOLD_LOOP** to control whether or not to use corresponded optimization.



And the reason why I don't use inline assembly is the build system of pico-sdk has used -O3 compiler option which will generate wrong assembly with inline assembly.

### OPTIMIZATION\_NONE

Test result with none optimization is:

<img src="file:///home/shuo/Projects/present-crypto-pico/assets/2022-04-08-01-10-48-image.png" title="" alt="" width="692">

### OPTIMIZATION\_SBOX

Original boolean expressions are:

```c
y0 = x0 ^ x2 ^ (x1 & x2) ^ x3;
y1 = x1 ^ (x0 & x1 & x2) ^ x3 ^ (x1 & x3) ^ (x0 & x1 & x3) 
     ^ (x2 & x3) ^ (x0 & x2 & x3);
y2 = 0xffffffffu ^ (x0 & x1) ^ x2 ^ x3 ^ (x0 & x3) ^ (x1 & x3) 
     ^ (x0 & x1 & x3) ^ (x0 & x2 & x3);
y3 = 0xffffffffu ^ x0 ^ x1 ^ (x1 & x2) ^ (x0 & x1 & x2) ^ x3 
     ^ (x0 & x1 & x3) ^ (x0 & x2 & x3);
```

Simplified boolean expressions are:

```c
x3_and_x1_xor_x2 = x3 & (x1 ^ x2);
x1_xor_x3 = x1 ^ x3;
x1_and_x2 = x1 & x2;

y0 = x0 ^ (~x1 & x2) ^ x3;
y1 = x1_xor_x3 ^ (x0 & x1_and_x2) ^ (~x0 & x3_and_x1_xor_x2);
y2 = ~x2 ^ (x0 & x1_xor_x3) ^ (~x1 & x3) ^ (x0 & x3_and_x1_xor_x2);
y3 = (~x0 & ~x1_and_x2) ^ x1_xor_x3 ^ (x0 & x3_and_x1_xor_x2);
```

Test result with only **OPTIMIZATION\_SBOX** is:

![](/home/shuo/Projects/present-crypto-pico/assets/2022-04-08-01-22-42-image.png)

So you can see the improvement of this optimization is not so large compared with non-optimization. The reason is that as I said before the build system of pico-sdk has already used -O3 optimization which has already optimize these operation much.

### OPTIMIZATION\_UNFOLD\_LOOP

This optimization unfold the inner loop in the enslice and unslice function which originally use nested loop of two levels. The detailed codes can be checked in the source file.



Test result with only **OPTIMIZATION\_UNFOLD\_LOOP** is:

![](/home/shuo/Projects/present-crypto-pico/assets/2022-04-08-01-31-19-image.png)

So you see there is no improvement and even worse then that of non-optimization. The reason of no improvement I think is because unfold\_loop will only save tens of branch commands so we cannot see the improvement. And I think the reason why this is worse than non-optimization is also because of the -O3 compiler option.

### OPTIMIZATION\_MULTICORE

There are two cores on the pico. So I use two cores to calculate the encryption asynchronously.



Test result with only **OPTIMIZATION\_MULTICORE** is:

![](/home/shuo/Projects/present-crypto-pico/assets/2022-04-08-01-37-54-image.png)

This optimization improves the performance eminently and nearly double the speed of non-optimization because we use two cores.

### OPTIMIZATION\_ALL

We use all of optimizations together.



The result with all optimizations is:

![](/home/shuo/Projects/present-crypto-pico/assets/2022-04-08-01-42-33-image.png)

There is still a little improvement compared with that of **OPTIMIZATION\_MULTICORE**.
