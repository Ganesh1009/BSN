/*
 * sample_container.h
 *
 *  Created on: Apr 21, 2020
 *      Author: sanchez
 */

#ifndef SAMPLE_CONTAINER_H_
#define SAMPLE_CONTAINER_H_

#include <stdint.h>
#include <stdbool.h>

#define MASTER_INDEX   0
#define MAX_NR_SYSTEM    30  	//30 systems including all possible sensors nodes: imu, respiration, environmental, insoles?

enum sample_type_t {
  s_ble_node,
	s_imu,
  s_environmental,
	s_HRS,
	s_insoles,
	s_respiration,
  s_kinematics,
	s_MaxType
};

#pragma pack(push, 1)
typedef struct sample_data_s { 		//when we receive a new system, we need to allocate memory for it
  uint8_t system_id;
  uint32_t innerTimer;
  uint16_t buffer_size;				// for allocation purposes
  uint16_t buffer_length;  			// actual length of the buffer
  uint8_t sample_optional : 1;		// is it a critical data? (HRS is not, imu_data it is)
  uint8_t valid : 1;		// is it a critical data? (HRS is not, imu_data it is)
  enum sample_type_t sample_type: 6;		// it indicates the sample type: is it imu_data? insole? ble_node? HRS)
  uint8_t* buffer; 			        // when initializing: if(==0 || NULL) -> allocate memory.
} sample_data_t;				    // in DFKI app, buffer will point to ble_message_t
#pragma pack(pop)
//  size(of) system_data_t = 24

#pragma pack(push, 1)
typedef struct sample_dataset_s {
  uint32_t globaltime;
  uint32_t rtctime;
  bool validity_f;         // validity flag (was it already tx?)
  uint8_t counter;		   // used for debugging in sort function= it counts the writing in each sample_container element
  sample_data_t* system[MAX_NR_SYSTEM]; //includes nodes/Bridge_Ble_Nodes via SPINAL
  //would system also include Ble_nodes with its nodes via BLE ( will it split the nodes or put each as a system)
} sample_dataset_t;
#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif

void do_nothing ();

/*
 * @brief set maximun time diff en ms for finding the match in sample_container
 * @param max_time to be settled
 */

void sample_container_set_max_time_diff_ms(uint8_t max_time);

/**
 * @brief init the container
 * @param containersize
 */
int sample_container_init(uint8_t containersize);

/**
 * @brief create system data object without allocating memory
 * @params are directly the element of the system_data_t object
 * @return an initialized system_data_t object
 */
sample_data_t sample_container_create_system_data(uint8_t* buffer, uint16_t buffer_size, uint8_t sample_optional, enum sample_type_t sample_type, uint8_t id);


/**
 * @brief put the system_data into the sample_container
 * @param time corresponds to the sync shared timer
 * @param rtc_time is the rtc local counter used as a local timer
 * @param system_index corresponds to the system id using the function (master, node 1,2..sensor 10,12..)
 * @param sys_data_ptr is the actual system_data to be saved in the sample_container
 * @return err_msg if sys_data was sort properly
 */
uint32_t sample_container_push(uint32_t time, uint32_t rtc_time, uint8_t system_index, sample_data_t* sample_data_ptr);

/**
 * @brief pop the next valid sample from the sample_Container
 * caller should save a copy of the returned pointer
 * sample_container ptr can be overwritten after this function is called
 * @return a pointer to a valid sample in the sample_container
 */
sample_dataset_t* sample_container_pop();

/**
 * @brief get max number of systems writing into the sample_container
 */
uint8_t sample_container_get_nr_systems();

#ifdef __cplusplus
}
#endif

#endif /* SAMPLE_CONTAINER_H_ */
