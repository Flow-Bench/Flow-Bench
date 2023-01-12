# FlowBench

## Introduction

This repository is the open source version for FlowBench. 

FlowBench is a toolkit for generating flow tables and traces. It works based on dependencies between different rules. We hope it can complement the classic ClassBench-like tools and support better controlled and more comprehensive evaluations for both existing and future algorithms.

FlowBench works based on the Directed Acyclic Graph (DAG) abstraction. Each rule in the flow table corresponds to a node in the DAG, and the edge between two nodes indicates the overlapping relationship of the corresponding rules. FlowBench aims to generate a flow table whose corresponding DAG satisfied some conditions given by the user, and allows users to customize match fields.

You can find the complete documentation in `\doc` directory.

## Installation

Requirements: C++ compiler supports C++14 standard.

```shell
cd source
g++ -std=c++14 -O2 quad_dag_generator.cpp -o quad_dag_generator
quad_dag_generator
g++ -std=c++14 -O2 rule_set_generator.cpp -o flowbench
g++ -std=c++14 -O2 trace_generator.cpp -o flowbench-trace
```

## Contact us

FlowBench is designed by the following authors:

* Zhikang Chen, Tsinghua University
* Ying Wan, Tsinghua University
* Ting Zhang, Tsinghua University
* Haoyu Song, Futurewei Technologies
* Bin Liu (corresponding author), Tsinghua University

We will regularly check and reply the issues on this repository. You can also contact us by e-mail: corresponding author *Bin Liu* (lmyujie@gmail.com) or the first author *Zhikang Chen* (clazychen@gmail.com).