CFLAGS := -DCL_USE_DEPRECATED_OPENCL_2_0_APIS=1 -Wall
LDLIBS := -lOpenCL

OUT := cl-compile

%.bin: %.cl
	ioc64 -cmd=build -input=$< -ir=$@

%.spir32: %.cl
	ioc64 -cmd=build -input=$< -spir32=$@

%.spir64: %.cl
	ioc64 -cmd=build -input=$< -spir64=$@

all: $(OUT)

clean:
	-rm *.o
	-rm $(OUT)
