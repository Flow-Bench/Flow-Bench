# FlowBench Document

## What is FlowBench?

FlowBench is a toolkit for generating flow tables and traces. It works based on dependencies between different rules. We hope it can complement the classic ClassBench-like tools and support better controlled and more comprehensive evaluations for both existing and future algorithms.

FlowBench works based on the Directed Acyclic Graph (DAG) abstraction. Each rule in the flow table corresponds to a node in the DAG, and the edge between two nodes indicates the overlapping relationship of the corresponding rules. FlowBench aims to generate a flow table whose corresponding DAG satisfied some conditions given by the user, and allows users to customize match fields.

FlowBench is to be published on INFOCOM'23. If you have questions or are interested in our details, please contact the authors *Bin Liu* (lmyujie@gmail.com) or  *Zhikang Chen* (clazychen@gmail.com).

## Why use FlowBench?

ClassBench and its improved versions, e.g. ClassBench-ng, are common tools used to generate flow tables. They follow the same design principle based on samples of real flow tables and a set of conditional probabilities derived from frequency statistics. Compared with those tools, FlowBench has the following features:

1. **Field Customization**. FlowBench allows users to specify **ANY** number of match fields with **ANY** bit width and **ANY** match type (Longest Prefix Match, Range Match, or Exact Match). IPv4, IPv6, OpenFlow and **ANY** custom protocols are supported.
2. **Table-scale Customization**. FlowBench allows users to specify **PRECISE** and **LARGE** flow table sizes, and generate the table **SWIFTLY**. A large-scale flow table with millions of rules can be generated in seconds.
3. **DAG Customization**. FlowBench allows users to specify an exact or relative value of the sum of node **DEPTHS** `D` or the total number of **EDGES** `E`, thus achieving strong control over the DAG. FlowBench provides marvelous flexibility to users. It can generate a table with either exactly no dependency or very dense dependencies.
4. **Repeatability**. FlowBench allows users to specify the random seed. Different users sharing the **SAME** set of arguments can get the **SAME** results, eliminating the effort to contact each other.

## How to install FlowBench?

FlowBench is a cross-platform C++ program that only depends on the C++ standard library. 

If your C++ compiler supports **C++17** standard (we recommend `g++ >= 7.3.0`), the `make` compilation should be simple and swift. We have verified the compilation on Windows and Ubuntu. Please contact us if there is any problem with the compilation.

## How to use FlowBench?

In this section, we will show you how to use FlowBench to generate your own flow tables and traces. 

We will use the default names `flowbench` and `flowbench-trace` to refer to the flow table generator and trace generator executable . Both of them use command-line options. You can also use the `--help` option to get a simple guide.

### Guide of Flow Table Generator

#### Overview

The table below summarizes the options provided by FlowBench's flow table generator. 

```
===================================================================================
| Options                    | Descriptions                                       |
|---------------------------------------------------------------------------------|
| -n                         | Size of flow table                                 |
| -o                         | Output file path                                   |
| -f                         | Field number                                       |
| -fw / --field-width        | Field bit widths                                   |
| -ft / --field-type         | Field match types                                  |
| -fwt / --field-weight      | Field weights                                      |
| -D / -d                    | Exact/Relative value of sum of node depths of DAG  |
| -E / -e                    | Exact/Relative value of total edges of DAG         |
| -s / --random-seed         | Random seed                                        |
| --flowbench                | Output the result in FlowBench's style (default)   |
| --classbench               | Output the result in ClassBench's style            |
| -ar / --arbitrary-range    | Enable arbitrary range feature                     |
| --dense                    | Enable dense mode                                  |
| -p / --protocol            | Enable predefined protocol                         |
===================================================================================
```

You can simply run `flowbench` with default options to get an IPv4 5-tuple flow table with 4,096 rules.

We will cover how to use these options in the following several subsections.

#### Table-scale Customization

##### Examples

`flowbench -n 4096` (to generate a flow table with 4,096 rules)

##### Description

Our experiments have shown that the time consumption of FlowBench has an approximately linear relationship with the argument given by `-n`. On our machine 1,048,576 IPv4 5-tuple rules can be generated within 2 seconds (output time is not counted). You can generate relatively large-scale flow tables with confidence.

Note that there is a limitation of the table size because of the bit widths and match types of the fields. For example, when there is only an 3-bit EM field, you can never generate 10 rules because there are only 9 different candidates (8 exact values and wildcard). In addition, FlowBench may not reach the theoretical upper bound of the table size. If the specified size is too large, FlowBench will raise an error. You can try `--dense` to enable *dense mode* (will be described in subsections below), or replace the size with a smaller one.

#### Field Customization

##### Examples

`flowbench -f 5 -fw 32 32 16 16 8 -ft LPM LPM RM RM EM` (IPv4 5-tuple)

`flowbench -f 5 -fw 128 128 16 16 8 -ft LPM LPM RM RM EM` (IPv6 5-tuple)

`flowbench -f 12 -fw 16 48 48 16 8 16 8 8 32 32 16 16 -ft EM EM EM EM EM EM EM EM LPM LPM RM RM ` (OpenFlow 1.0 12-tuple)

##### Description

Field customization includes 3 options: `-f`, `-fw`, and `-ft`. The default value is IPv4 5-tuple.

`-f` is used to specify the number of matching fields. You should specify `-f` first, and then specify `-fw` and `-ft` with the same number of arguments. FlowBench supports 3 match types: Longest Prefix Match (LPM), Range Match (RM), and Exact Match (EM).

Our experiments have shown the time consumption of FlowBench has an approximately linear relationship with the argument given by `-f`. Since FlowBench use 32-bit data to participate in operation, it may be a little slower when you specify a bit width larger than 32 (e.g. MAC addresses and IPv6 addresses). On the other hand, the match type does not have a significant impact on time.

To simplify commonly used protocols, FlowBench provides another option `-p`, or `--protocol`. You can use `-p ipv4`, `-p ipv6`, and `-p openflow1.0` to use the predefined protocols.

#### Field Weight Specification

##### Examples

`flowbench -p ipv4 -fwt 1.0 1.0 1.0 1.0 1.0` (Give the IPv4 5 fields the same weight)

##### Description

`-fwt` is used to control the probability of dependency on each field. If you give a field a larger weight, it will have more possible prefix lengths for LPM, more different intervals for RM, or more exact values in EM.  Conversely, if you set a field's weight to `0.0`, this field for every rule in the table will be wildcard. 

Note that it is the relative value rather than the absolute value that matters. For example, `-fwt 1.0 2.0` is equivalent to `-fwt 0.5 1.0` because they have the same ratio between the weights.

If `-fwt` is not specified, FlowBench will use the default weights: LPM and RM fields have weights equal to their bit widths, and EM fields have a weight equal to `1.0`. For example, `flowbench -p ipv4` is equivalent to `flowbench -p ipv4 -fwt 32 32 16 16 1`.

#### DAG Customization

##### Examples

`flowbench -D 120` (The sum of node depths is 120)

`flowbench -E 120` (The total number of edges is 120)

`flowbench -d 0.7` (The *relative* value of sum of node depths is 0.7)

##### Description

FlowBench provides `-D` and `-E` to specify the exact value of the parameters `D` and `E` of the corresponding DAG, where `D` represents the sum of node depths and `E` represents the total number of edges. For example, the DAG below has `D=4` and `E=3`.

```
A ----------> B       depth(A) = 0
|                     depth(B) = depth(C) = 1
|                     depth(D) = 2       
|                     D = 0 + 1 + 1 + 2 = 4
|                     E                 = 3
V              
C ----------> D
```

The exact value has some disadvantages. When it comes to density, DAGs with different sizes may have a significant difference, even if they have the same `D` or `E`. In order to allow users to use the same indicators when experimenting on flow tables with different sizes, FlowBench provides `-d` and `-e` to specify *relative* values of the parameters `D` and `E`. For example, `flowbench -d 0.7` means the relative value of `D` is `0.7`.

FlowBench set a baseline value `P1` related to the size of the flow table `n`, where `P1 = O(nlogn)`. If the specified relative value is `r`, FlowBench will calculate the corresponding exact value with `r * P1`. Unless limited by the bit widths of fields,  FlowBench guarantees that the flow table can be generated successfully when `r <= 1.0`. When you want a denser DAG, you can try `--dense` to enable *dense mode*, or try different random seeds if FlowBench raises an error. According to our experience, there is high risk that FlowBench cannot find a table satisfying your requirements when `r > 2.0`.

> If not limited by the bit widths, the theoretical upper bound of the parameters is `O(n^2)`, corresponding to a *complete* DAG. However, under most circumstances this bound cannot be reached because it needs too many bits.
>
> When the bit widths are taken into account, the upper bound remains an unsolved issue. We have noted that there is still a huge gap between FlowBench and the bound, because our algorithm is tree-based, thus limited by `O(nlogn)`.

You cannot specify both of `-D` and `-E`, otherwise only one of them may take effect. Similarly, you cannot specify both exact and relative values of either parameter. If no parameter is specified, FlowBench will randomly generate a DAG. 

#### Random Seed Specification

##### Examples

`flowbench -s 5489` (Use 5489 as the random seed)

##### Description

FlowBench provides `-s` to specify the random seed. 

FlowBench uses Mersenne Twister Algorithm to generate random numbers. We do not use the version in C++ STL for better repeatability on different platforms.

#### Output Specification

##### Examples

`flowbench -o out.txt` (Output the generated flow table to `out.txt `)

`flowbench --classbench` (Output in ClassBench's style)

##### Description

`-o` is used to specify the output file path. If not specified, FlowBench will generate an output file automatically in the current directory. The default file name is `n.txt` where `n` is the size of the flow table. For example, FlowBench will output a `4096.txt` if you simply enter `flowbench -n 4096`.

Apart from FlowBench's default style, we also support ClassBench's output style for better compatibility. You can use the option `--classbench` to switch to ClassBench's style, so that you can reuse your codes written for ClassBench. The table below shows the difference between the two output styles.

```
===================================================================================
| Items              | FlowBench                  | ClassBench                    |
|---------------------------------------------------------------------------------|
| Rule Symbol        | R                          | @                             |
| LPM                | 1000                       | 128.0.0.0/4                   |
| LPM (Wildcard)     | *                          | 0.0.0.0/0                     |
| RM                 | 0 : 65535                  | 0 : 65535                     |
| EM                 | 0x11                       | 0x11/0xFF                     |
| EM (Wildcard)      | *                          | 0x00/0x00                     |
===================================================================================
```

As is shown in the table, FlowBench and ClassBench share the same output style of RM fields. Wildcard LPM or EM fields are output as `*` in FlowBench's default style. Normal LPM fields are represented by their determined bits (omit the suffix of `*`), and normal EM fields are represented hexadecimally. We design the style to support arbitrarily customized protocols.

MAC addresses and IPv6 addresses have their own characteristic representations, but in order to make easier for users to parse these fields, no special handling is given to these fields in FlowBench.

#### Arbitrary Range

##### Examples

`flowbench -ar` (Enable the *arbitrary range* feature)

##### Description

*Arbitrary Range* is an optional feature supported by FlowBench. If this feature is enabled, FlowBench will generate ranges with no limitation for RM fields. By contrast, if it is disabled, FlowBench will guarantee that each range for RM fields can be converted to a prefix. For example, an 8-bit RM field with the range `0:15` can be converted to the prefix `0x00/4`, while the range `0:14` cannot be converted to only one prefix.

When `-ar` is disabled, FlowBench will treat RM fields as if they were less flexible LPM fields. Therefore, we recommend you disable this option and save 2-3 times the time of generating flow tables. 

In addition, we have noticed that there are many algorithms requiring converting RM fields to a set of LPM fields first, e.g. for TCAM-based tables. When evaluating those algorithms, the flow table will be inflated due to this conversion, which may causes other options (`-n`, `-fwt`,  `-d/-D`, and `-e/-E`) to fail. Classic ClassBench-like tools cannot handle this problem, but in FlowBench you just need to leave the `-ar` option disabled.

#### Dense Mode

##### Examples

`flowbench --dense`

##### Description

*Dense Mode* is an optional feature supported by FlowBench. If this feature is enabled, FlowBench can generate a flow table with larger size, but no longer guarantees that every rule in the table can be hit by a certain packet. For example, when there exist 3 rules, `0.0.0.0/0`, `0.0.0.0/1`, and `128.0.0.0/1`, you may find that `0.0.0.0/0` can never be hit by any packet. This situation may occur in the real flow table, but will cause some options in our *trace generator* to fail. Therefore, unless you need to generate a relatively large table with insufficient bit widths, we recommend you disable this option.

### Guide of Trace Generator

#### Overview

The table below summarizes the options provided by FlowBench's flow table generator. 

```
===================================================================================
| Options                    | Descriptions                                       |
|---------------------------------------------------------------------------------|
| -n                         | Size of trace                                      |
| -d                         | Density (Relative size) of trace                   |
| -i                         | Input file path                                    |
| -o                         | Output file path                                   |
| -f                         | Field number                                       |
| -fw / --field-width        | Field bit widths                                   |
| -ft / --field-type         | Field match types                                  |
| -rd / --rule-distribution  | Pareto parameter of rules' distribution            |
| -fd / --flow-distribution  | Pareto parameter of flows' distribution            |
| -s / --random-seed         | Random seed                                        |
| --flowbench                | Output the result in FlowBench's style (default)   |
| --classbench               | Output the result in ClassBench's style            |
| -p / --protocol            | Enable predefined protocol                         |
===================================================================================
```

You can find that `flowbench-trace` has a lot of common options with the trace generator `flowbench`: `-f`, `-fw` and `-ft` define the customized protocol, and you can also use our pre-defined protocols with `-p`.

We will cover how to use the other options in the following several subsections.

#### Trace-scale Customization

##### Examples

`flowbench-trace -n 4096` (to generate a file with 4,096 traces)

`flowbench-trace -d 10` (to generate a file with `10n` traces, where `n` is the scale of the input table)

##### Description

FlowBench provides two ways to specify the scale of the trace file. The first way is to directly give the trace count by `-n`, and the second way is to give a ratio by `-d`. For example, if the given rule set has `4,096` rules in it, and we set `-r 10`, then `flowbench-trace` will generate `40,960` traces. We call this ratio the *density* of traces, which is the average number of packets hitting each rule. This design refers to ClassBench's solution, which is conducive to comparative experiments on flow tables of different scales.

#### Spatial Locality Customization

##### Examples

`flowbench-trace -rd 1 0.1` (The rules follow a distribution $\mathrm{Pareto}(1,0.1)$)

`flowbench-trace -fd 1 0.1` (The flows follow a distribution $\mathrm{Pareto(1,0.1)}$)

##### Description

FlowBench provides two types of spatial locality control: one is the flow-level locality, which is the same as what ClassBench's trace generator provides, and the other is the rule-level locality.

If you did not enable dense mode, FlowBench guarantees that every rule can bit hit by some packets. In this case, we provides an extra option to control the rule-level spatial locality. If you enabled dense mode, the rules may not follow the given Pareto distribution properly.

> It is important to note that when rule-level locality is specified, FlowBench will take a lot of time to discretize the rules and build an isolate rule set. This step is to ensure that the generated trace hits the rules we expect, but it has a time complexity of `O(n^2 * f)`, where `n` is the rule count of the given rule set, and `f` is the field count of the user-defined protocol. 
>
> If you do not specify the rule-level spatial locality, FlowBench will enable *fast mode* and omit this step. In this case, FlowBench's trace generator will have an better efficiency similar to that of ClassBench.

#### Input Specification

##### Examples

`flowbench-trace -i 4096.txt` (The input rule set is in `4096.txt`)

##### Description

`-i` is used to specify the input file path, and it cannot be omitted. FlowBench's flow table generator may output the rule set either in FlowBench's default style or in ClassBench's style, and our trace generator supports both of the output styles. 

> You can also use FlowBench's trace generator to process ClassBench's output. Because other tools may not guarantee that every rule can be hit by some packets, in this case you should not specify the rule-level spatial locality.

#### Output Specification

##### Examples

`flowbench-trace -o 4096.txt_trace` (The generated trace will be output to `4096.txt_trace`)

`flowbench --classbench` (Output in ClassBench's style)

##### Description

`-o` is used to specify the output file path. If not specified, FlowBench will generate an output file automatically in the same directory as the input file. The default file name is `xxx_trace` where `xxx` is the input file. For example, FlowBench will output a `4096.txt_trace` if the input (flow table) file is `4096.txt`. 

FlowBench's trace generator also supports two types of output style. One is ClassBench's style: every field is represented by a decimal number, and the symbol `,` is used to separate them. Because FlowBench supports very wide fields, we use another style as FlowBench's default style: every field is represented by a hexadecimal number, and they are separated by spaces.

