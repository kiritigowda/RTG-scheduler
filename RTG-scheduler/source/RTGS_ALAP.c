/*
* RTGS-ALAP.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP
(
	kernelInfo *kernel_info_list,
	int kernel_number, 
	int present_time,
	int processors_available, 
	scheduledNode ** processor_alloc_list, 
	scheduledNode **kernel_queue_list
) 
{
	PROFILER_START(SRTG, ALAP)
	int Pro = 0, kernel_release_time, processor_release_time = 0, processors_allocated = 0;
	backup_list *P_Given_list = NULL;
	processors_allocated = kernel_info_list[kernel_number].processor_req;
	processor_release_time = kernel_info_list[kernel_number].deadline;
	kernel_release_time = processor_release_time - kernel_info_list[kernel_number].execution_time;
	Pro = processors_available;

	if (Pro >= kernel_info_list[kernel_number].processor_req)
	{
		int processorReleased = kernel_info_list[kernel_number].processor_req;
		int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;
		GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
		printf("ALAP: Kernel:%d scheduled\n", kernel_number);
		printf("ALAP: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
		GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, 
			processors_allocated, kernel_number);
		Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, 
			schedule_method, kernel_number, kernel_queue_list);
		PROFILER_STOP(SRTG, ALAP)
		return processors_available;
	}
	P_Given_list = insert_ALAP_list(P_Given_list, kernel_release_time, processor_release_time,
		processors_available, kernel_number);
	scheduledNode* temp = *processor_alloc_list;

	while (temp != NULL) 
	{
		if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) 
	{
			int count = 0;
			scheduledNode*temp1 = *processor_alloc_list;
			backup_list* temp2 = P_Given_list;

			while (temp2 != NULL)
			{
				if (count == 0)
					temp2 = temp2->next;
				else {
					temp1->kernel_release_time = 0;
					temp1 = temp1->next;
					temp2 = temp2->next;
				}
				count++;
			}
			P_Given_list = clean_list(P_Given_list);
			GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
			printf("ALAP: The Kernel:%d Cannot be scheduled\n", kernel_number);
			printf("ALAP: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
			PROFILER_STOP(SRTG, ALAP)
			return processors_available;
		}
		else 
		{
			Pro = Pro + temp->processors_allocated;
			if (Pro >= kernel_info_list[kernel_number].processor_req) 
			{
				temp->kernel_release_time = kernel_release_time;
				P_Given_list = clean_list(P_Given_list);
				int processorReleased = kernel_info_list[kernel_number].processor_req;
				int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;
				GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
				printf("ALAP: Kernel:%d scheduled\n", kernel_number);
				printf("ALAP: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
				GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time,
					processors_allocated, kernel_number);
				Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, 
					schedule_method, kernel_number, kernel_queue_list);
				PROFILER_STOP(SRTG, ALAP)
				return processors_available;
			}
			else if (Pro < kernel_info_list[kernel_number].processor_req) 
			{
				P_Given_list = insert_ALAP_list(P_Given_list, kernel_release_time, processor_release_time, temp->processors_allocated, kernel_number);
				temp->kernel_release_time = kernel_release_time;
				temp = temp->next;
			}
		}
	}
	if (temp == NULL && P_Given_list != NULL)
	{
		int count = 0;
		scheduledNode*temp1 = *processor_alloc_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL) 
		{
			if (count == 0) {
				temp2 = temp2->next;
			}
			else {
				temp1->kernel_release_time = 0;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}
			count++;
		}
		P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
		printf("ALAP: The Kernel:%d Cannot be scheduled AEAP", kernel_number);
#endif
	}
	PROFILER_STOP(SRTG, ALAP)
	return processors_available;
}