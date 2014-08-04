rmmod c64

sleep 10

./go64.sh

sleep 10

# GPP-only float (original code)
./c64_fractal_gppfpu

sleep 10

# _FPmpy original
./c64_fractal_gpponly

sleep 10

# _FPmpy opt
./c64_fractal_gpponly2

sleep 10

./c64_fractal_dsponly

sleep 10

./c64_fractal_gppdsp

sleep 10
