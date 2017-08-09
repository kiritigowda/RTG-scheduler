/*
* RTGS.h
*      Author: Kiriti Nagesh Gowda
*/

#ifndef RTGS_H
#define RTGS_H

#if _WIN32
#include <Windows.h>
#define strcasecmp strcmp
#else
#include <sys/time.h>
#include <strings.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define DEBUG_INFO 1				// debug mode information
#define DEBUG_MESSAGES 0			// detailed debug messages

#define MAX_GPU_PROCESSOR 14		// total streaming multi-processors available on the GPU
#define MAX_KERNELS 100				// max Kernels needed to be scheduled
#define PROCESSOR_LIMIT 10			// ALAP Processor Limit
#define MAX_RUN_TIME 1000			// MAX RUN TIME TO VERIFY -- TBD

#define MULTIPLE_KERNELS_SCHEDULED -99 // multiple kerenls scheduled at a given time

/*! \brief A formal status type with known fixed size.
* \see RTGS_status_e
*/
typedef int RTGS_Status;

/*! \brief The enumeration of all status codes.
* \see rtgs_status.
*/
enum RTGS_status_e {
	RTGS_ERROR_INVALID_PARAMETERS = -9,		/*!< \brief Indicates that the supplied parameter information does not match the kernel_info_list. */
	RTGS_ERROR_NO_MEMORY = -8,				/*!< \brief Indicates that an internal or implicit allocation failed. Typically catastrophic. */
	RTGS_ERROR_NO_RESOURCES = -7,			/*!< \brief Indicates that an internal or implicit resource can not be acquired (not memory). */
	RTGS_ERROR_NOT_COMPATIBLE = -6,			/*!< \brief Indicates that the attempt to link two parameters together failed due to type incompatibilty. */
	RTGS_ERROR_NOT_ALLOCATED = -5,			/*!< \brief Indicates to the system that the parameter must be allocated by the system.  */
	RTGS_ERROR_NOT_SUFFICIENT = -4,			/*!< \brief Indicates that the given an insufficient number of required parameters. */
	RTGS_ERROR_NOT_SUPPORTED = -3,			/*!< \brief Indicates that the requested set of parameters produce a configuration that cannot be supported */
	RTGS_ERROR_NOT_IMPLEMENTED = -2,		/*!< \brief Indicates that the requested function is missing. */
	RTGS_FAILURE = -1,						/*!< \brief Indicates a generic error code, used when no other describes the error. */
	RTGS_SUCCESS = 0,						/*!< \brief No error. */
};

/*! \brief The enumeration of all schedule methods.
*/
enum RTGS_schedule_method_e {
	RTGS_SCHEDULE_METHOD_IMMEDIATE = 0,
	RTGS_SCHEDULE_METHOD_AEAP = 1,
	RTGS_SCHEDULE_METHOD_ALAP = 2,
	RTGS_SCHEDULE_METHOD_NOT_DEFINED = 99,
};

/* Kernel Info Structure */
struct kernel_information {
	int processor_req;				//Processors needed
	int execution_time;				//Execution time
	int deadline;					//Deadline
	int latest_schedulable_time;	//Latest schedule time
};
//! \brief kernel_info_list info structure
typedef struct kernel_information kernelInfo;

/* scheduled kernel linked list*/
struct linked_list {
	int kernel_number;
	int kernel_release_time;
	int processor_release_time;
	int processors_allocated;
	int schedule_method;
	int data;
	struct linked_list* next;
	struct linked_list* kernel_next;
};
//! \brief kernel scheduled info
typedef struct linked_list scheduledNode;

// Backup list to revert the processor actions
struct list {
	int kernel_number;
	int data;
	int processor_release_time;
	int processors_allocated;
	struct list* next;
};
//! \brief backup kernel scheduled info
typedef struct list backup_list;

// global variables
int GLOBAL_GPU_KERNELS;
int GLOBAL_CPU_KERNELS;
int GLOBAL_RELEASE_TIME[MAX_RUN_TIME];
backup_list* GLOBAL_ALAP_LIST;

//! \brief RTG-scheduler main function
int scheduler_main(char *kernelFilename, char *releaseTimeFilename, int schedulerMode);

//! \brief RTG-scheduler Mode-1 function
int RTGS_mode_1(char *kernelFilename, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-2 function
int RTGS_mode_2(char *kernelFilename, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-3 function
int RTGS_mode_3(char *kernelFilename, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-4 function
int RTGS_mode_4(char *kernelFilename, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-4 function
int RTGS_mode_5(char *kernelFilename, char *releaseTimeFilename);

//! \brief function to get kernel info from the input file
int get_kernel_information(kernelInfo *kernelInfoList, const char *kernelFilename);

//! \brief function to get kernel release time info from the input file
int get_kernel_release_times(const char *releaseTimeFilename);

//! \brief function to retrieve processors from kernels which complete thier execution
int Retrieve_processors(int present_time, int processors_available, 
		scheduledNode** processor_alloc_list);

//! \brief function to release kernels for exection at the scheduled time
int Dispatch_queued_kernels(int present_time, int processors_available, 
		scheduledNode** kernel_queue_list, scheduledNode **processor_alloc_list);

//! \brief Function to add future kernel_info_list releases and arrange kernel_info_list execution times in ascending order
void Queue_kernel_execution(int processorReleased, int processorReleaseTime, int presentTime, 
		int scheduleMethod, int kernelNumber, scheduledNode **processorAllocList);

//! \brief Function to
int Kernel_book_keeper(kernelInfo* kernel_info_list, int kernel_number, int processors_available, int present_time, 
		scheduledNode **processor_alloc_list, scheduledNode **kernel_queue_list);

//! \brief Function to
int Processors_unavailable(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available,
		scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list);

//! \brief Function to
void Kernel_queue_handler(int processorReleased, int kernel_release_time, int processor_release_time, 
		int schedule_method, int kernel_number, scheduledNode **kernel_queue_list);

//! \brief Function to
int AEAP(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, 
		scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list);

//! \brief Function to
int AEAP_ALAP_improve(kernelInfo *kernel_info_list, int kernel_release_time, int present_time, int processors_available,
		scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list);

//! \brief Function to
int AEAP_advanced(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, 
		scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list);

//! \brief Function to
int ALAP(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available,
		scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list);

//! \brief Function to
int ALAP_advanced(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, 
		scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list);

//! \brief Function to
int ALAP_improve(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available,
		scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list);

/***************************************************************
					helper functions 
****************************************************************/
//! \brief clock counter function
int64_t RTGS_GetClockCounter();

//! \brief clock frequency function
int64_t RTGS_GetClockFrequency();

// list functions
scheduledNode* insert(scheduledNode* head, scheduledNode* data);
scheduledNode* ascending_insert(scheduledNode* head, int data, int processor_release_time, 
		int processorReleased, int kernel_number, int schedule_method);
scheduledNode* position_insert(scheduledNode* head, scheduledNode* data, int position);
scheduledNode* position_delete(scheduledNode* head, int position);
scheduledNode* reverse(scheduledNode* head);
scheduledNode* remove_recurring_node(scheduledNode* head);
scheduledNode* clean_node(scheduledNode* head);
backup_list* insert_list(backup_list* head, int data);
backup_list* clean_list(backup_list* head);
backup_list* position_delete_list(backup_list* head);
backup_list* insert_ALAP_list(backup_list* head, int kernel_release_time, 
		int processor_release_time, int processors_allocated, int kernel_number);
void print(scheduledNode* head);
void Kernel_queue_print(scheduledNode* head);
void R_print(scheduledNode* head);

#endif /* RTGS_H */
