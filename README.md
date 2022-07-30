<div align="middle">

# Blend Interpolator v0.9

</div>


#### COMPILE WITH GCC TESTED

    gcc -Wall -std=c99 -m64 -O2  -c main.c -o main.o
    windres.exe   -J rc -O coff -i resource.rc -o resource.res
    gcc -o colorinter main.o resource.res -static -m64 -lgdi32 -s -mwindows

#### NOTES

‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ WINDOWS 10, 11 program. DOES NOT WORK ON WIN7 without a lot of glitches !! You have been warned.

‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ This is a GDI32 Win32 Application.

‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ ‌‌ This is a fun little project to find the hex / decimal / percentage values between two color values.

<br>

<div align="middle">

![progress](image.png "progress")

</div>
