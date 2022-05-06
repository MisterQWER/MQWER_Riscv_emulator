A program that emulates the translation of instruction and its execution from binary to assembly

USAGE: $: make riscv
       $: ./riscv -d ./code/input/R/R.input
       for other examples refer to the log.md file
       
-d: this flag will list the translations of designated binary file to its assembly file
-e: executes the instructions
-r: this tag show the trace of the registers, helping us see the modification of each instruction at each step
