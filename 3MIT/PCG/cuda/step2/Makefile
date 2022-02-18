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

# step 0
run10: 
	./nbody 25600 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/25600.h5 ../sampledata/25600_out.h5
run11: 
	./nbody 28160 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/28160.h5 ../sampledata/28160_out.h5
run12: 
	./nbody 30720 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/30720.h5 ../sampledata/30720_out.h5
run13: 
	./nbody 33280 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/33280.h5 ../sampledata/33280_out.h5
run14: 
	./nbody 35840 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/35840.h5 ../sampledata/35840_out.h5
run15: 
	./nbody 38400 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/38400.h5 ../sampledata/38400_out.h5
run16: 
	./nbody 40960 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/40960.h5 ../sampledata/40960_out.h5
run17: 
	./nbody 43520 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/43520.h5 ../sampledata/43520_out.h5
run18: 
	./nbody 46080 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/46080.h5 ../sampledata/46080_out.h5
run19: 
	./nbody 48640 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/48640.h5 ../sampledata/48640_out.h5
run20: 
	./nbody 51200 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/51200.h5 ../sampledata/51200_out.h5
run21: 
	./nbody 53760 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/53760.h5 ../sampledata/53760_out.h5
run22: 
	./nbody 56320 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/56320.h5 ../sampledata/56320_out.h5
run23: 
	./nbody 58880 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/58880.h5 ../sampledata/58880_out.h5
run24: 
	./nbody 61440 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/61440.h5 ../sampledata/61440_out.h5
run25: 
	./nbody 64000 ${DT} ${STEPS} ${THREADS_PER_BLOCK} ${WRITE_INTESITY} ${RED_THREADS} ${RED_THREADS_PER_BLOCK} ../sampledata/64000.h5 ../sampledata/64000_out.h5

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
		./nbody ${N} ${DT} 1 ${THREADS_PER_BLOCK} 0  ${RED_THREADS} ${RED_THREADS_PER_BLOCK} $(SAMPLE_INPUT) $(OUTPUT) 