# Mission-Jarvis
# MISSION JARVIS (CO PROJECT)

## Phase 1: Simple Multi-Core Processor Simulation

#### Functions we implemented
```code=assembly
ADD
ADDI
SUB
LD
LW
JAL
ADDI
LI
J
BNE
BEQ
BLE
SW
```

### Key Components:

1. *Core Class:*
   - Represents a single core in the processor.
   - Contains an array of 32 registers.
   - Program counter (pc): A vector to store the program code.
   - A boolean flag to indicate if an "EXIT" instruction is encountered.
   - A Map (labelToAddress) to store labels and their corresponding addresses.
   - The execute method interprets and executes the instructions based on the opcode and operands, handling control flow instructions like jumps and branches.

2. *Processor Class:*
   - Manages the overall simulation and contains two instances of the Core class.
   - The run method iteratively executes instructions in each core until an "EXIT" instruction is encountered in both cores.

3. *Main Function:*
   - Creates an instance of the Processor class (sim).
   - Reads program instructions from two files (test1.txt and test2.txt), initializes memory, and sets up the program code for each core.
   - Calls the run method to start the simulation.
   - Prints the contents of registers in each core after the simulation is completed.

### Instruction Set:

- ADD, SUB, LD (load), LW (load word), JAL (jump and link), ADDI (add immediate), LI (load immediate), J (jump), BNE (branch if not equal), BEQ (branch if equal), BLE (branch if less than or equal), and EXIT (to terminate the simulation).

### Memory Handling:

- The program reads instructions from files and initializes memory based on ".word" and ".string" directives.
- Memory addresses wordmemory1(starts at 500), stringmemory1(starts from 1000), wordmemory2(starts from 1500), and stringmemory2(starts from 2000) are used to store data for both cores.

### Label Handling:

- Labels are handled by storing them in the labelToAddress map, allowing efficient jumps to labeled addresses.

### Control Flow:

- Control flow instructions like jumps (J, JAL) and branches (BNE, BEQ, BLE) are implemented to modify the program counter based on conditions.

### Exit Condition:

- The simulation continues until both cores encounter an "EXIT" instruction.

### Constraints:

- Only one space is given after each register.
- Comma not allowed between registers.
- For a new line, it should not start with a space. It should start with an opcode and then register.
- When a new line starts with a label, then it should end with a semicolon.
- Hexadecimal format is not yet implemented.
- Comments should not be given for now.
- In LA, We need memory address to use that .Its still not implemented in our program. 

## Note:

The code might be expanded or modified based on the specific requirements of the simulated processor architecture and instruction set. Additionally, it assumes a sequential execution of instructions in each core without concurrency considerations.



## Phase 2: Incorporate Pipelining (With Data Forwarding)
An extension of the previous dual-core processor simulator with the addition of a basic pipeline and data forwarding mechanism to handle data hazards more efficiently.    
1. **Pipeline Stages:**
  - The pipeline is divided into five stages: IF (Instruction Fetch), ID (Instruction Decode), EXE (Execute), MEM (Memory Access), and WB (Write Back).

2. **Data Dependency Handling:**
  - After the ID stage, the code identifies the type of instruction during the decode phase ("decode" function). Depending on the instruction type, data dependencies are detected to handle hazards.
   - Data dependencies introduce clock cycles between the ID and EXE stages to resolve hazards.

3. **Pipeline Register:**
  - The `PipelineRegister` struct is introduced to model the pipeline stages, providing fields for the instruction and relevant operands (e.g., "rd", "rs1", "rs2", "imm").

4. **Data Hazard Detection:**
  - Data hazards are detected after the ID stage. If a hazard is identified and data forwarding is not enabled, the program counter (`pc`) is adjusted to introduce stalls, and the `stalls` counter is incremented.

5. **Clock Manipulation:**
  - The clock is incremented for each cycle of the main processing loop.
   - The number of instructions per cycles and stalls is printed during the simulation, providing insights into the performance of the processor.

6. **Data Forwarding:**
  - Data forwarding is an optional feature that can be enabled or disabled by the user. When enabled, it reduces stalls by forwarding data to resolve hazards.

   **Input Format:**
   
   - Input is taken in .txt format.
   - For Enabling Data Forwarding User Should Give 1 
   ----
The output includes information about each core's program counter, stalls, and instructions per cycle.

The provided code implements a simple dual-core processor simulator with basic pipelining and data forwarding features. The simulator reads assembly-like instructions from a file and executes them, simulating the behavior of a processor with multiple cores.

## Phase 3: Included Cache
A simple processor with a cache memory hierarchy. 

**Cache Class**: 
   - Represents a cache memory with methods for reading and writing data.
   - Implements two replacement policies: LRU (Least Recently Used) and FIFO (First-In-First-Out).
   - Maintains cache parameters such as size, block size, associativity, and access latency.

 **Core Class**:
   - Represents a processor core with registers, program counter (pc), and a pipeline for executing instructions.
   - Implements methods for executing instructions, handling data hazards, and forwarding data between pipeline stages.
   - Keeps track of stalls due to cache misses or data hazards.

 **Processor Class**:
   - Manages multiple processor cores and a shared cache memory.
   - Runs the simulation by executing instructions on each core and detecting stalls caused by data hazards.
   - Computes performance metrics like the number of stalls, cache miss rate, and instructions per cycle (IPC).

**Main Function**:
   - Prompts the user to enable data forwarding and input the filename containing the program instructions.
   - Reads the program from the file and initializes the cache and processor objects.

**Analysis**:

- It supports basic instruction execution (ADD, SUB, MUL, DIV, LD, ADDI, JAL, JALR) with instruction latencies defined.
- The cache implementation includes support for LRU and FIFO replacement policies, contributing to a more realistic simulation.
- The code handles data hazards and cache misses.



