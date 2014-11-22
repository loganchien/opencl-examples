CFLAGS := -DCL_USE_DEPRECATED_OPENCL_2_0_APIS=1
LDLIBS := -lOpenCL

OUT := vec_add_source vec_add_binary vec_add_spir \
       vec_add.bin vec_add.spir32 vec_add.spir64

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
