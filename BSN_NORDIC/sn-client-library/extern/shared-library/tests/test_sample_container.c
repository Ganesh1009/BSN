#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define IMU_DATA_SIZE  22
#define NR_SYSTEMS     5
#include "sample_container.h"

//what do i need to have variable:
// size of sample_container
// max_time_diff_ms
// samples_optional?
// nr_sample_writes: number of times that we write in the sample_container //in for
// pop_concurrency: how often do we want to pop
//concurrency_1 : how often do we write in pos 1

void test(uint8_t sample_container_size, uint8_t max_time_diff_ms, bool optional_sample_f,
		uint8_t nr_sample_writes, uint8_t pop_concurrency, uint8_t concurrency_1){
	sample_dataset_t* sample_container_ptr;
	sample_dataset_t sample_container_current;
	uint8_t err_ctn[NR_SYSTEMS] = { 0 } ;

	sample_container_init(sample_container_size);
	sample_container_set_max_time_diff_ms(max_time_diff_ms);
	uint8_t imudummydata[NR_SYSTEMS][IMU_DATA_SIZE];
	for(uint8_t j = 0; j < NR_SYSTEMS; j++){
		for(uint8_t i = 0; i< IMU_DATA_SIZE; i++){
			imudummydata[j][i] = (1 + i) + ((j)*IMU_DATA_SIZE);
		}
	}
	sample_data_t master_sys_sample = sample_container_create_system_data(
			imudummydata[0], IMU_DATA_SIZE, optional_sample_f, s_imu, 0);
	for(uint8_t i = 1; i<nr_sample_writes; i++){
//		sample_container_push(10*i, 10*i, MASTER_INDEX, &master_sys_sample);
//		if(i%concurrency_1 == 0){
//			master_sys_sample.buffer = imudummydata[1];
//			for(uint8_t j =  concurrency_1-1; j < concurrency_1; j++){
//				sample_container_push(10*(i-j), 10*(i-j), 1, &master_sys_sample);
//			}
//		}
		for(uint8_t j = 0; j < NR_SYSTEMS ; j++){
			master_sys_sample.buffer = imudummydata[j];
			sample_container_push(10*i, 10*i, j, &master_sys_sample);
		}
		if(i % pop_concurrency == 0){
			for(sample_container_ptr = sample_container_pop();
					sample_container_ptr!=NULL;
					sample_container_ptr = sample_container_pop()){

				memcpy(&sample_container_current, sample_container_ptr, sizeof(sample_dataset_t)); // todo: not ok: return size of pointer and copy properly
				printf("pop %d systems_Data ", sample_container_current.counter);
				for(uint8_t j= 0; j< sample_container_get_nr_systems(); j++){
					printf(" %d ", sample_container_current.system[j]->innerTimer);
					if(sample_container_current.globaltime - sample_container_current.system[j]->innerTimer > 5)
						printf(" old sample for id %d", sample_container_current.system[j]->system_id);
				}
				printf(" \n");

				for(uint8_t j = 0; j< sample_container_get_nr_systems(); j++){
					if(memcmp(sample_container_current.system[j]->buffer,  imudummydata[j], IMU_DATA_SIZE) !=0 ){
						printf("Error in system %d\n", j);
						err_ctn[j]++;
					}
				}
			}
		}
	}
	printf("Nr packages tx with < existing systems ");
	for ( uint8_t j = 0; j< NR_SYSTEMS; j++)
		printf(" %d ", err_ctn[j]);
}

void first_test(uint8_t sample_container_size, uint8_t nr_systems , bool optional_sample_3){
	  sample_dataset_t* sample_container_ptr;
	  sample_dataset_t sample_container_current;
	  uint8_t err_ctn[NR_SYSTEMS] = {0};
	  sample_container_init(sample_container_size);
	  sample_container_set_max_time_diff_ms(15);

	  uint8_t imudummydata[nr_systems][IMU_DATA_SIZE];
	  for(uint8_t j = 0; j < nr_systems; j++){
		  for(uint8_t i = 0; i< IMU_DATA_SIZE; i++){
			  imudummydata[j][i] = (1 + i) + ((j)*IMU_DATA_SIZE);
		  }
	  }
	  sample_data_t master_sys_sample = sample_container_create_system_data(
			  imudummydata[0], IMU_DATA_SIZE, false, s_imu, 0);

	  sample_data_t node1_sys_sample = sample_container_create_system_data(
			  imudummydata[1], IMU_DATA_SIZE, false, s_imu,1);

	  sample_data_t node2_sys_sample = sample_container_create_system_data(
			  imudummydata[2], IMU_DATA_SIZE, false, s_imu,2);

	  sample_data_t node3_sys_sample = sample_container_create_system_data(
			  imudummydata[3], IMU_DATA_SIZE, optional_sample_3, s_imu,3);

	  for(uint8_t i = 1; i<100; i++){
		  sample_container_push(10*i, 10*i, MASTER_INDEX, &master_sys_sample);
		  if(i%2 == 0){
			  sample_container_push(10*(i-1), 10*i, 1, &node1_sys_sample);
			  sample_container_push(10*(i-1), 10*i, 2, &node2_sys_sample);
			  sample_container_push(10*(i), 10*i, 1, &node1_sys_sample);	// change to push name
			  sample_container_push(10*(i), 10*i, 2, &node2_sys_sample);
		  }
		  if(i%3 == 0){
			  sample_container_push(10*(i-2), 10*i, 3, &node3_sys_sample);
			  sample_container_push(10*(i-1), 10*i, 3, &node3_sys_sample);
			  sample_container_push(10*i, 10*i, 3, &node3_sys_sample);
		  }
		  for(sample_container_ptr = sample_container_pop();
				  sample_container_ptr!=NULL;
				  sample_container_ptr = sample_container_pop()){

			  memcpy(&sample_container_current, sample_container_ptr, sizeof(sample_dataset_t)); // todo: not ok: return size of pointer and copy properly
			  printf("pop %d systems_Data ", sample_container_current.counter);
			  for(uint8_t j= 0; j< sample_container_get_nr_systems(); j++){
				  printf(" %d ", sample_container_current.system[j]->innerTimer);
				  if(sample_container_current.globaltime - sample_container_current.system[j]->innerTimer > 5)
					  printf(" old sample for id %d", sample_container_current.system[j]->system_id);
			  }
			  printf(" \n");

			  for(uint8_t j = 0; j< sample_container_get_nr_systems(); j++){
				  if(memcmp(sample_container_current.system[j]->buffer,  imudummydata[j], IMU_DATA_SIZE) !=0 ){
					  printf("Error in system %d\n", j);
					  err_ctn[j]++ ;
				  }
			  }
		  }
	  }
		printf("Nr packages tx with < existing systems ");
		for ( uint8_t j = 0; j< NR_SYSTEMS; j++)
			printf(" %d ", err_ctn[j]);}

int main(int argc, char **argv) {

	uint8_t sample_container_size = 12;
	uint8_t nr_systems = 5;
	bool samples_optional =false;

	// all systems not optional but one
	//sample container size 12
	//first_test(sample_container_size, nr_systems, samples_optional);

	//todo: both test are nor working simultaneously-> clear the sample_container is needed

	uint8_t max_time_diff_ms = 15;
	uint8_t max_writing_sample_container = 15; // number of times that we write in the sample_container //in for
	uint8_t pop_concurrency = 2;     // how often do we want to pop
	uint8_t concurrency_1 = 1 ;         // how often do we write in pos 1
	first_test(10,3,1);
	test(sample_container_size, max_time_diff_ms, samples_optional, max_writing_sample_container, 2, concurrency_1 );

	//do_nothing();
	return 0;
}
