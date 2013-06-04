## Project-ASM is an open commandline ASM code (subset) interpreter.

------------------------------

/// NOT COMPLETED NOW!  

co-developers:

* Li Xintian
* Yu Hanxuan
* Chen Tianyu

features: (all the opcode & hex operand should be written in lower-case

	commands:  

* mov [reg] [value / reg]
* loop [label]
* jmp [label]
* 	jg / jl / je / jne / jz / jnz [label]
* inc [reg] (increase by 1
* dec [reg] (decrease by 1
* add [reg] [hex_value] (reg_content + hex_value
* sub [reg] [hex_value] (reg_content + hex_value
* int 20 (exit
* int 21 (when ah = 1, input dl; ah = 2, output dl
* [label]

	registers:  

* ah (default as the controller for in / out
* al
* bh
* bl
* cx (loop controller
* dh
* dl (data

	comments:  

* ;

	indentation:  

* tab
* space
