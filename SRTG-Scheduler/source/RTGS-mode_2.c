/*
* RTGS-mode_2.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/**********************************************************************************************************
MODE 2 Helper Functions
***********************************************************************************************************/
static int Mode_2_AEAP
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int localProcessors = 0, job_release_time = 0;
	genericBackupNode *processorsDistList = NULL;
	localProcessors = processors_available;
	processorsDistList = insert_node(processorsDistList, processors_available);
	processors_available = 0;
	scheduledResourceNode* localProcessorsAllocatedList = *processorsAllocatedList;

	while (localProcessorsAllocatedList != NULL)
	{
		if ((localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
		{
			int AEAP_count = 0;
			scheduledResourceNode*temp1 = *processorsAllocatedList;
			genericBackupNode* temp2 = processorsDistList;
			while (temp2 != NULL)
			{
				if (AEAP_count == 0) {
					processors_available = temp2->data;
					temp2 = temp2->next;
				}
				else {
					temp1->processors_allocated = temp2->data;
					temp1 = temp1->next;
					temp2 = temp2->next;
				}
				AEAP_count++;
			}
			// TBD:: Return Job to CPU - Function to send Job to CPU execution 
			jobAttributesList[jobNumber].schedule_hardware = 2;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].completion_time = -1;
			jobAttributesList[jobNumber].scheduled_execution = -1;
			GLOBAL_CPU_JOBS++;
			processorsDistList = clean_list(processorsDistList);
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("MODE-2 AEAP:: The Job:%d Cannot be scheduled AEAP\n", jobNumber);
				printf("MODE-2 AEAP:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
			}
			break;
		}
		else
		{
			localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
			if (localProcessors >= jobAttributesList[jobNumber].processor_req)
			{
				localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
				job_release_time = localProcessorsAllocatedList->processor_release_time;
				processorsDistList = clean_list(processorsDistList);

				int processorsInUse = jobAttributesList[jobNumber].processor_req;
				int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
				int presentTime = present_time;
				int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
				jobAttributesList[jobNumber].schedule_hardware = 1;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].scheduled_execution = job_release_time;
				jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
				GLOBAL_GPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("MODE-2 AEAP:: The Job:%d scheduled AEAP\n", jobNumber);
					printf("MODE-2 AEAP:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
				}
				queue_job_execution(processorsInUse, processor_release_time, presentTime,
					schedule_method, jobNumber, processorsAllocatedList);
				job_queue_handler(processorsInUse, job_release_time, presentTime,
					schedule_method, jobNumber, jobScheduledQueueList);
				break;
			}
			else
			{
				processorsDistList = insert_node(processorsDistList, localProcessorsAllocatedList->processors_allocated);
				localProcessorsAllocatedList->processors_allocated = 0;
			}
		}
		localProcessorsAllocatedList = localProcessorsAllocatedList->next;
	}
	return processors_available;
}

static int Mode_2_Processors_Unavailable
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	processors_available = Mode_2_AEAP(jobAttributesList, jobNumber, present_time, processors_available, processorsAllocatedList, jobScheduledQueueList);
	return processors_available;
}

static int Mode_2_book_keeper
(
	jobAttributes* jobAttributesList,
	int jobNumber,
	int processors_available,
	int present_time,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int processorsInUse = 0, processor_release_time = 0;
	int presentTime = present_time;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode-2 Book Keeper:: Job::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			jobNumber, jobAttributesList[jobNumber].processor_req, jobAttributesList[jobNumber].execution_time,
			jobAttributesList[jobNumber].deadline, jobAttributesList[jobNumber].latest_schedulable_time);
	}
	// If processors available is greater than the required processors by the jobAttributesList
	if (jobAttributesList[jobNumber].processor_req <= processors_available)
	{
		if (jobAttributesList[jobNumber].execution_time + presentTime <= jobAttributesList[jobNumber].deadline) {
			processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
			processorsInUse = jobAttributesList[jobNumber].processor_req;
			processor_release_time = jobAttributesList[jobNumber].execution_time + presentTime;
			schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
			// Job call for the GPU to handle the given Jobs and number of blocks
			queue_job_execution(processorsInUse, processor_release_time, presentTime,
				schedule_method, jobNumber, processorsAllocatedList);
			jobAttributesList[jobNumber].schedule_hardware = 1;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].scheduled_execution = present_time;
			jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
			GLOBAL_GPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode-2 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
		}
		else {
			jobAttributesList[jobNumber].schedule_hardware = 2;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].completion_time = -1;
			jobAttributesList[jobNumber].scheduled_execution = -1;
			GLOBAL_CPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode-2 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
				printf("Mode-2 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
			}
		}
	}
	else
	{
		// Schedule the job to be released in a future time
		processors_available = Mode_2_Processors_Unavailable(jobAttributesList, jobNumber,
			present_time, processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 2 - As Early As Possible mode->AEAP
***********************************************************************************************************/
int RTGS_mode_2(char *jobsListFileName, char *releaseTimeFilename)
{
	jobAttributes jobAttributesList[MAX_JOBS] = {{0}};
	jobReleaseInfo releaseTimeInfo[MAX_JOBS] = {{0}};
	scheduledResourceNode *processorsAllocatedList = NULL;
	scheduledResourceNode *jobScheduledQueueList = NULL;

	// global variable initialize
	GLOBAL_GPU_JOBS = 0;
	GLOBAL_CPU_JOBS = 0;
	GLOBAL_preScheduleList = NULL;

	int processorsAvailable = GLOBAL_MAX_PROCESSORS;
	int jobNumber = 0;

	int kernelMax = get_job_information(jobAttributesList, jobsListFileName);
	if (kernelMax <= RTGS_FAILURE) { printf("ERROR - get_job_information failed with %d code\n", kernelMax); return  RTGS_FAILURE; }
	int maxReleases = get_job_release_times(releaseTimeInfo, releaseTimeFilename);
	if (maxReleases <= RTGS_FAILURE) { printf("ERROR - get_job_release_times failed with %d code\n", maxReleases); return  RTGS_FAILURE; }

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("\n**************** The GPU Scheduler will Schedule %d Jobs ****************\n", kernelMax);
	}

	int numReleases = 0;
	for (int present_time = 0; present_time < MAX_RUN_TIME; present_time++)
	{
		// Freeing-up processors
		processorsAvailable = Retrieve_processors(present_time, processorsAvailable, &processorsAllocatedList);
		if (processorsAvailable < 0) { printf("Retrieve_processors ERROR- GCUs Available:%d\n", processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }
		processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &jobScheduledQueueList, &processorsAllocatedList);
		if (processorsAvailable < 0) { printf("Dispatch_queued_kernels ERROR - GCUs Available:%d\n", processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }

		if (releaseTimeInfo[numReleases].release_time == present_time) {

			if (releaseTimeInfo[numReleases].num_job_released == 1)
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("\nRTGS Mode 2 -- Total GCUs Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 2 -- Job-%d Released\n", jobNumber);
				}
				jobAttributesList[jobNumber].release_time = present_time;
				// handling the released jobAttributesList by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Mode_2_book_keeper(jobAttributesList, jobNumber, processorsAvailable, present_time,
					&processorsAllocatedList, &jobScheduledQueueList);
				int64_t end_t = RTGS_GetClockCounter();
				int64_t freq = RTGS_GetClockFrequency();
				float factor = 1000.0f / (float)freq; // to convert clock counter to ms
				float SchedulerOverhead = (float)((end_t - start_t) * factor);
				jobAttributesList[jobNumber].schedule_overhead = SchedulerOverhead;
				jobNumber++;
			}
			else if (releaseTimeInfo[numReleases].num_job_released == 2)
			{
				int k1 = jobNumber; jobNumber++;
				int k2 = jobNumber; jobNumber++;
				jobAttributesList[k1].release_time = present_time;
				jobAttributesList[k2].release_time = present_time;

				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("\nRTGS Mode 2 -- Total GCUs Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 2 -- Job-%d Released\n", k1);
					printf("RTGS Mode 2 -- Job-%d Released\n", k2);
				}
				if (jobAttributesList[k1].deadline <= jobAttributesList[k2].deadline)
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
				}
				else
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
				}
			}
			else { printf("RTGS Mode 2 ERROR --  RTGS_ERROR_NOT_IMPLEMENTED\n"); return RTGS_ERROR_NOT_IMPLEMENTED; }

			numReleases++;
			if (numReleases > maxReleases) {
				printf("RTGS Mode 2 ERROR --  Job Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0) {

		if (GLOBAL_RTGS_DEBUG_MSG) {
			printf("\n******* Scheduler Mode 2 *******\n");
			printf("GCUs Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs    -- %d\n", GLOBAL_GPU_JOBS);
			printf("	Jobs Sent Back To CPU -- %d\n", GLOBAL_CPU_JOBS);
		}

		if (RTGS_PrintScheduleSummary(2, kernelMax, jobAttributesList)) {
			printf("\nSummary Failed\n");
		}

		if ((kernelMax != (GLOBAL_GPU_JOBS + GLOBAL_CPU_JOBS)) || processorsAvailable != GLOBAL_MAX_PROCESSORS) {
			return RTGS_FAILURE;
		}

		for (int j = 0; j <= kernelMax; j++) {
			jobAttributesList[j].processor_req = jobAttributesList[j].deadline = jobAttributesList[j].execution_time = jobAttributesList[j].latest_schedulable_time = 0;
		}
		kernelMax = 0; maxReleases = 0; jobNumber = 0; GLOBAL_GPU_JOBS = 0; GLOBAL_CPU_JOBS = 0;
	}

	if (processorsAllocatedList) {
		printf("\nERROR -- processorsAllocatedList Failed %d\n", processorsAllocatedList == NULL ? 0 : 1);
		return RTGS_FAILURE;
	}
	return RTGS_SUCCESS;
}