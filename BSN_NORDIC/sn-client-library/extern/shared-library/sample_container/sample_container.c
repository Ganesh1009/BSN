/*
 * sample_container.c
 *
 *  Created on: Apr 21, 2020
 *      Author: sanchez
 */

#include "sample_container.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMP_CONTAINER_SIZE 	(20)
#define MAX_DIFF_IN_MS 			(50)
#define INVALID_SYSTEM_ID 		(0xfe)

typedef struct sample_container_s {
	uint8_t rIndex;
	uint8_t wIndex;
	uint8_t datasets_size;
	sample_dataset_t** datasets;
	uint8_t systems_id_list[MAX_NR_SYSTEM];
	uint8_t systems_id_count;
	uint8_t max_time_diff_ms;
} sample_container_t;

static sample_container_t m_sample_container={
	.rIndex = 0,
	.wIndex = 0,
	.datasets_size = 0,
	.datasets = NULL,
	.systems_id_list = {INVALID_SYSTEM_ID},
	.systems_id_count=0,
	.max_time_diff_ms=MAX_DIFF_IN_MS
};


uint8_t sample_container_get_size(){
	return m_sample_container.datasets_size;
}

void sample_container_set_max_time_diff_ms(uint8_t max_time){
	m_sample_container.max_time_diff_ms = max_time;
}

uint8_t sample_container_get_nr_systems(){
	return m_sample_container.systems_id_count;
}

int sample_container_init(uint8_t containersize){

	m_sample_container.datasets_size = containersize;

	m_sample_container.datasets = malloc(sizeof(sample_dataset_t*)*m_sample_container.datasets_size);

	if(m_sample_container.datasets == NULL)
		return -1; // not enough space for the sample_container_ptr
	memset(m_sample_container.systems_id_list, INVALID_SYSTEM_ID, sizeof(m_sample_container.systems_id_list));
	int i=0;
	for (i = 0; i < m_sample_container.datasets_size; ++i) {
		//alloc
		m_sample_container.datasets[i]=malloc(sizeof(sample_dataset_t));
		//check
		if(m_sample_container.datasets[i] == NULL){
			printf("Not enough space for sample_container_ptr");
    		return -1-i;
		}
	}

	return i;

}

sample_data_t sample_container_create_system_data(uint8_t* buffer, uint16_t buffer_size, uint8_t sample_optional, enum sample_type_t sample_type, uint8_t id){
	sample_data_t sampledata= {0};
	sampledata.buffer = buffer;
	sampledata.buffer_length=buffer_size;
	sampledata.buffer_size=buffer_size;
	sampledata.sample_optional=sample_optional;
	sampledata.sample_type=sample_type;
	sampledata.valid = false;
	sampledata.system_id = id;
	return sampledata;
}


/**
 * @brief Initializes a system_data pointer to save each system_sample received
 * It allocates memory space
 * @param system_Data to use for initializing the system
 * @return a point to the created system_data, NULL if it could not be created
 */
static sample_data_t* sample_container_init_sample_data(sample_data_t* system_data);
/**
 * brief put the system data into the given position in the sample container
 * @param sys_data_ptr to be copied in the sample_container
 * @param write_position postion in sample_container
 * @param system_index corresponds to the system id using the function (master, node 1,2..sensor 10,12..)
 * @return err_msg of put result
 */
static int32_t sample_container_write_sample_data_at_index(uint8_t write_position, sample_data_t* sys_data_ptr, uint8_t system_index );


sample_data_t* sample_container_init_sample_data(sample_data_t* system_data){
	if(system_data == NULL) {
		printf("invalid system_data\n");
		return NULL;
	}
	sample_data_t* new_system_data;
	new_system_data = malloc(sizeof(sample_data_t));
	if(new_system_data == NULL){
		printf("Error creating new_system_Data\n");
		return NULL;
	}
	new_system_data->buffer = malloc(system_data->buffer_size);
	new_system_data->buffer_size = system_data->buffer_size;
	new_system_data->sample_optional = system_data->sample_optional;
	new_system_data->sample_type = system_data->sample_type;
	return new_system_data;
}

int8_t find_match_in_container(uint32_t sample_time){
	int8_t indexAss= -1;
	int8_t matched_index = -1;
	int32_t minDiff = 0xFFFF;
	uint8_t sampleContainer_size = sample_container_get_size();
    int32_t minDiffArray;

    uint8_t i = m_sample_container.rIndex;
    do{
    	minDiffArray = abs(m_sample_container.datasets[i]->globaltime - sample_time); // (sample_time - global_timer && < sample_time)
    	if(minDiffArray < minDiff && minDiffArray< m_sample_container.max_time_diff_ms){
    		matched_index = i;
    		minDiff = minDiffArray;
    	}else if(minDiffArray > minDiff){
    		indexAss = matched_index;
    	}
    	i++;
    	if (i == m_sample_container.datasets_size) i = 0;
    }while(i != m_sample_container.wIndex);

	if(indexAss != matched_index && indexAss !=-1){
		printf("find_match_jump error %d ", indexAss);
	}
	return matched_index;
}

void increment_sample_cont_xPtr(uint8_t *sample_cont_xPtr){
	(*sample_cont_xPtr)++;
	if((*sample_cont_xPtr) == sample_container_get_size())
		*sample_cont_xPtr = 0;
}

uint8_t system_index_assignment(uint8_t system_id, sample_data_t* sample_data_ptr){

	for(uint8_t i=0; i<MAX_NR_SYSTEM; i++){   //sytem_id is not in systems_id_list
		if(m_sample_container.systems_id_list[i] == system_id)
			return i;
	}
	m_sample_container.systems_id_list[m_sample_container.systems_id_count] = system_id;
	for(uint8_t i = 0; i < sample_container_get_size() ;i++){
		m_sample_container.datasets[i]->system[m_sample_container.systems_id_count] =  sample_container_init_sample_data(sample_data_ptr);
	}
	m_sample_container.systems_id_count++;
	return m_sample_container.systems_id_count-1;
}

uint32_t sample_container_push(uint32_t time, uint32_t rtc_time, uint8_t system_id, sample_data_t* sample_data_ptr ){
	if (sample_data_ptr == NULL)
		printf("Not valid sys_data_ptr");
	sample_data_ptr->innerTimer = time;
    uint8_t system_index = system_index_assignment(system_id, sample_data_ptr);
	if(system_index == MASTER_INDEX){
		//sample_container_pop();   		// should be transmit what is here to the outside or we should wait for a pop?
		//memset ( sample_container_ptr [sample_cont_wPtr]->system , 0 , sizeof(sample_container_t));
		m_sample_container.datasets[m_sample_container.wIndex]->globaltime = time;
		m_sample_container.datasets[m_sample_container.wIndex]->rtctime = rtc_time;
		m_sample_container.datasets[m_sample_container.wIndex]->counter = 1;
		m_sample_container.datasets[m_sample_container.wIndex]->validity_f = 1;
		//all systems need to be des-validated
		for(uint8_t j =1; j< sample_container_get_nr_systems(); j++){
			if(m_sample_container.datasets [m_sample_container.wIndex]->system[j] != NULL)
				m_sample_container.datasets [m_sample_container.wIndex]->system[j]->valid = false;
		}
		sample_container_write_sample_data_at_index(m_sample_container.wIndex, sample_data_ptr, system_index);
		increment_sample_cont_xPtr(&m_sample_container.wIndex);

	}else{

		int8_t sample_cont_matchPtr = find_match_in_container(time);
		if(sample_cont_matchPtr >= 0){
			m_sample_container.datasets [sample_cont_matchPtr]->counter++;
			return sample_container_write_sample_data_at_index(sample_cont_matchPtr, sample_data_ptr, system_index);
		}else{
			printf("No match found\n");
			//return err;
		}
	}
	return 0;
}

bool check_validity(sample_dataset_t*  sample_dataset ){
    for (uint8_t i = 0; i< sample_container_get_nr_systems() ; i++){
    	if(sample_dataset->system[i] == NULL){
        	printf("check_validity: system in sample_container not initialized\n");
        	return false;
    	}else if(!sample_dataset->system[i]->sample_optional &&
    			!sample_dataset->system[i]->valid){
        	printf("check_validity: sample % d not optional and not valid -> don'T tx\n", i);
    		return  false;
    	}
    }
    return true;
}


int32_t sample_container_write_sample_data_at_index(uint8_t write_position, sample_data_t* sample_data_ptr, uint8_t system_index ){
    //check the given position, is it valid?
	//does the specific system has the data allocated?
    if(m_sample_container.datasets == NULL || m_sample_container.datasets[write_position] == NULL){
    	printf("put: sample_container not initialized\n");
    	//should we allocate? ->NO
    	return -1;
    }
    if(m_sample_container.datasets[write_position]->system[system_index] == NULL){
    	//systems_count++;
    	printf("put: system in sample_container with index %d not initialized\n", system_index);
    	m_sample_container.datasets[write_position]->system[system_index] = sample_container_init_sample_data(sample_data_ptr);
    	if(m_sample_container.datasets[write_position]->system[system_index] == NULL)
    	{
        	printf("failed to init_sample_data\n");
        	return -2;
    	}
    }
    //add length check
    if(m_sample_container.datasets[write_position]->system[system_index]->buffer_size < sample_data_ptr->buffer_length){
    	printf("buffer in datasets too small\n");
    	return -3;
    }
    memcpy(m_sample_container.datasets[write_position]->system[system_index], sample_data_ptr, sizeof(sample_data_ptr));
    memcpy(m_sample_container.datasets[write_position]->system[system_index]->buffer,
    		sample_data_ptr->buffer, sample_data_ptr->buffer_length);
    m_sample_container.datasets[write_position]->system[system_index]->valid = true;
	return 0;
}

sample_dataset_t* sample_container_pop (){	//pop_function
	sample_dataset_t*  return_sample_dataset_ptr = m_sample_container.datasets[m_sample_container.rIndex];
	if (return_sample_dataset_ptr!= NULL){
		if(return_sample_dataset_ptr->validity_f && check_validity(return_sample_dataset_ptr)){
			increment_sample_cont_xPtr(&m_sample_container.rIndex);
			return_sample_dataset_ptr->validity_f = false;		// once tx -> sample_container_local not valid
			return return_sample_dataset_ptr;
		}
	}
	return NULL;
}


void do_nothing (){
	printf("nr of systems %d %ld %ld\n", m_sample_container.systems_id_count, sizeof(sample_data_t), sizeof(sample_dataset_t));
}

