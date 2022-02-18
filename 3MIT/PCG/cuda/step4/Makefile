# Paralelní programování na GPU (PCG 2021)
# Projekt c. 1 (cuda)
# Login: xsadil07


N=4096
DT=0.01f
STEPS=500
THREADS_PER_BLOCK=128
RED_THREADS=4096
RED_THREADS_PER_BLOCK=128
WRITE_INTESITY=20

SAMPLE_INPUT=../sampledata/sampleInput.h5
SAMPLE_OUTPUT=../sampledata/sampleOutput.h5
OUTPUT=step0Output.h5

INCLUDE=../commons
LIBS=-lhdf5

FLAGS=  

.PHONY: all clean run profile

all: nbody

nbody: nbody.cu main.cu nbody.h
	nvcc ${FLAGS} -I${INCLUDE} nbody.cu main.cu ../commons/h5Helper.cpp ${LIBS} -o nbody

clean:
	rm -f *.o nbody

run:
	./nbody ${N} ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} $(SAMPLE_INPUT) $(OUTPUT)

# step 5
run2k: 	
	./nbody 2048 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 2048 128 ../sampledata/2048.h5 ../sampledata/2048_out.h5
run4k: 	
	./nbody 4096 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 4096 128 ../sampledata/4096.h5 ../sampledata/4096_out.h5
run8k: 	
	./nbody 8192 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 8192 128 ../sampledata/8192.h5 ../sampledata/8192_out.h5
run16k: 
	./nbody 16384 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 16384 128 ../sampledata/16384.h5 ../sampledata/16384_out.h5
run32k: 
	./nbody 32768 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 32768 128 ../sampledata/32768.h5 ../sampledata/32768_out.h5
run64k: 
	./nbody 65536 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 65536 128 ../sampledata/65536.h5 ../sampledata/65536_out.h5
run128k: 
	./nbody 131072 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 131072 128 ../sampledata/131072.h5 ../sampledata/131072_out.h5
run256k: 
	./nbody 262144 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 262144 128 ../sampledata/262144.h5 ../sampledata/262144_out.h5
run512k: 
	./nbody 524288 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 524288 128 ../sampledata/524288.h5 ../sampledata/524288_out.h5
run1024k: 
	./nbody 1048576 ${DT} ${STEPS} 128 ${WRITE_INTESITY} 1048576 128 ../sampledata/1048576.h5 ../sampledata/1048576_out.h5

check_output:
	./nbody 4096 0.01f 500 ${THREADS_PER_BLOCK} 20 ${RED_THREADS} ${RED_THREADS_PER_BLOCK} $(SAMPLE_INPUT) $(OUTPUT)
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_x_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_y_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_z_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_x_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_y_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_z_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /weight_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_x_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_y_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_z_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_w_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_x
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_y
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_z
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_x
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_y
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_z
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /weight
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_x
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_y
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_z
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /com_w

check_final:
	./nbody 4096 0.01f 500 ${THREADS_PER_BLOCK} 20 ${RED_THREADS} ${RED_THREADS_PER_BLOCK} $(SAMPLE_INPUT) $(OUTPUT)
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_x_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_y_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /pos_z_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_x_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_y_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /vel_z_final
	-h5diff -v2 -p 0.00001 $(SAMPLE_OUTPUT) $(OUTPUT) /weight_final

check_com:
	./nbody 10 0.01f 10000 ${THREADS_PER_BLOCK} 0 ${RED_THREADS} ${RED_THREADS_PER_BLOCK} $(SAMPLE_INPUT) $(OUTPUT)


profile:
	nvprof \
		--devices 0 \
		--metrics flops_sp \
		--metrics flops_sp_add \
		--metrics flops_sp_mul \
		--metrics flops_sp_fma \
		--metrics flops_sp_special \
		--metrics flop_sp_efficiency \
		--metrics sm_efficiency \
		--metrics alu_fu_utilization \
		--metrics gld_transactions \
		--metrics l1_cache_global_hit_rate \
		--metrics shared_load_transactions \
		--metrics gld_throughput \
		--metrics flop_count_sp \
		./nbody ${N} ${DT} 1 ${THREADS_PER_BLOCK} 0  ${RED_THREADS} ${RED_THREADS_PER_BLOCK} $(SAMPLE_INPUT) $(OUTPUT) 