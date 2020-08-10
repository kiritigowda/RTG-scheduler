[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://travis-ci.org/kiritigowda/SRTG-Schedule.svg?branch=master)](https://travis-ci.org/kiritigowda/SRTG-Schedule)
[![codecov](https://codecov.io/gh/kiritigowda/SRTG-Schedule/branch/master/graph/badge.svg)](https://codecov.io/gh/kiritigowda/SRTG-Schedule)
[![CodeFactor](https://www.codefactor.io/repository/github/kiritigowda/srtg-schedule/badge)](https://www.codefactor.io/repository/github/kiritigowda/srtg-schedule)
<a href="https://codeclimate.com/github/kiritigowda/SRTG-Schedule/maintainability"><img src="https://api.codeclimate.com/v1/badges/dc43dc1b470535e927ac/maintainability" /></a>

# Dynamic Schedule Management Framework For GPUs

## Soft Real-Time GPU Scheduler

A schedule management framework for soft-real-time jobs that may be used by a CPU - GPU system designer/integrator to select, configure, and deploy a suitable architectural platform and to perform concurrent scheduling of these jobs. 

[Real-Time GPU Scheduler](RTG-scheduler) (RTG-Scheduler) is a dynamic scheduler for aperiodic soft-real-time jobs on GPU based architectures, with a simple, easy-to-use command-line interface (CLI). The SRTG-Scheduler is provided under the [MIT license](https://opensource.org/licenses/MIT). It is currently supported on Windows, Linux, and macOS platforms.

## Dynamic schedule management framework for soft-real-time jobs on GPU based architectures

<p align="left"><img width="70%" src="documents/images/readme_images/header/why_gpus.png" /></p>

### GPUs execute at higher frequencies 

* Accelerates execution of jobs allocated to it
* Improves System response time

<p align="center"><img width="70%" src="documents/images/body/flops_per_year.png" /></p>
<p align="center"> The above image depicts GFlops improvements over the years comparing CPUs and GPUs. <a href="#note1" id="note1ref"><sup>[1]</sup></a>

### GPUs are energy efficient

* Power needed for GPU to carry out an operation lesser than CPUs
* Ideal for use in real time embedded system


<p align="right"><img width="70%" src="documents/images/readme_images/header/what_are_the_challenges.png" /></p>

* Significant hardware and firmware challenges
* Executions are non-preemptive 
* Low degree of controllability of cores

<p align="left"><img width="70%" src="documents/images/readme_images/header/whats_been_done.png" /></p>

* Policies for scheduling Real-Time jobs 
* Decoding the driver
* Managing the GPU as a resource
* Targeting a Multi-GPU model

This entire body of work assumes that only one kernel may execute on a GPU at a given time(partly due to lack of hardware support)

<p align="right"><img width="70%" src="documents/images/readme_images/header/our_approach.png" /></p>

## What's the problem?

<p align="right"><img width="70%" src="documents/images/readme_images/body/serial_kernels.png" /></p>

Sending a single non-preemptive kernel on to a GPU, is under utilizing the GPU

## Solution

<p align="right"><img width="70%" src="documents/images/readme_images/body/parallel_kernels.png" /></p>

Concurrent Kernel Execution on GPU

* Safe concurrent kernels
* Performance boost
* Execution units available

<p align="left"><img width="70%" src="documents/images/readme_images/header/our_ongoing_work.png" /></p>

Aims to develop a dynamic schedule management framework for soft-real-time
jobs on GPU based architectures.

<p align="right"><img width="70%" src="documents/images/readme_images/body/RTG-Scheduler.png" /></p>

We propose to exploit this basic idea to perform coarse grained scheduling of jobs on GCUs.

<p align="right"><img width="70%" src="documents/images/readme_images/body/RTG_Scheduler_block_diagram.png" /></p>

Our work lays emphasis on minimal programmer involvement.

A dynamic schedule management framework that is responsible for

* Keeping track of current and expected GUC availability
* Determining which kernel(s) to dispatch to the GPU at a given time
* Determining how many GCUs to assign for a given kernel.

<p align="right"><img width="70%" src="documents/images/readme_images/header/summary.png" /></p>


**note:**

<a id="note1" href="#note1ref"><sup>[1]</sup></a> [FLOPs per Cycle for CPUs, GPUs and Xeon Phis](https://www.karlrupp.net/2016/08/flops-per-cycle-for-cpus-gpus-and-xeon-phis/)


